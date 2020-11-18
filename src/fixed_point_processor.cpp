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

pipeline::result_t fixed_point::add_sub(add_sub_decode_t decoded, registers_t &registers, pipeline::operands_t operands) {
    ap_uint<32> op1 = operands.op1;
    ap_uint<32> op2 = operands.op2;

    ap_uint<1> carry_in;
    if (decoded.subtract) {
        // Two's complement
        op1 = ~op1;
        if (decoded.add_CA) {
            carry_in = registers.fixed_exception_reg.exception_fields.CA;
        } else {
            carry_in = 1;
        }
    } else if (decoded.add_CA) {
        carry_in = registers.fixed_exception_reg.exception_fields.CA;
    } else {
        carry_in = 0;
    }

    fixed_point::add_result<32> result = fixed_point::add<32>(op1, op2, carry_in);

    if (decoded.alter_CA) {
        registers.fixed_exception_reg.exception_fields.CA = result.carry;
    }

    if (decoded.alter_OV) {
        fixed_point::set_overflow(result.overflow, registers);
    }

    if (decoded.alter_CR0) {
        fixed_point::check_condition(result.sum, registers);
        // Copy the SO field from XER into CR0
        fixed_point::copy_summary_overflow(registers);
    }

    pipeline::result_t write_back = {result.sum, decoded.result_reg_address, true, false};

    return write_back;
}

pipeline::result_t fixed_point::multiply(mul_decode_t decoded, registers_t &registers, pipeline::operands_t operands) {
    ap_int<33> op1 = operands.op1;
    ap_int<33> op2 = operands.op2;

    if (decoded.mul_signed) {
        // sign extend
        op1[32] = op1[31];
        op2[32] = op2[31];
    } else {
        // zero extend
        op1[32] = 0;
        op2[32] = 0;
    }

    ap_int<66> op_result = op1 * op2;
    // Choose upper or lower part of result
    uint32_t result = decoded.mul_higher ? op_result(63, 32) : op_result(31, 0);
    ap_uint<1> overflow;

    // Overflow
    if (decoded.alter_OV) {
        // If one operand is signed
        if ((op1[32] == 1) ^ (op2[32] == 1)) {
            if (op_result(65, 31) != 0x7FFFFFFFF) {
                overflow = 1;
            } else {
                overflow = 0;
            }
        } else {
            // Unsigned
            // On signed multiplication, the MSB of the 32 bit
            // result has to be checked as well
            // (if it's one, the result would be signed, which is incorrect)
            if (op_result(65, 31) != 0) {
                overflow = 1;
            } else {
                overflow = 0;
            }
        }

        fixed_point::set_overflow(overflow, registers);
    }

    // Must respect lower and higher!
    if (decoded.alter_CR0) {
        fixed_point::check_condition(result, registers);
        // Copy the SO field from XER into CR0
        fixed_point::copy_summary_overflow(registers);
    }

    pipeline::result_t write_back = {result, decoded.result_reg_address, true, false};

    return write_back;
}

pipeline::result_t fixed_point::divide(div_decode_t decoded, registers_t &registers, pipeline::operands_t operands) {
    ap_int<33> signed_dividend;
    ap_int<33> signed_divisor;
    signed_dividend(31, 0) = operands.op1;
    signed_divisor(31, 0) = operands.op2;
    if (decoded.div_signed) {
        // sign extend
        signed_dividend[32] = signed_dividend[31];
        signed_divisor[32] = signed_divisor[31];
    } else {
        // zero extend
        signed_dividend[32] = 0;
        signed_divisor[32] = 0;
    }

    ap_int<32> quotient;
    // avoid division by zero, since this will raise an exception, when executed in software
    if (signed_divisor != 0) {
        quotient = signed_dividend / signed_divisor;
    } else {
        quotient = 0;
    }

    ap_uint<1> overflow;

    if (signed_divisor == 0 || (signed_divisor == -1 && signed_dividend(31, 0) == 0x80000000)) {
        // divide by zero and the most negative number divided by -1 (the result wouldn't fit in 32 bits) is undefined
        overflow = registers.fixed_exception_reg.exception_fields.OV = 1;
    } else {
        overflow = registers.fixed_exception_reg.exception_fields.OV = 0;
    }

    if (decoded.alter_OV) {
        fixed_point::set_overflow(overflow, registers);
    }

    if (decoded.alter_CR0) {
        fixed_point::check_condition(quotient, registers);
        // Copy the SO field from XER into CR0
        fixed_point::copy_summary_overflow(registers);
    }

    pipeline::result_t write_back = {quotient, decoded.result_reg_address, true, false};

    return write_back;
}

