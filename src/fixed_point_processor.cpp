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
#include "fixed_point_utils.hpp"

void fixed_point::load(bool execute, load_store_decode_t decoded, registers_t &registers, ap_uint<32> *data_memory) {
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

void fixed_point::store(bool execute, load_store_decode_t decoded, registers_t &registers, ap_uint<32> *data_memory) {
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

void fixed_point::add_sub(bool execute, add_sub_decode_t decoded, registers_t &registers) {
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

		registers.GPR[decoded.result_reg_address] = result;registers.GPR[decoded.result_reg_address] = result;

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
			fixed_point::set_overflow(overflow, registers);
		}

		if(decoded.alter_CR0) {
			fixed_point::check_condition(result, registers);
			// Copy the SO field from XER into CR0
			fixed_point::copy_summary_overflow(registers);
		}
	}
}

void fixed_point::multiply(bool execute, mul_decode_t decoded, registers_t &registers) {
	if (execute) {

	}
}

void fixed_point::divide(bool execute, div_decode_t decoded, registers_t &registers) {
	if(execute) {
		ap_int<33> signed_dividend;
		ap_int<33> signed_divisor;
		signed_dividend(0, 31) = registers.GPR[decoded.dividend_reg_address];
		signed_divisor(0, 31) = registers.GPR[decoded.divisor_reg_address];
		if(decoded.div_signed) {
			// sign extend
			signed_dividend[32] = signed_dividend[31];
			signed_divisor[32] = signed_divisor[31];
		} else {
			// zero extend
			signed_dividend[32] = 0;
			signed_divisor[32] = 0;
		}

		ap_int<32> quotient = signed_dividend / signed_divisor;

		registers.GPR[decoded.result_reg_address] = quotient;

		ap_uint<1> overflow;

		if(signed_divisor == 0 || (signed_divisor == -1 && signed_dividend(0, 31) == 0x80000000)) {
			// divide by zero and the most negative number divided by -1 (the result wouldn't fit in 32 bits) is undefined
			overflow = registers.fixed_exception_reg.exception_fields.OV = 1;
		} else {
			overflow = registers.fixed_exception_reg.exception_fields.OV = 0;
		}

		if(decoded.alter_OV) {
			fixed_point::set_overflow(overflow, registers);
		}

		if(decoded.alter_CR0) {
			fixed_point::check_condition(quotient, registers);
			// Copy the SO field from XER into CR0
			fixed_point::copy_summary_overflow(registers);
		}
	}
}

void fixed_point::compare(bool execute, cmp_decode_t decoded, registers_t &registers) {
	if(execute) {
		ap_int<33> op1, op2;

		op1(0, 31) = registers.GPR[decoded.op1_reg_address];
		if(decoded.op2_imm) {
			op2(0, 31) = decoded.op2_immediate;
		} else {
			op2(0, 31) = registers.GPR[decoded.op2_reg_address];
		}

		if(decoded.cmp_signed) {
			op1[32] = op1[31];
			op2[32] = op1[31];
		} else {
			op1[32] = 0;
			if(decoded.op2_imm) { // If immediate is being used, it will always be sign extended
				op2[32] = op2[31];
			} else {
				op2[32] = 0;
			}
		}

		// Convert, because the fields lie in opposite order
		ap_uint<5> BF = ((1<<5)-1) - decoded.BF;

		if(op1 < op2) {
			registers.condition_reg.CR[BF].condition_fixed_point.LT = 1;
			registers.condition_reg.CR[BF].condition_fixed_point.GT = 0;
			registers.condition_reg.CR[BF].condition_fixed_point.EQ = 0;
		} else if(op1 > op2) {
			registers.condition_reg.CR[BF].condition_fixed_point.LT = 0;
			registers.condition_reg.CR[BF].condition_fixed_point.GT = 1;
			registers.condition_reg.CR[BF].condition_fixed_point.EQ = 0;
		} else { // op1 == op2
			registers.condition_reg.CR[BF].condition_fixed_point.LT = 0;
			registers.condition_reg.CR[BF].condition_fixed_point.GT = 0;
			registers.condition_reg.CR[BF].condition_fixed_point.EQ = 1;
		}

		// Copy SO into the given SPR
		registers.condition_reg.CR[decoded.BF].condition_fixed_point.SO = registers.fixed_exception_reg.exception_fields.SO;
	}
}

bool fixed_point::trap(bool execute, trap_decode_t decoded, registers_t &registers) {
	if(execute) {
		int32_t op1 = registers.GPR[decoded.op1_reg_address];
		int32_t op2;

		if(decoded.op2_imm) {
			op2 = decoded.op2_immediate;
		} else {
			op2 = registers.GPR[decoded.op2_reg_address];
		}

		uint32_t u_op1 = op1;
		uint32_t u_op2 = op2;

		ap_uint<5> TO = decoded.TO;

		return	(
				(op1   < op2   && TO[4] == 1) ||
				(op1   > op2   && TO[3] == 1) ||
				(op1  == op2   && TO[2] == 1) ||
				(u_op1 < u_op2 && TO[1] == 1) ||
				(u_op1 > u_op2 && TO[0] == 1)
				);
	}

	return false;
}
