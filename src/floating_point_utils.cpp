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

//
// Created by Jonas Fuhrmann on 21.11.20.
//

#include "floating_point_utils.hpp"

void floating_point::copy_condition(registers_t &registers) {
    registers.condition_reg[1].condition_floating_point.OX = registers.FPSCR.OX;
    registers.condition_reg[1].condition_floating_point.VX = registers.FPSCR.VX;
    registers.condition_reg[1].condition_floating_point.FEX = registers.FPSCR.FEX;
    registers.condition_reg[1].condition_floating_point.FX = registers.FPSCR.FX;
}

double floating_point::convert_to_double(ap_uint<64> val) {
    convert_t convert;
    convert.x = val;
    return convert.y;
}

ap_uint<64> floating_point::convert_to_uint(double val) {
    convert_t convert;
    convert.y = val;
    return convert.x;
}

void floating_point::check_exceptions(double result, registers_t &registers) {
    if(isnan(result)) {

    }
    if(isinf(result)) {

    }
}