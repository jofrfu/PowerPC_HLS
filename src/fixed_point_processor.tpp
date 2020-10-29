// Copyright 2020 Jonas Fuhrmann. All rights reserved.
//
// This project is dual licensed under GNU General Public License version 3
// and a commercial license available on request.
//-------------------------------------------------------------------------
// For non commercial use only:
// This file is part of PowerPC_HLS.
//
// PowerPC_HLS is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// PowerPC_HLS is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with PowerPC_HLS. If not, see <http://www.gnu.org/licenses/>.

#include <cstdint>
#include <ap_int.h>
#include "ppc_types.h"

namespace fixed_point {
    template<typename T>
    void load(bool execute, load_store_decode_t decoded, registers_t &registers, T data_memory) {
        if (execute) {
            uint32_t sum1, sum2;
            if (decoded.sum1_imm) {
                sum1 = (int32_t) decoded.sum1_immediate;
            } else {
                sum1 = registers.GPR[decoded.sum1_reg_address];
            }

            if (decoded.sum2_imm) {
                sum2 = (int32_t) decoded.sum2_immediate;
            } else {
                sum2 = registers.GPR[decoded.sum2_reg_address];
            }

            uint32_t effective_address = sum1 + sum2;
            ap_uint<30> upper_address = effective_address >> 2;
            ap_uint<2> lower_address = effective_address;

            ap_uint<32> interm;
            ap_uint<32> result = 0;

            int32_t n = decoded.multiple ? 32 : decoded.result_reg_address+1;
            for(int32_t i = decoded.result_reg_address; i < n; i++) {
                switch (decoded.word_size) {
                    case 0: // Byte
                        interm = data_memory[upper_address];
                        result(7, 0) = interm((lower_address + 1) * 8 - 1, lower_address * 8);
                        if (decoded.sign_extend && result[7] == 1) {
                            result(31, 8) = 0xFFFFFF;
                        }
                        break;
                    case 1: // Halfword
                        interm = data_memory[upper_address];
                        switch (lower_address) {
                            case 0: // Halfword lies in the first two bytes of this word
                                if (decoded.little_endian) {
                                    result(7, 0) = interm(7, 0);
                                    result(15, 8) = interm(15, 8);
                                } else {
                                    result(15, 8) = interm(7, 0);
                                    result(7, 0) = interm(15, 8);
                                }
                                break;
                            case 1: // Halfword lies in the second and third byte
                                if (decoded.little_endian) {
                                    result(7, 0) = interm(15, 8);
                                    result(15, 8) = interm(23, 16);
                                } else {
                                    result(15, 8) = interm(15, 8);
                                    result(7, 0) = interm(23, 16);
                                }
                                break;
                            case 2: // Halfword lies in the last two bytes
                                if (decoded.little_endian) {
                                    result(7, 0) = interm(23, 16);
                                    result(15, 8) = interm(31, 24);
                                } else {
                                    result(15, 8) = interm(23, 16);
                                    result(7, 0) = interm(31, 24);
                                }
                                break;
                            case 3: // Halfword lies in the last byte of this word AND the first byte of the next word
                                if (decoded.little_endian) {
                                    result(7, 0) = interm(31, 24);
                                    interm = data_memory[upper_address + 1];
                                    result(15, 8) = interm(7, 0);
                                } else {
                                    result(15, 8) = interm(31, 24);
                                    interm = data_memory[upper_address + 1];
                                    result(7, 0) = interm(7, 0);
                                }
                                break;
                        }

                        if (decoded.sign_extend && result[15] == 1) {
                            result(31, 16) = 0xFFFF;
                        }
                        break;
                    case 2: // Unsupported (3 bytes)
                        break;
                    case 3: // Word
                        interm = data_memory[upper_address];
                        switch (lower_address) {
                            case 0: // Word lies in the four bytes of this word
                                if (decoded.little_endian) {
                                    result(7, 0) = interm(7, 0);
                                    result(15, 8) = interm(15, 8);
                                    result(23, 16) = interm(23, 16);
                                    result(31, 24) = interm(31, 24);
                                } else {
                                    result(31, 24) = interm(7, 0);
                                    result(23, 16) = interm(15, 8);
                                    result(15, 8) = interm(23, 16);
                                    result(7, 0) = interm(31, 24);
                                }
                                break;
                            case 1: // Word lies in the last three bytes of this word AND in the first byte of the next word
                                if (decoded.little_endian) {
                                    result(7, 0) = interm(15, 8);
                                    result(15, 8) = interm(23, 16);
                                    result(23, 16) = interm(31, 24);
                                    interm = data_memory[upper_address + 1];
                                    result(31, 24) = interm(7, 0);
                                } else {
                                    result(31, 24) = interm(15, 8);
                                    result(23, 16) = interm(23, 16);
                                    result(15, 8) = interm(31, 24);
                                    interm = data_memory[upper_address + 1];
                                    result(7, 0) = interm(7, 0);
                                }
                                break;
                            case 2: // Word lies in the last two bytes of this word AND in the first two bytes of the next word
                                if (decoded.little_endian) {
                                    result(7, 0) = interm(23, 16);
                                    result(15, 8) = interm(31, 24);
                                    interm = data_memory[upper_address + 1];
                                    result(23, 16) = interm(7, 0);
                                    result(31, 24) = interm(15, 8);
                                } else {
                                    result(31, 24) = interm(23, 16);
                                    result(23, 16) = interm(31, 24);
                                    interm = data_memory[upper_address + 1];
                                    result(15, 8) = interm(7, 0);
                                    result(7, 0) = interm(15, 8);
                                }
                                break;
                            case 3: // Word lies in the last byte of this word AND in the first three bytes of the next word
                                if (decoded.little_endian) {
                                    result(7, 0) = interm(31, 24);
                                    interm = data_memory[upper_address + 1];
                                    result(15, 8) = interm(7, 0);
                                    result(23, 16) = interm(15, 8);
                                    result(31, 24) = interm(23, 16);
                                } else {
                                    result(31, 24) = interm(31, 24);
                                    interm = data_memory[upper_address + 1];
                                    result(23, 16) = interm(7, 0);
                                    result(15, 8) = interm(15, 8);
                                    result(7, 0) = interm(23, 16);
                                }
                                break;
                        }
                        break;
                }

                registers.GPR[i] = result;
                upper_address++;
            }

            if (decoded.write_ea) {
                registers.GPR[decoded.ea_reg_address] = effective_address;
            }
        }
    }

