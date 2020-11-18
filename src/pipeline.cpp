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
// Created by Jonas Fuhrmann on 05.11.20.
//

#include "pipeline.hpp"
#include "fixed_point_processor.hpp"
#include "branch_processor.hpp"

ap_uint<32> pipeline::instruction_fetch(ap_uint<32> *instruction_memory, registers_t &registers) {
#pragma HLS inline
    ap_uint<32> instruction = instruction_memory[registers.program_counter(31, 2)];
    ap_uint<32> big_endian;
    // Conversion for big endian access
    big_endian(31, 24) = instruction(7, 0);
    big_endian(23, 16) = instruction(15, 8);
    big_endian(15, 8) = instruction(23, 16);
    big_endian(7, 0) = instruction(31, 24);
    return big_endian;
}

// For testing only
ap_uint<32> pipeline::fetch_index(ap_uint<32> *instruction_memory, uint32_t index) {
    ap_uint<32> instruction = instruction_memory[index];
    ap_uint<32> big_endian;
    // Conversion for big endian access
    big_endian(31, 24) = instruction(7, 0);
    big_endian(23, 16) = instruction(15, 8);
    big_endian(15, 8) = instruction(23, 16);
    big_endian(7, 0) = instruction(31, 24);
    return big_endian;
}

pipeline::operands_t pipeline::fetch_operands(decode_result_t decoded, registers_t &registers) {
    operands_t operands;
    switch (decoded.fixed_point_decode_result.execute) {
        case fixed_point::NONE:
            break;
        case fixed_point::LOAD:
        case fixed_point::STORE: {
            uint32_t sum1, sum2;
            if (decoded.fixed_point_decode_result.load_store_decoded.sum1_imm) {
                sum1 = (int32_t) decoded.fixed_point_decode_result.load_store_decoded.sum1_immediate;
            } else {
                sum1 = registers.GPR[decoded.fixed_point_decode_result.load_store_decoded.sum1_reg_address];
            }

            if (decoded.fixed_point_decode_result.load_store_decoded.sum2_imm) {
                sum2 = (int32_t) decoded.fixed_point_decode_result.load_store_decoded.sum2_immediate;
            } else {
                sum2 = registers.GPR[decoded.fixed_point_decode_result.load_store_decoded.sum2_reg_address];
            }

            operands.op1 = sum1;
            operands.op2 = sum2;
            operands.op3 = 0;
        }
            break;
        case fixed_point::LOAD_STRING:
        case fixed_point::STORE_STRING: {
            uint32_t sum1, sum2;
            ap_uint<7> n;
            if (decoded.fixed_point_decode_result.load_store_decoded.sum1_imm) {
                sum1 = (int32_t) decoded.fixed_point_decode_result.load_store_decoded.sum1_immediate;
            } else {
                sum1 = registers.GPR[decoded.fixed_point_decode_result.load_store_decoded.sum1_reg_address];
            }

            if (decoded.fixed_point_decode_result.load_store_decoded.sum2_imm) {
                // Sum 2 is zero, which means this is a lswi instruction
                sum2 = (int32_t) decoded.fixed_point_decode_result.load_store_decoded.sum2_immediate;
                if (decoded.fixed_point_decode_result.load_store_decoded.sum2_reg_address == 0) {
                    n = 32;
                } else {
                    n = decoded.fixed_point_decode_result.load_store_decoded.sum2_reg_address;
                }
            } else {
                // lswx instruction
                sum2 = registers.GPR[decoded.fixed_point_decode_result.load_store_decoded.sum2_reg_address];
                n = registers.fixed_exception_reg.exception_fields.string_bytes;
            }

            operands.op1 = sum1;
            operands.op2 = sum2;
            operands.op3 = n;
        }
            break;
        case fixed_point::ADD_SUB:
            if (decoded.fixed_point_decode_result.add_sub_decoded.op1_imm) {
                operands.op1 = decoded.fixed_point_decode_result.add_sub_decoded.op1_immediate;
            } else {
                operands.op1 = registers.GPR[decoded.fixed_point_decode_result.add_sub_decoded.op1_reg_address];
            }

            if (decoded.fixed_point_decode_result.add_sub_decoded.op2_imm) {
                operands.op2 = decoded.fixed_point_decode_result.add_sub_decoded.op2_immediate;
            } else {
                operands.op2 = registers.GPR[decoded.fixed_point_decode_result.add_sub_decoded.op2_reg_address];
            }
            operands.op3 = 0;
            break;
        case fixed_point::MUL:
            operands.op1 = registers.GPR[decoded.fixed_point_decode_result.mul_decoded.op1_reg_address];
            if (decoded.fixed_point_decode_result.mul_decoded.op2_imm) {
                operands.op2 = decoded.fixed_point_decode_result.mul_decoded.op2_immediate;
            } else {
                operands.op2 = registers.GPR[decoded.fixed_point_decode_result.mul_decoded.op2_reg_address];
            }
            operands.op3 = 0;
            break;
        case fixed_point::DIV:
            operands.op1 = registers.GPR[decoded.fixed_point_decode_result.div_decoded.dividend_reg_address];
            operands.op2 = registers.GPR[decoded.fixed_point_decode_result.div_decoded.divisor_reg_address];
            operands.op3 = 0;
            break;
        case fixed_point::COMPARE:
            operands.op1 = registers.GPR[decoded.fixed_point_decode_result.cmp_decoded.op1_reg_address];
            if (decoded.fixed_point_decode_result.cmp_decoded.op2_imm) {
                operands.op2 = decoded.fixed_point_decode_result.cmp_decoded.op2_immediate;
            } else {
                operands.op2 = registers.GPR[decoded.fixed_point_decode_result.cmp_decoded.op2_reg_address];
            }
            operands.op3 = 0;
            break;
        case fixed_point::TRAP:
            operands.op1 = registers.GPR[decoded.fixed_point_decode_result.trap_decoded.op1_reg_address];
            if (decoded.fixed_point_decode_result.trap_decoded.op2_imm) {
                operands.op2 = decoded.fixed_point_decode_result.trap_decoded.op2_immediate;
            } else {
                operands.op2 = registers.GPR[decoded.fixed_point_decode_result.trap_decoded.op2_reg_address];
            }
            operands.op3 = 0;
            break;
        case fixed_point::LOGICAL:
            operands.op1 = registers.GPR[decoded.fixed_point_decode_result.log_decoded.op1_reg_address];
            if (decoded.fixed_point_decode_result.log_decoded.op2_imm) {
                operands.op2 = decoded.fixed_point_decode_result.log_decoded.op2_immediate;
            } else {
                operands.op2 = registers.GPR[decoded.fixed_point_decode_result.log_decoded.op2_reg_address];
            }
            operands.op3 = 0;
            break;
        case fixed_point::ROTATE:
            operands.op1 = registers.GPR[decoded.fixed_point_decode_result.rotate_decoded.source_reg_address];
            if (decoded.fixed_point_decode_result.rotate_decoded.shift_imm) {
                operands.op2 = decoded.fixed_point_decode_result.rotate_decoded.shift_immediate;
            } else {
                operands.op2 = registers.GPR[decoded.fixed_point_decode_result.rotate_decoded.shift_reg_address](5, 0);
            }
            operands.op3 = 0;
            break;
        case fixed_point::SYSTEM:
            operands.op1 = registers.GPR[decoded.fixed_point_decode_result.system_decoded.RS_RT];
            operands.op2 = 0;
            operands.op3 = 0;
            break;
    }

    return operands;
}

