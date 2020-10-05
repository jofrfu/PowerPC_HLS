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
			registers.fixed_exception_reg.exception_fields.SO = overflow;
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
