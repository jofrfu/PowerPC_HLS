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

#include <hls_math.h>

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
    if(decoded.operation == SUBTRACT) {
        // Sub is just an add with negation
        operands.op2[63] = ~operands.op2[63];
    }
    double op1 = convert_to_double(operands.op1);
    double op2 = convert_to_double(operands.op2);
    double result;

    ap_uint<1> VE = registers.FPSCR.VE;

    bool exception = false;

    if(isnan(op1) || isnan(op2)) {
        exception = true;
        // Calculating with NaNs is always invalid
        if(VE == 1) {
            result = std::numeric_limits<double>::signaling_NaN();
        } else {
            result = std::numeric_limits<double>::quiet_NaN();
        }
    } else {
        // We don't care if it's single precision or double precision
        switch (decoded.operation) {
            case SUBTRACT:
            case ADD:
                if (isinf(op1) && isinf(op2) && (isless(op1, 0.0) ^ isless(op2, 0.0))) {
                    exception = true;
                    // Calculating inf - inf is invalid
                    if (VE == 1) {
                        registers.FPSCR.VXISI = 1;
                        result = std::numeric_limits<double>::signaling_NaN();
                    } else {
                        result = std::numeric_limits<double>::quiet_NaN();
                    }
                } else {
                    result = op1 + op2;
                }
                break;
            case MULTIPLY:
                if ((isinf(op1) ^ isinf(op2)) && (op1 == 0.0 ^ op2 == 0.0)) {
                    exception = true;
                    // Calculating inf * 0 is invalid
                    if (VE == 1) {
                        registers.FPSCR.VXIMZ = 1;
                        result = std::numeric_limits<double>::signaling_NaN();
                    } else {
                        result = std::numeric_limits<double>::quiet_NaN();
                    }
                } else {
                    result = op1 * op2;
                }
                break;
            case DIVIDE:
                if (op2 == 0.0) {
                    exception = true;
                    // Calculating x / 0 results in QNaN
                    if (VE == 1) {
                        if (op1 == 0.0) {
                            // Calculating 0 / 0 is invalid
                            registers.FPSCR.VXZDZ = 1;
                            result = std::numeric_limits<double>::signaling_NaN();
                        } else {
                            result = std::numeric_limits<double>::quiet_NaN();
                        }
                    } else {
                        result = std::numeric_limits<double>::quiet_NaN();
                    }
                } else if (isinf(op1) && isinf(op2)) {
                    exception = true;
                    // Calculating inf / inf is invalid
                    if (VE == 1) {
                        registers.FPSCR.VXIDI = 1;
                        result = std::numeric_limits<double>::signaling_NaN();
                    } else {
                        result = std::numeric_limits<double>::quiet_NaN();
                    }
                } else {
                    result = op1 / op2;
                }
                break;
        }
    }

    pipeline::float_result_t write_back;
    write_back.result = convert_to_uint(result);
    write_back.address = decoded.result_reg_address;

    // Check conditions and set exceptions accordingly
    if(VE) {
        if(result == std::numeric_limits<double>::signaling_NaN()) {
            registers.FPSCR.VXSNAN = 1;
            write_back.write_back = false;
            exception = true;
        } else {
            write_back.write_back = true;
        }

        if(exception) {
            registers.FPSCR.FR = 0;
            registers.FPSCR.FI = 0;
        }
    } else {
        if(exception) {
            registers.FPSCR.FR = 0;
            registers.FPSCR.FI = 0;
            registers.FPSCR.FPRF = E_QNAN;
        }
    }

    return write_back;
}

pipeline::float_result_t floating_point::multiply_add(float_madd_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands) {

}

pipeline::float_result_t floating_point::convert(float_convert_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands) {

}

pipeline::float_result_t floating_point::compare(float_compare_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands) {

}

pipeline::float_result_t floating_point::status(float_status_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands) {

}