pipeline::result_t
pipeline::execute(decode_result_t decoded, registers_t &registers, operands_t operands, ap_uint<32> *data_memory) {
    result_t result = {0, 0, false, false};

    switch (decoded.fixed_point_decode_result.execute) {
        case fixed_point::LOAD:
            result = fixed_point::load<ap_uint<32> *>(decoded.fixed_point_decode_result.load_store_decoded,
                                             registers,
                                             operands,
                                             data_memory);
            break;
        case fixed_point::STORE:
            result = fixed_point::store<ap_uint<32> *>(decoded.fixed_point_decode_result.load_store_decoded,
                                              registers,
                                              operands,
                                              data_memory);
            break;
        case fixed_point::LOAD_STRING:
            fixed_point::load_string<ap_uint<32> *>(decoded.fixed_point_decode_result.load_store_decoded,
                                                    registers,
                                                    operands,
                                                    data_memory);
            break;
        case fixed_point::STORE_STRING:
            fixed_point::store_string<ap_uint<32> *>(decoded.fixed_point_decode_result.load_store_decoded,
                                                     registers,
                                                     operands,
                                                     data_memory);
            break;
        case fixed_point::ADD_SUB:
            result = fixed_point::add_sub(decoded.fixed_point_decode_result.add_sub_decoded, registers, operands);
            break;
        case fixed_point::MUL:
            result = fixed_point::multiply(decoded.fixed_point_decode_result.mul_decoded, registers, operands);
            break;
        case fixed_point::DIV:
            result = fixed_point::divide(decoded.fixed_point_decode_result.div_decoded, registers, operands);
            break;
        case fixed_point::COMPARE:
            result = fixed_point::compare(decoded.fixed_point_decode_result.cmp_decoded, registers, operands);
            break;
        case fixed_point::TRAP:
            result = fixed_point::trap(decoded.fixed_point_decode_result.trap_decoded, registers, operands);
            break;
        case fixed_point::LOGICAL:
            result = fixed_point::logical(decoded.fixed_point_decode_result.log_decoded, registers, operands);
            break;
        case fixed_point::ROTATE:
            result = fixed_point::rotate(decoded.fixed_point_decode_result.rotate_decoded, registers, operands);
            break;
        case fixed_point::SYSTEM:
            result = fixed_point::system(decoded.fixed_point_decode_result.system_decoded, registers, operands);
            break;
        case fixed_point::NONE:
            break;
    }
    switch (decoded.branch_decode_result.execute) {
        case branch::BRANCH:
            // Branch will be executed beforehand for performance reasons
            break;
        case branch::SYSTEM_CALL:
            branch::system_call(decoded.branch_decode_result.system_call_decoded, registers);
            break;
        case branch::CONDITION:
            branch::condition(decoded.branch_decode_result.condition_decoded, registers);
            break;
        case branch::NONE:
            break;
    }

    return result;
}

void pipeline::write_back(result_t result, registers_t &registers) {
    if(result.write_back) {
        registers.GPR[result.address] = result.result;
    }
}
