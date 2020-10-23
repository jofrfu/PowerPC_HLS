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

#include "fixed_point_utils.hpp"

condition_field_t& fixed_point::select_CR(ap_uint<3> address, registers_t &registers) {
    switch(address) {
        case 0:
            return registers.condition_reg.CR.CR0;
        case 1:
            return registers.condition_reg.CR.CR1;
        case 2:
            return registers.condition_reg.CR.CR2;
        case 3:
            return registers.condition_reg.CR.CR3;
        case 4:
            return registers.condition_reg.CR.CR4;
        case 5:
            return registers.condition_reg.CR.CR5;
        case 6:
            return registers.condition_reg.CR.CR6;
        case 7:
            return registers.condition_reg.CR.CR7;
    }
    // This can't happen
    return registers.condition_reg.CR.CR0;
}

void fixed_point::check_condition(int32_t result, registers_t &registers) {
    condition_field_t &CR = select_CR(0, registers);
	if(result == 0) {
		// CR0 is at position 7
		CR.condition_fixed_point.LT = 0;
        CR.condition_fixed_point.GT = 0;
        CR.condition_fixed_point.EQ = 1;
	} else if(result < 0) { // < 0
		// CR0 is at position 7
        CR.condition_fixed_point.LT = 1;
        CR.condition_fixed_point.GT = 0;
        CR.condition_fixed_point.EQ = 0;
	} else { // > 0
		// CR0 is at position 7
        CR.condition_fixed_point.LT = 0;
        CR.condition_fixed_point.GT = 1;
        CR.condition_fixed_point.EQ = 0;
	}
}

void fixed_point::copy_summary_overflow(registers_t &registers) {
    select_CR(0, registers).condition_fixed_point.SO = registers.fixed_exception_reg.exception_fields.SO;
}

void fixed_point::set_overflow(ap_uint<1> overflow, registers_t &registers) {
	registers.fixed_exception_reg.exception_fields.OV = overflow;
	// SO bit is sticky
	if(registers.fixed_exception_reg.exception_fields.SO == 0) {
		registers.fixed_exception_reg.exception_fields.SO = overflow;
	}
}