    template<typename T>
    void store(bool execute, load_store_decode_t decoded, registers_t &registers, T data_memory) {
        if(execute) {
            uint32_t sum1, sum2;
            if(decoded.sum1_imm) {
                sum1 = (int32_t)decoded.sum1_immediate;
            } else {
                sum1 = registers.GPR[decoded.sum1_reg_address];
            }

            if(decoded.sum2_imm) {
                sum2 = (int32_t)decoded.sum2_immediate;
            } else {
                sum2 = registers.GPR[decoded.sum2_reg_address];
            }

            uint32_t effective_address = sum1 + sum2;
            ap_uint<30> upper_address = effective_address >> 2;
            ap_uint<2> lower_address = effective_address;

            ap_uint<32> result = registers.GPR[decoded.result_reg_address];

            switch(decoded.word_size) {
                case 0: // Byte
                    data_memory[upper_address]((lower_address+1)*8-1, lower_address*8) = result(7, 0);
                    break;
                case 1: // Halfword
                    switch(lower_address) {
                        case 0: // Halfword lies in the first two bytes of this word
                            if(decoded.little_endian) {
                                data_memory[upper_address](7, 0) = result(7, 0);
                                data_memory[upper_address](15, 8) = result(15, 8);
                            } else {
                                data_memory[upper_address](7, 0) = result(15, 8);
                                data_memory[upper_address](15, 8) = result(7, 0);
                            }
                            break;
                        case 1: // Halfword lies in the second and third byte
                            if(decoded.little_endian) {
                                data_memory[upper_address](15, 8) = result(7, 0);
                                data_memory[upper_address](23, 16) = result(15, 8);
                            } else {
                                data_memory[upper_address](15, 8) = result(15, 8);
                                data_memory[upper_address](23, 16) = result(7, 0);
                            }
                            break;
                        case 2: // Halfword lies in the last two bytes
                            if(decoded.little_endian) {
                                data_memory[upper_address](23, 16) = result(7, 0);
                                data_memory[upper_address](31, 24) = result(15, 8);
                            } else {
                                data_memory[upper_address](23, 16) = result(15, 8);
                                data_memory[upper_address](31, 24) = result(7, 0);
                            }
                            break;
                        case 3: // Halfword lies in the last byte of this word AND the first byte of the next word
                            if(decoded.little_endian) {
                                data_memory[upper_address](31, 24) = result(7, 0);
                                data_memory[upper_address + 1](7, 0) = result(15, 8);
                            } else {
                                data_memory[upper_address](31, 24) = result(15, 8);
                                data_memory[upper_address + 1](7, 0) = result(7, 0);
                            }
                            break;
                    }
                    break;
                case 2: // Unsupported (3 bytes)
                    break;
                case 3: // Word
                    switch(lower_address) {
                        case 0: // Word lies in the four bytes of this word
                            if(decoded.little_endian) {
                                data_memory[upper_address](7, 0) = result(7, 0);
                                data_memory[upper_address](15, 8) = result(15, 8);
                                data_memory[upper_address](23, 16) = result(23, 16);
                                data_memory[upper_address](31, 24) = result(31, 24);
                            } else {
                                data_memory[upper_address](7, 0) = result(31, 24);
                                data_memory[upper_address](15, 8) = result(23, 16);
                                data_memory[upper_address](23, 16) = result(15, 8);
                                data_memory[upper_address](31, 24) = result(7, 0);
                            }
                            break;
                        case 1: // Word lies in the last three bytes of this word AND in the first byte of the next word
                            if(decoded.little_endian) {
                                data_memory[upper_address](15, 8) = result(7, 0);
                                data_memory[upper_address](23, 16) = result(15, 8);
                                data_memory[upper_address](31, 24) = result(23, 16);
                                data_memory[upper_address + 1](7, 0) = result(31, 24);
                            } else {
                                data_memory[upper_address](15, 8) = result(31, 24);
                                data_memory[upper_address](23, 16) = result(23, 16);
                                data_memory[upper_address](31, 24) = result(15, 8);
                                data_memory[upper_address + 1](7, 0) = result(7, 0);
                            }
                            break;
                        case 2: // Word lies in the last two bytes of this word AND in the first two bytes of the next word
                            if(decoded.little_endian) {
                                data_memory[upper_address](23, 16) = result(7, 0);
                                data_memory[upper_address](31, 24) = result(15, 8);
                                data_memory[upper_address + 1](7, 0) = result(23, 16);
                                data_memory[upper_address + 1](15, 8) = result(31, 24);
                            } else {
                                data_memory[upper_address](23, 16) = result(31, 24);
                                data_memory[upper_address](31, 24) = result(23, 16);
                                data_memory[upper_address + 1](7, 0) = result(15, 8);
                                data_memory[upper_address + 1](15, 8) = result(7, 0);
                            }
                            break;
                        case 3: // Word lies in the last byte of this word AND in the first three bytes of the next word
                            if(decoded.little_endian) {
                                data_memory[upper_address](31, 24) = result(7, 0);
                                data_memory[upper_address + 1](7, 0) = result(15, 8);
                                data_memory[upper_address + 1](15, 8) = result(23, 16);
                                data_memory[upper_address + 1](23, 16) = result(31, 24);
                            } else {
                                data_memory[upper_address](31, 24) = result(31, 24);
                                data_memory[upper_address + 1](7, 0) = result(23, 16);
                                data_memory[upper_address + 1](15, 8) = result(15, 8);
                                data_memory[upper_address + 1](23, 16) = result(7, 0);
                            }
                            break;
                    }
                    break;
            }

            if(decoded.write_ea) {
                registers.GPR[decoded.ea_reg_address] = effective_address;
            }
        }
    }

    template<typename T>
    void load_string();
}