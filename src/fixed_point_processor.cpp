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

#include "fixed_point_processor.hpp"

void load(bool execute, load_store_decode_t decoded, registers_t &registers, ap_uint<32> *data_memory) {
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

		ap_uint<32> interm;
		ap_uint<32> result = 0;
		switch(decoded.word_size) {
			case 0: // Byte
				interm = data_memory[upper_address];
				result(0, 7) = interm(lower_address*8, (lower_address+1)*8);
				if(decoded.sign_extend) {
					for(uint32_t i = 8; i < 32; i++) {
						result[i] = result[7];
					}
				}
				break;
			case 1: // Halfword
				interm = data_memory[upper_address];
				switch(lower_address) {
					case 0: // Halfword lies in the first two bytes of this word
						result(8, 15) = interm(0, 7);
						result(0, 7) = interm(8, 15);
						break;
					case 1: // Halfword lies in the second and third byte
						result(8, 15) = interm(8, 15);
						result(0, 7) = interm(16, 23);
						break;
					case 2: // Halfword lies in the last two bytes
						result(8, 15) = interm(16, 23);
						result(0, 7) = interm(24, 31);
						break;
					case 3: // Halfword lies in the last byte of this word AND the first byte of the next word
						result(8, 15) = interm(24, 31);
						interm = data_memory[upper_address+1];
						result(0, 7) = interm(0, 7);
						break;
				}

				if(decoded.sign_extend) {
					for(uint32_t i = 16; i < 32; i++) {
						result[i] = result[15];
					}
				}
				break;
			case 2: // Unsupported (3 bytes)
				break;
			case 3: // Word
				interm = data_memory[upper_address];
				switch(lower_address) {
					case 0: // Word lies in the four bytes of this word
						result(24, 31) = interm(0, 7);
						result(16, 23) = interm(8, 15);
						result(8, 15) = interm(16, 23);
						result(0, 7) = interm(24, 31);
						break;
					case 1: // Word lies in the last three bytes of this word AND in the first byte of the next word
						result(24, 31) = interm(8, 15);
						result(16, 23) = interm(16, 23);
						result(8, 15) = interm(24, 31);
						interm = data_memory[upper_address+1];
						result(0, 7) = interm(0, 7);
						break;
					case 2: // Word lies in the last two bytes of this word AND in the first two bytes of the next word
						result(24, 31) = interm(16, 23);
						result(16, 23) = interm(24, 31);
						interm = data_memory[upper_address+1];
						result(8, 15) = interm(0, 7);
						result(0, 7) = interm(8, 15);
						break;
					case 3: // Word lies in the last byte of this word AND in the first three bytes of the next word
						result(24, 31) = interm(24, 31);
						interm = data_memory[upper_address+1];
						result(16, 23) = interm(0, 7);
						result(8, 15) = interm(8, 15);
						result(0, 7) = interm(16, 23);
						break;
				}
				break;
		}

		registers.GPR[decoded.result_reg_address] = result;

		if(decoded.write_ea) {
			registers.GPR[decoded.ea_reg_address] = effective_address;
		}
	}
}

void store(bool execute, load_store_decode_t decoded, registers_t &registers, ap_uint<32> *data_memory) {
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
				data_memory[upper_address](lower_address*8, (lower_address+1)*8) = result(0, 7);
				break;
			case 1: // Halfword
				switch(lower_address) {
					case 0: // Halfword lies in the first two bytes of this word
						data_memory[upper_address](0, 7) = result(8, 15);
						data_memory[upper_address](8, 15) = result(0, 7);
						break;
					case 1: // Halfword lies in the second and third byte
						data_memory[upper_address](8, 15) = result(8, 15);
						data_memory[upper_address](16, 23) = result(0, 7);
						break;
					case 2: // Halfword lies in the last two bytes
						data_memory[upper_address](16, 23) = result(8, 15);
						data_memory[upper_address](24, 31) = result(0, 7);
						break;
					case 3: // Halfword lies in the last byte of this word AND the first byte of the next word
						data_memory[upper_address](24, 31) = result(8, 15);
						data_memory[upper_address+1](0, 7) = result(0, 7);
						break;
				}
				break;
			case 2: // Unsupported (3 bytes)
				break;
			case 3: // Word
				switch(lower_address) {
					case 0: // Word lies in the four bytes of this word
						data_memory[upper_address](0, 7) = result(24, 31);
						data_memory[upper_address](8, 15) = result(16, 23);
						data_memory[upper_address](16, 23) = result(8, 15);
						data_memory[upper_address](24, 31) = result(0, 7);
						break;
					case 1: // Word lies in the last three bytes of this word AND in the first byte of the next word
						data_memory[upper_address](8, 15) = result(24, 31);
						data_memory[upper_address](16, 23) = result(16, 23);
						data_memory[upper_address](24, 31) = result(8, 15);
						data_memory[upper_address+1](0, 7) = result(0, 7);
						break;
					case 2: // Word lies in the last two bytes of this word AND in the first two bytes of the next word
						data_memory[upper_address](16, 23) = result(24, 31);
						data_memory[upper_address](24, 31) = result(16, 23);
						data_memory[upper_address+1](0, 7) = result(8, 15);
						data_memory[upper_address+1](8, 15) = result(0, 7);
						break;
					case 3: // Word lies in the last byte of this word AND in the first three bytes of the next word
						data_memory[upper_address](24, 31) = result(24, 31);
						data_memory[upper_address+1](0, 7) = result(16, 23);
						data_memory[upper_address+1](8, 15) = result(8, 15);
						data_memory[upper_address+1](16, 23) = result(0, 7);
						break;
				}
				break;
		}

		if(decoded.write_ea) {
			registers.GPR[decoded.ea_reg_address] = effective_address;
		}
	}
}