pipeline::result_t fixed_point::compare(cmp_decode_t decoded, registers_t &registers, pipeline::operands_t operands) {
    ap_int<33> op1, op2;

    op1(31, 0) = operands.op1;
    op2(31, 0) = operands.op2;

    if (decoded.cmp_signed) {
        op1[32] = op1[31];
        op2[32] = op2[31];
    } else {
        op1[32] = 0;
        op2[32] = 0;
    }

    condition_field &CR = registers.condition_reg[decoded.BF];

    if (op1 < op2) {
        CR.condition_fixed_point.LT = 1;
        CR.condition_fixed_point.GT = 0;
        CR.condition_fixed_point.EQ = 0;
    } else if (op1 > op2) {
        CR.condition_fixed_point.LT = 0;
        CR.condition_fixed_point.GT = 1;
        CR.condition_fixed_point.EQ = 0;
    } else { // op1 == op2
        CR.condition_fixed_point.LT = 0;
        CR.condition_fixed_point.GT = 0;
        CR.condition_fixed_point.EQ = 1;
    }

    // Copy SO into the given SPR
    CR.condition_fixed_point.SO = registers.fixed_exception_reg.exception_fields.SO;

    return {0, 0, false, false};
}

pipeline::result_t fixed_point::trap(trap_decode_t decoded, registers_t &registers, pipeline::operands_t operands) {
    int32_t op1 = operands.op1;
    int32_t op2 = operands.op2;

    uint32_t u_op1 = op1;
    uint32_t u_op2 = op2;

    ap_uint<5> TO = decoded.TO;

    pipeline::result_t write_back = {0, 0, false, false};
    write_back.trap = (
            (op1 < op2 && TO[4] == 1) ||
            (op1 > op2 && TO[3] == 1) ||
            (op1 == op2 && TO[2] == 1) ||
            (u_op1 < u_op2 && TO[1] == 1) ||
            (u_op1 > u_op2 && TO[0] == 1)
    );
    return write_back;
}

pipeline::result_t fixed_point::logical(log_decode_t decoded, registers_t &registers, pipeline::operands_t operands) {
    ap_uint<32> op1 = operands.op1;
    ap_uint<32> op2 = operands.op2;

    ap_uint<32> result;

    switch (decoded.operation) {
        case logical::AND:
            result = op1 & op2;
            break;
        case logical::OR:
            result = op1 | op2;
            break;
        case logical::XOR:
            result = op1 ^ op2;
            break;
        case logical::NAND:
            result = ~(op1 & op2);
            break;
        case logical::NOR:
            result = ~(op1 | op2);
            break;
        case logical::EQUIVALENT:
            result = ~(op1 ^ op2);
            break;
        case logical::AND_COMPLEMENT:
            result = op1 & ~op2;
            break;
        case logical::OR_COMPLEMENT:
            result = op1 | ~op2;
            break;
        case logical::EXTEND_SIGN_BYTE:
            result(7, 0) = op1(7, 0);
            if (result[7] == 1) {
                // sign extend
                result(31, 8) = 0xFFFFFF;
            } else {
                // zero extend
                result(31, 8) = 0;
            }
            break;
        case logical::EXTEND_SIGN_HALFWORD:
            result(15, 0) = op1(15, 0);
            if (result[15] == 1) {
                // sign extend
                result(31, 16) = 0xFFFF;
            } else {
                // zero extend
                result(31, 16) = 0;
            }
            break;
        case logical::COUNT_LEDING_ZEROS_WORD: {
            ap_uint<6> count = 0;
            for (int32_t i = 31; i >= 0; i--) {
#pragma HLS unroll
                if (op1[i] == 0) {
                    count++;
                } else {
                    break;
                }
            }
            result = count;
        }
            break;
            // popcntb doesn't seem to exist on 32 bit implementations, but it's not mentioned anywhere
            /*
            case logical::POPULATION_COUNT_BYTES:
                for(int32_t b = 0; b < 4; b++) {
#pragma HLS unroll
                    ap_uint<4> count = 0;
                    ap_uint<8> byte = op1((b+1)*8-1, b*8);
                    for(uint32_t i = 0; i < 8; i++) {
#pragma HLS unroll
                        if(byte[i] == 1) {
                            count++;
                        }
                    }
                    result((b+1)*8-1, b*8) = count;
                }
             */
    }

    if (decoded.alter_CR0) {
        fixed_point::check_condition(result, registers);
        // Copy the SO field from XER into CR0
        fixed_point::copy_summary_overflow(registers);
    }

    pipeline::result_t write_back = {result, decoded.result_reg_address, true, false};

    return write_back;
}

