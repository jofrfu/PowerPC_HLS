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
#include <ap_int.h>

void add_sub(bool execute, add_sub_decode_t decoded, uint32_t *GPR) {
	if(execute) {
		int32_t op1;
		int32_t op2;

		if(decoded.op1_imm) {
			op1 = decoded.op1_immediate;
		} else {
			op1 = GPR[decoded.op1_reg_address];
		}

		if(decoded.op2_imm) {
			op2 = decoded.op2_immediate;
		} else {
			op2 = GPR[decoded.op2_reg_address];
		}

		ap_int<33> result;

		if(decoded.subtract) {
			// Two's complement
			op1 = ~op1;
		}

		result = op1 + op2;

		if(decoded.subtract) {
			// Two's complement
			result += 1;
		}

		ap_int<1> carry = result[32];

		// TODO: Add carry and CR assignment

		GPR[decoded.result_reg_address] = result;
	}
}