void add_sub(bool execute, add_sub_decode_t decoded, registers_t &registers) {
	if(execute) {
		ap_uint<32> op1;
		ap_uint<32> op2;

		if(decoded.op1_imm) {
			op1 = decoded.op1_immediate;
		} else {
			op1 = registers.GPR[decoded.op1_reg_address];
		}

		if(decoded.op2_imm) {
			op2 = decoded.op2_immediate;
		} else {
			op2 = registers.GPR[decoded.op2_reg_address];
		}

		ap_uint<33> result;
		ap_int<3> op3;

		if(decoded.subtract) {
			// Two's complement
			op1 = ~op1;
			if(decoded.sub_one) {
				if(decoded.add_CA) {
					if(registers.fixed_exception_reg.exception_fields.CA == 1) {
						// 1 - 1 + 1 == 1
						op3 = 1;
					} else {
						// 1 - 1 + 0 == 0
						op3 = 0;
					}
				}
			} else {
				if(decoded.add_CA) {
					if(registers.fixed_exception_reg.exception_fields.CA == 1) {
						// 1 - 0 + 1 == 2
						op3 = 2;
					} else {
						// 1 - 0 + 0 == 1
						op3 = 1;
					}
				}
			}
		} else {
			if(decoded.sub_one) {
				if(decoded.add_CA) {
					if(registers.fixed_exception_reg.exception_fields.CA == 1) {
						// 0 - 1 + 1 == 0
						op3 = 0;
					} else {
						// 0 - 1 + 0 == -1
						op3 = -1;
					}
				}
			} else {
				if(decoded.add_CA) {
					if(registers.fixed_exception_reg.exception_fields.CA == 1) {
						// 0 - 0 + 1 == 1
						op3 = 1;
					} else {
						// 0 - 0 + 0 == 0
						op3 = 0;
					}
				}
			}
		}

		result = op1 + op2 + op3;

		ap_uint<1> carry = result[32];
		ap_uint<1> overflow;

		// Because there is no way to access the carry bits directly, this is used as a workaround
		if(	(op1[31] == 1 && op2[31] == 1 && result[31] == 0) ||
			(op1[31] == 0 && op2[31] == 0 && result[31] == 1)) {
			overflow = 1;
		} else {
			overflow = 0;
		}

		if(decoded.alter_CA) {
			registers.fixed_exception_reg.exception_fields.CA = carry;
		}

		if(decoded.alter_OV) {
			registers.fixed_exception_reg.exception_fields.OV = overflow;
			// SO bit is sticky
			if(registers.fixed_exception_reg.exception_fields.SO == 0) {
				registers.fixed_exception_reg.exception_fields.SO = overflow;
			}
		}

		if(decoded.alter_CR0) {
			if(result == 0) {
				// CR0 is at position 7
				registers.condition_reg.CR[7].condition_fixed_point.LT = 0;
				registers.condition_reg.CR[7].condition_fixed_point.GT = 0;
				registers.condition_reg.CR[7].condition_fixed_point.EQ = 1;
			} else if(result[32] == 1) { // < 0
				// CR0 is at position 7
				registers.condition_reg.CR[7].condition_fixed_point.LT = 1;
				registers.condition_reg.CR[7].condition_fixed_point.GT = 0;
				registers.condition_reg.CR[7].condition_fixed_point.EQ = 0;
			} else { // > 0
				// CR0 is at position 7
				registers.condition_reg.CR[7].condition_fixed_point.LT = 0;
				registers.condition_reg.CR[7].condition_fixed_point.GT = 1;
				registers.condition_reg.CR[7].condition_fixed_point.EQ = 0;
			}

			// Copy the SO field from XER into CR0
			registers.condition_reg.CR[7].condition_fixed_point.SO = registers.fixed_exception_reg.exception_fields.SO;
		}

		registers.GPR[decoded.result_reg_address] = result;
	}
}
