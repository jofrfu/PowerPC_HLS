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


#ifndef __FIXED_POINT_PROCESSOR__
#define __FIXED_POINT_PROCESSOR__

#include <stdint.h>
#include <ap_int.h>
#include "ppc_types.h"
#include "pipeline.hpp"

namespace fixed_point {
    template<typename T>
    pipeline::result_t load(load_store_decode_t decoded, registers_t &registers, pipeline::operands_t operands, T data_memory) {
        uint32_t sum1 = operands.op1;
        uint32_t sum2 = operands.op2;

        ap_uint<32> effective_address = sum1 + sum2;
        ap_uint<30> upper_address = effective_address(31, 2);
        ap_uint<2> lower_address = effective_address;

        ap_uint<32> interm;
        ap_uint<32> result = 0;

        int32_t n;
        if (decoded.multiple) {
            n = 32;
        } else {
            n = decoded.result_reg_address + 1;
        }

        for (int32_t i = decoded.result_reg_address; i < n; i++) {
#pragma HLS loop_tripcount min=1 max=32 avg=16
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

        pipeline::result_t write_back = {effective_address, 0, false, false};

        if (decoded.write_ea) {
            write_back.address = decoded.ea_reg_address;
            write_back.write_back = true;
        }

        return write_back;
    }

    template<typename T>
    pipeline::result_t store(load_store_decode_t decoded, registers_t &registers, pipeline::operands_t operands, T data_memory) {
        uint32_t sum1 = operands.op1;
        uint32_t sum2 = operands.op2;

        ap_uint<32> effective_address = sum1 + sum2;
        ap_uint<30> upper_address = effective_address(31, 2);
        ap_uint<2> lower_address = effective_address;

        int32_t n;
        if (decoded.multiple) {
            n = 32;
        } else {
            n = decoded.result_reg_address + 1;
        }

        for (int32_t i = decoded.result_reg_address; i < n; i++) {
#pragma HLS loop_tripcount min=1 max=32 avg=16
            ap_uint<32> result = registers.GPR[i];
            switch (decoded.word_size) {
                case 0: // Byte
                    data_memory[upper_address]((lower_address + 1) * 8 - 1, lower_address * 8) = result(7, 0);
                    break;
                case 1: // Halfword
                    switch (lower_address) {
                        case 0: // Halfword lies in the first two bytes of this word
                            if (decoded.little_endian) {
                                data_memory[upper_address](7, 0) = result(7, 0);
                                data_memory[upper_address](15, 8) = result(15, 8);
                            } else {
                                data_memory[upper_address](7, 0) = result(15, 8);
                                data_memory[upper_address](15, 8) = result(7, 0);
                            }
                            break;
                        case 1: // Halfword lies in the second and third byte
                            if (decoded.little_endian) {
                                data_memory[upper_address](15, 8) = result(7, 0);
                                data_memory[upper_address](23, 16) = result(15, 8);
                            } else {
                                data_memory[upper_address](15, 8) = result(15, 8);
                                data_memory[upper_address](23, 16) = result(7, 0);
                            }
                            break;
                        case 2: // Halfword lies in the last two bytes
                            if (decoded.little_endian) {
                                data_memory[upper_address](23, 16) = result(7, 0);
                                data_memory[upper_address](31, 24) = result(15, 8);
                            } else {
                                data_memory[upper_address](23, 16) = result(15, 8);
                                data_memory[upper_address](31, 24) = result(7, 0);
                            }
                            break;
                        case 3: // Halfword lies in the last byte of this word AND the first byte of the next word
                            if (decoded.little_endian) {
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
                    switch (lower_address) {
                        case 0: // Word lies in the four bytes of this word
                            if (decoded.little_endian) {
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
                            if (decoded.little_endian) {
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
                            if (decoded.little_endian) {
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
                            if (decoded.little_endian) {
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
            upper_address++;
        }

        pipeline::result_t write_back = {effective_address, 0, false, false};

        if (decoded.write_ea) {
            write_back.address = decoded.ea_reg_address;
            write_back.write_back = true;
        }

        return write_back;
    }

    template<typename T>
    void load_string(load_store_decode_t decoded, registers_t &registers, pipeline::operands_t operands, T data_memory) {
        uint32_t sum1 = operands.op1;
        uint32_t sum2 = operands.op2;
        ap_uint<7> n = operands.op3;
        ap_uint<5> r = decoded.result_reg_address - 1;

        ap_uint<32> ea = sum1 + sum2;
        for (ap_uint<2> i = 3; n > 0; i--, n--, ea++) {
//#pragma HLS unroll factor=4 TODO: write the loop in a way, that 4 bytes accesses the memory once
#pragma HLS loop_tripcount min=1 max=120 avg=32
            if (i == 3) {
                r++;
                registers.GPR[r] = 0;
            }
            ap_uint<30> upper_address = ea(31, 2);
            ap_uint<2> lower_address = ea;
            registers.GPR[r]((i + 1) * 8 - 1, i * 8) =
                    data_memory[upper_address]((lower_address + 1) * 8 - 1,lower_address * 8);
        }
    }

    template<typename T>
    void store_string(load_store_decode_t decoded, registers_t &registers, pipeline::operands_t operands, T data_memory) {
        uint32_t sum1 = operands.op1;
        uint32_t sum2 = operands.op2;
        ap_uint<7> n = operands.op3;
        ap_uint<5> r = decoded.result_reg_address - 1;

        ap_uint<32> ea = sum1 + sum2;
        for (ap_uint<2> i = 3; n > 0; i--, n--, ea++) {
//#pragma HLS unroll factor=4 TODO: write the loop in a way, that 4 bytes accesses the memory once
#pragma HLS loop_tripcount min=1 max=120 avg=32
            if (i == 3) {
                r++;
            }
            ap_uint<30> upper_address = ea(31, 2);
            ap_uint<2> lower_address = ea;
            data_memory[upper_address]((lower_address + 1) * 8 - 1, lower_address * 8) =
                    registers.GPR[r]((i + 1) * 8 - 1, i * 8);
        }
    }

    pipeline::result_t add_sub(add_sub_decode_t decoded, registers_t &registers, pipeline::operands_t operands);

    pipeline::result_t multiply(mul_decode_t decoded, registers_t &registers, pipeline::operands_t operands);

    pipeline::result_t divide(div_decode_t decoded, registers_t &registers, pipeline::operands_t operands);

    pipeline::result_t compare(cmp_decode_t decoded, registers_t &registers, pipeline::operands_t operands);

    pipeline::result_t trap(trap_decode_t decoded, registers_t &registers, pipeline::operands_t operands);

    pipeline::result_t logical(log_decode_t decoded, registers_t &registers, pipeline::operands_t operands);

    pipeline::result_t rotate(rotate_decode_t decoded, registers_t &registers, pipeline::operands_t operands);

    pipeline::result_t system(system_decode_t decoded, registers_t &registers, pipeline::operands_t operands);

}

#endif