pipeline::result_t fixed_point::rotate(rotate_decode_t decoded, registers_t &registers, pipeline::operands_t operands) {
    ap_uint<32> source = operands.op1;
    ap_uint<6> shift = operands.op2(5, 0);

    ap_uint<5> mask_begin = 0;
    ap_uint<5> mask_end = 0;

    bool compute_mask;

    if(decoded.shift) {
        if(decoded.left) {
            if(shift[5] == 0) {
                mask_begin = 0;
                mask_end = 31 - shift(4, 0);
                compute_mask = true;
            } else {
                compute_mask = false;
            }
        } else {
            mask_begin = shift(4, 0);
            mask_end = 31;
            if(decoded.sign_extend) {
                if(decoded.shift_imm) {
                    compute_mask = true;
                } else {
                    if(shift[5] == 0) {
                        compute_mask = true;
                    } else {
                        compute_mask = false;
                    }
                }
            } else {
                if(shift[5] == 0) {
                    compute_mask = true;
                } else {
                    compute_mask = false;
                }
            }
            // Make the rotate left go around completely for right shifts.
            shift = 32-shift;
        }
    } else {
        mask_begin = decoded.MB;
        mask_end = decoded.ME;
        compute_mask = true;
    }

    ap_uint<32> mask = 0;

    if(compute_mask) {
        // Generate the mask
        // MB and ME are in big endian notation, hence the position is reversed with 31-i
        for (uint32_t i = 0; i < 32; i++) {
#pragma HLS unroll
            if (mask_begin > mask_end) {
                if (i <= mask_end) {
                    mask[31 - i] = 1;
                } else if (i >= mask_begin) {
                    mask[31 - i] = 1;
                } else {
                    mask[31 - i] = 0;
                }
            } else {
                if (i >= mask_begin && i <= mask_end) {
                    mask[31 - i] = 1;
                } else {
                    mask[31 - i] = 0;
                }
            }
        }
    }

    ap_uint<32> shifted;
    // Rotate left
    for (uint32_t i = 0; i < 32; i++) {
#pragma HLS unroll
        ap_uint<5> target_index = i + shift(4, 0);
        shifted[target_index] = source[i];
    }

    ap_uint<32> result;
    if (decoded.mask_insert) {
        result = (shifted & mask) | (registers.GPR[decoded.target_reg_address] & ~mask);
    } else if(decoded.shift && !decoded.left && decoded.sign_extend) {
        ap_uint<32> sign;
        if(source[31] == 1) {
            sign = 0xFFFFFFFF;
        } else {
            sign = 0;
        }
        result = (shifted & mask) | (sign & ~mask);
        ap_uint<1> carry = sign & ((shifted & ~mask) != 0);
        registers.fixed_exception_reg.exception_fields.CA = carry;
    } else {
        result = shifted & mask;
    }

    if (decoded.alter_CR0) {
        fixed_point::check_condition(result, registers);
        // Copy the SO field from XER into CR0
        fixed_point::copy_summary_overflow(registers);
    }

    pipeline::result_t write_back = {result, decoded.target_reg_address, true, false};

    return write_back;
}

pipeline::result_t fixed_point::system(system_decode_t decoded, registers_t &registers, pipeline::operands_t operands) {
    ap_uint<32> op1 = operands.op1;

    // The order of the two 5 bit halves is reversed
    ap_uint<10> SPR;
    SPR(4, 0) = decoded.SPR(9, 5);
    SPR(9, 5) = decoded.SPR(4, 0);

    ap_uint<8> FXM = decoded.FXM;
    ap_uint<32> mask;
    for (uint32_t n = 0, b = 0; n < 8; n++) {
#pragma HLS unroll
        for (uint32_t i = 0; i < 4; i++) {
#pragma HLS unroll
            mask[b++] = FXM[n];
        }
    }

    pipeline::result_t write_back = {0, 0, false, false};

    switch (decoded.operation) {
        case system_ppc::MOVE_TO_SPR:
            switch (SPR) {
                case 1:
                    registers.fixed_exception_reg = op1;
                    break;
                case 8:
                    registers.link_register = op1;
                    break;
                case 9:
                    registers.count_register = op1;
                    break;
            }
            break;
        case system_ppc::MOVE_FROM_SPR:
            switch (SPR) {
                case 1:
                    write_back.result = registers.fixed_exception_reg.getXER();
                    write_back.address = decoded.RS_RT;
                    write_back.write_back = true;
                    break;
                case 8:
                    write_back.result = registers.link_register;
                    write_back.address = decoded.RS_RT;
                    write_back.write_back = true;
                    break;
                case 9:
                    write_back.result = registers.count_register;
                    write_back.address = decoded.RS_RT;
                    write_back.write_back = true;
                    break;
            }
            break;
        case system_ppc::MOVE_TO_CR:
            registers.condition_reg = ((registers.GPR[decoded.RS_RT] & mask) |
                                       (registers.condition_reg.getCR() & ~mask));
            break;
        case system_ppc::MOVE_FROM_CR:
            write_back.result = registers.condition_reg.getCR();
            write_back.address = decoded.RS_RT;
            write_back.write_back = true;
            break;
    }

    return write_back;
}
