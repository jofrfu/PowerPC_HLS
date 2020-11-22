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
// Created by Jonas Fuhrmann on 18.11.20.
//

#include "floating_point_processor.hpp"
#include "floating_point_utils.hpp"

pipeline::float_result_t floating_point::move(float_move_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands) {
    ap_uint<64> source = operands.op1;
    ap_uint<64> result = source(62, 0);
    switch (decoded.operation) {
        case ::MOVE:
            result[63] = source[63];
            break;
        case NEGATE:
            result[63] = ~source[63];
            break;
        case ABSOLUTE:
            result[63] = 0;
            break;
        case NEGATIVE_ABSOLUTE:
            result[63] = 1;
            break;
    }

    if(decoded.alter_CR1) {
        copy_condition(registers);
    }

    return {result, decoded.target_reg_address, true};
}

pipeline::float_result_t floating_point::arithmetic(float_arithmetic_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands) {
    ap_uint<64> op1 = operands.op1;
    ap_uint<64> op2 = operands.op2;

    switch(decoded.operation) {
        case ADD:

            break;
        case SUBTRACT:
            break;
        case MULTIPLY:
            break;
        case DIVIDE:
            break;
    }
}

pipeline::float_result_t floating_point::multiply_add(float_madd_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands) {

}

pipeline::float_result_t floating_point::convert(float_convert_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands) {

}

pipeline::float_result_t floating_point::compare(float_compare_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands) {

}

pipeline::float_result_t floating_point::status(float_status_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands) {

}

