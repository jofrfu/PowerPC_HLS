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

pipeline::float_result_t
floating_point::move(float_move_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands) {
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

    if (decoded.alter_CR1) {
        copy_condition(registers);
    }

    return {result, decoded.target_reg_address, true};
}

pipeline::float_result_t floating_point::arithmetic(float_arithmetic_decode_t decoded, registers_t &registers,
                                                    pipeline::float_operands_t operands) {
    if (decoded.operation == SUBTRACT) {
        // Sub is just an add with negation
        operands.op2[63] = ~operands.op2[63];
    }
    double op1 = convert_to_double(operands.op1);
    double op2 = convert_to_double(operands.op2);
    double result;

    ap_uint<1> VE = registers.FPSCR.VE;
    ap_uint<1> ZE = registers.FPSCR.ZE;
    ap_uint<1> OE = registers.FPSCR.OE;

    bool exception = false;
    bool zero_exception = false;

    if (isnan(op1) || isnan(op2)) {
        exception = true;
        // Calculating with NaNs is always invalid
        if (VE == 1) {
            result = std::numeric_limits<double>::signaling_NaN();
        } else {
            result = std::numeric_limits<double>::quiet_NaN();
        }
    } else {
        // We don't care if it's single precision or double precision
        switch (decoded.operation) {
            case SUBTRACT:
            case ADD:
                if (isinf(op1) && isinf(op2) && (operands.op1[63] ^ operands.op2[63])) {
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
                if (op1 == 0.0 && op2 == 0.0) {
                    exception = true;
                    // Calculating 0 / 0 is invalid
                    if (VE == 1) {
                        registers.FPSCR.VXZDZ = 1;
                        result = std::numeric_limits<double>::signaling_NaN();
                    } else {
                        result = std::numeric_limits<double>::quiet_NaN();
                    }
                } else if (op1 != 0.0 && op2 == 0.0) {
                    zero_exception = true;
                    // Calculating x / zero is invalid
                    registers.FPSCR.ZX = 1;
                    result = std::numeric_limits<double>::infinity();
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
    ap_uint<64> bit_result = convert_to_uint(result);

    write_back.write_back = true;

    bool ov_exception = isfinite(op1) && isfinite(op2) && isinf(result) && !zero_exception;
    // Underflow is impossible to check with this style of code, it will be ignored
    // Inexact will not be covered, because we ignore rounding

    if (ov_exception) {
        registers.FPSCR.OX = 1;
    }

    write_back.write_back = true;

    // Check conditions and set exceptions accordingly
    if (VE == 0 && ZE == 0 && OE == 0) {
        if (exception) {
            registers.FPSCR.FR = 0;
            registers.FPSCR.FI = 0;
            registers.FPSCR.FPRF = E_QNAN;

            registers.FPSCR.FX = 1;
        } else if (zero_exception) {
            registers.FPSCR.FR = 0;
            registers.FPSCR.FI = 0;
            // Set sign bit of infinity result
            bit_result[63] = operands.op1[63] ^ operands.op2[63];

            if (bit_result[63] == 1) {
                registers.FPSCR.FPRF = E_N_INF;
            } else {
                registers.FPSCR.FPRF = E_P_INF;
            }

            registers.FPSCR.FX = 1;
        } else if (ov_exception) {
            registers.FPSCR.OX = 1;
            registers.FPSCR.XX = 1;
            registers.FPSCR.FI = 1;

            // Ignore rounding and only allow infinity here
            if (bit_result[63] == 1) {
                registers.FPSCR.FPRF = E_N_INF;
            } else {
                registers.FPSCR.FPRF = E_P_INF;
            }

            registers.FPSCR.FX = 1;
        } else {
            registers.FPSCR.FPRF = get_FPRF(result, bit_result[63]);
        }
    } else {
        if (VE == 1) {
            if (result == std::numeric_limits<double>::signaling_NaN()) {
                registers.FPSCR.VXSNAN = 1;
                write_back.write_back = false;
                exception = true;
            }

            if (exception) {
                registers.FPSCR.FR = 0;
                registers.FPSCR.FI = 0;

                registers.FPSCR.FX = 1;
            }
        }

        if (ZE == 1) {
            if (zero_exception) {
                registers.FPSCR.FR = 0;
                registers.FPSCR.FI = 0;
                write_back.write_back = false;

                registers.FPSCR.FX = 1;
            }
        }

        if (OE == 1) {
            if (ov_exception) {
                registers.FPSCR.OX = 1;
                // We only care for double precision, correct the exponent by 1536
                bit_result(62, 52) = bit_result(62, 52) - 1536;
                if (bit_result[63] == 1) {
                    registers.FPSCR.FPRF = E_N_NORM;
                } else {
                    registers.FPSCR.FPRF = E_P_NORM;
                }

                registers.FPSCR.FX = 1;
            }
        }
    }

    if (decoded.alter_CR1) {
        copy_condition(registers);
    }

    write_back.result = bit_result;
    write_back.address = decoded.result_reg_address;

    return write_back;
}

pipeline::float_result_t
floating_point::multiply_add(float_madd_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands) {
    double op1 = convert_to_double(operands.op1);
    double op2 = convert_to_double(operands.op2);
    if (decoded.negate_add) {
        operands.op3[63] = ~operands.op3[63];
    }
    double op3 = convert_to_double(operands.op3);

    ap_uint<1> VE = registers.FPSCR.VE;
    ap_uint<1> OE = registers.FPSCR.OE;

    bool exception = false;

    double result;

    if ((isinf(op1) ^ isinf(op2)) && (op1 == 0.0 ^ op2 == 0.0)) {
        exception = true;
        // Calculating inf * 0 is invalid
        if (VE == 1) {
            registers.FPSCR.VXIMZ = 1;
            result = std::numeric_limits<double>::signaling_NaN();
        } else {
            result = std::numeric_limits<double>::quiet_NaN();
        }
    } else if ((isinf(op1) || isinf(op2)) && isinf(op3) && ((operands.op1[63] ^ operands.op2[63]) ^ operands.op3[63])) {
        exception = true;
        // Calculating inf - inf i invalid
        if (VE == 1) {
            registers.FPSCR.VXISI = 1;
            result = std::numeric_limits<double>::signaling_NaN();
        } else {
            result = std::numeric_limits<double>::quiet_NaN();
        }
    } else {
        result = fma(op1, op2, op3);
    }

    ap_uint<64> bit_result = convert_to_uint(result);

    if (decoded.negate_result) {
        bit_result[63] = ~bit_result[63];
    }

    bool ov_exception = isfinite(op1) && isfinite(op2) && isfinite(op3) && isinf(result);
    // Underflow is impossible to check with this style of code, it will be ignored
    // Inexact will not be covered, because we ignore rounding

    if (ov_exception) {
        registers.FPSCR.OX = 1;
    }

    pipeline::float_result_t write_back;
    write_back.write_back = true;

    // Check conditions and set exceptions accordingly
    if (VE == 0 && OE == 0) {
        if (exception) {
            registers.FPSCR.FR = 0;
            registers.FPSCR.FI = 0;
            registers.FPSCR.FPRF = E_QNAN;

            registers.FPSCR.FX = 1;
        } else if (ov_exception) {
            registers.FPSCR.OX = 1;
            registers.FPSCR.XX = 1;
            registers.FPSCR.FI = 1;

            // Ignore rounding and only allow infinity here
            if (bit_result[63] == 1) {
                registers.FPSCR.FPRF = E_N_INF;
            } else {
                registers.FPSCR.FPRF = E_P_INF;
            }

            registers.FPSCR.FX = 1;
        } else {
            registers.FPSCR.FPRF = get_FPRF(result, bit_result[63]);
        }
    } else {
        if (VE == 1) {
            if (result == std::numeric_limits<double>::signaling_NaN()) {
                registers.FPSCR.VXSNAN = 1;
                write_back.write_back = false;
                exception = true;
            }

            if (exception) {
                registers.FPSCR.FR = 0;
                registers.FPSCR.FI = 0;

                registers.FPSCR.FX = 1;
            }
        }

        if (OE == 1) {
            if (ov_exception) {
                registers.FPSCR.OX = 1;
                // We only care for double precision, correct the exponent by 1536
                bit_result(62, 52) = bit_result(62, 52) - 1536;
                if (bit_result[63] == 1) {
                    registers.FPSCR.FPRF = E_N_NORM;
                } else {
                    registers.FPSCR.FPRF = E_P_NORM;
                }

                registers.FPSCR.FX = 1;
            }
        }
    }

    if (decoded.alter_CR1) {
        copy_condition(registers);
    }

    write_back.result = result;
    write_back.address = decoded.result_reg_address;

    return write_back;
}

pipeline::float_result_t
floating_point::convert(float_convert_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands) {
    double op1 = convert_to_double(operands.op1);
    ap_uint<64> result;

    // TODO: Check for SNaN and set exceptions accordingly
    if (decoded.round_to_single) {
        // Ignore rounding, we only use double precision
        result = convert_to_uint(op1);
    } else if (decoded.convert_to_integer) {
        // Only words are supported - ignore doublewords, ignore rounding modes
        if(op1 > 0x7FFFFFFF) {
            result = 0x7FFFFFFF;
        } else if(op1 < ((int32_t)0x80000000)) {
            result = 0x80000000;
        } else {
            result = (int32_t) op1;
        }
    }

    if (decoded.alter_CR1) {
        copy_condition(registers);
    }

    pipeline::float_result_t write_back = {result, decoded.target_reg_address, true};
}

pipeline::float_result_t
floating_point::compare(float_compare_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands) {
    double op1 = operands.op1;
    double op2 = operands.op2;

    auto CR = registers.condition_reg[decoded.BF];

    if(isnan(op1) || isnan(op2)) {
        CR.condition_floating_point_compare.FU = 1;
        CR.condition_floating_point_compare.FE = 0;
        CR.condition_floating_point_compare.FG = 0;
        CR.condition_floating_point_compare.FL = 0;

        registers.FPSCR.FPRF[0] = 1;
        registers.FPSCR.FPRF[1] = 0;
        registers.FPSCR.FPRF[2] = 0;
        registers.FPSCR.FPRF[3] = 0;
    } else if(op1 < op2) {
        CR.condition_floating_point_compare.FU = 0;
        CR.condition_floating_point_compare.FE = 0;
        CR.condition_floating_point_compare.FG = 0;
        CR.condition_floating_point_compare.FL = 1;

        registers.FPSCR.FPRF[0] = 0;
        registers.FPSCR.FPRF[1] = 0;
        registers.FPSCR.FPRF[2] = 0;
        registers.FPSCR.FPRF[3] = 1;
    } else if(op1 > op2) {
        CR.condition_floating_point_compare.FU = 0;
        CR.condition_floating_point_compare.FE = 0;
        CR.condition_floating_point_compare.FG = 1;
        CR.condition_floating_point_compare.FL = 0;

        registers.FPSCR.FPRF[0] = 0;
        registers.FPSCR.FPRF[1] = 0;
        registers.FPSCR.FPRF[2] = 1;
        registers.FPSCR.FPRF[3] = 0;
    } else {
        CR.condition_floating_point_compare.FU = 0;
        CR.condition_floating_point_compare.FE = 1;
        CR.condition_floating_point_compare.FG = 0;
        CR.condition_floating_point_compare.FL = 0;

        registers.FPSCR.FPRF[0] = 0;
        registers.FPSCR.FPRF[1] = 1;
        registers.FPSCR.FPRF[2] = 0;
        registers.FPSCR.FPRF[3] = 0;
    }

    if(op1 == std::numeric_limits<double>::signaling_NaN() || op2 == std::numeric_limits<double>::signaling_NaN()) {
        registers.FPSCR.VXSNAN = 1;

        if(!decoded.unordered) {
            if (registers.FPSCR.VE == 0) {
                registers.FPSCR.VXVC = 1;
            }
        }

        registers.FPSCR.FX = 1;
    } else if(op1 == std::numeric_limits<double>::quiet_NaN() || op2 == std::numeric_limits<double>::quiet_NaN()) {
        if(!decoded.unordered) {
            registers.FPSCR.VXVC = 1;

            registers.FPSCR.FX = 1;
        }
    }

    pipeline::float_result_t write_back = {0.0, 0, false};
}

pipeline::float_result_t
floating_point::status(float_status_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands) {
    ap_uint<64> op1 = operands.op1;
    ap_uint<64> result = 0;

    if(decoded.move_to_FPR) {
        result(31, 0) = registers.FPSCR.getFPSCR();
    } else if(decoded.move_to_CR) {
        auto CR = registers.condition_reg.getCR();
        // BF and BFA are in big endian notation, reverse with 7-x
        CR((7-decoded.BF_BT+1)*4-1, (7-decoded.BF_BT)*4) = registers.FPSCR.getFPSCR()((7-decoded.BFA+1)*4-1, (7-decoded.BFA)*4);
        registers.condition_reg = CR;
        switch(decoded.BFA) {
            case 0:
                registers.FPSCR.FX = 0;
                registers.FPSCR.OX = 0;
                break;
            case 1:
                registers.FPSCR.UX = 0;
                registers.FPSCR.ZX = 0;
                registers.FPSCR.XX = 0;
                registers.FPSCR.VXSNAN = 0;
                break;
            case 2:
                registers.FPSCR.VXISI = 0;
                registers.FPSCR.VXIDI = 0;
                registers.FPSCR.VXZDZ = 0;
                registers.FPSCR.VXIMZ = 0;
                break;
            case 3:
                registers.FPSCR.VXVC = 0;
                break;
            case 5:
                registers.FPSCR.VXSOFT = 0;
                registers.FPSCR.VXSQRT = 0;
                registers.FPSCR.VXCVI = 0;
                break;
        }
    } else if(decoded.move_to_FPSCR) {
        if(decoded.use_U) {
            auto FPSCR = registers.FPSCR.getFPSCR();
            if(decoded.BF_BT == 0) { // FEX and VX cannot be set explicitly
                FPSCR(30, 29) = decoded.U(2, 1); // TODO: Check if this is correct
            } else {
                FPSCR((7-decoded.BF_BT+1)*4-1, (7-decoded.BF_BT)*4) = decoded.U;
            }
            registers.FPSCR = FPSCR;
        } else if(decoded.bit_0) {
            // FEX and VX cannot be set explicitly
            if(decoded.BF_BT != 1 || decoded.BF_BT != 2) {
                auto FPSCR = registers.FPSCR.getFPSCR();
                // BT is in big endian notation - reverse with 31-x
                FPSCR[31 - decoded.BF_BT] = 0;
                registers.FPSCR = FPSCR;
            }
        } else if(decoded.bit_1) {
            // FEX and VX cannot be set explicitly
            if(decoded.BF_BT != 1 || decoded.BF_BT != 2) {
                auto FPSCR = registers.FPSCR.getFPSCR();
                // BT is in big endian notation - reverse with 31-x
                FPSCR[31 - decoded.BF_BT] = 1;
                registers.FPSCR = FPSCR;
            }
        } else { // use_FLM
            ap_uint<8> FLM = decoded.FLM;
            ap_uint<32> mask;
            for (uint32_t n = 0, b = 0; n < 8; n++) {
#pragma HLS unroll
                for (uint32_t i = 0; i < 4; i++) {
#pragma HLS unroll
                    mask[b++] = FLM[n];
                }
            }

            mask(30, 29) = 0b00;

            registers.FPSCR = op1(31, 0) & mask;
        }
    }

    if(decoded.alter_CR1) {
        copy_condition(registers);
    }
}

