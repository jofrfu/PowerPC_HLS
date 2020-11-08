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
// Created by Jonas Fuhrmann on 07.11.20.
//

#include "branch_processor.hpp"

void branch::branch(branch_decode_t decoded, registers_t &registers) {
    ap_uint<32> CIA = registers.program_counter;
    ap_uint<32> NIA;
    ap_uint<32> NIA_ext;
    switch(decoded.operation) {
        case ::BRANCH:
            NIA_ext(1, 0) = 0;
            NIA_ext(25, 2) = decoded.LI;
            if (decoded.LI[23] == 1) {
                // Sign extend
                NIA_ext(31, 26) = 0x3F;
            } else {
                NIA_ext(31, 26) = 0;
            }

            if (decoded.AA) {
                NIA = NIA_ext;
            } else {
                NIA = CIA + NIA_ext;
            }

            // Take the branch, every iteration the PC will be incremented, subtract 4 to correct the NIA
            registers.program_counter = NIA-4;
            break;
        case BRANCH_CONDITIONAL: {
            // BO is in big endian notation, reverse the access (4-x)
            if (decoded.BO[4-2] == 0) {
                registers.count_register--;
            }
            bool ctr_ok = decoded.BO[4-2] | ((registers.count_register != 0) ^ decoded.BO[4-3]);
            bool cond_ok = decoded.BO[4-0] | (registers.condition_reg.getCR()[decoded.BI] ^ ~decoded.BO[4-1]);
            if(ctr_ok && cond_ok) {
                NIA_ext(1, 0) = 0;
                NIA_ext(15, 2) = decoded.BD;
                if(decoded.BD[13] == 1) {
                    NIA_ext(31, 16) = 0xFFFF;
                } else {
                    NIA_ext(31, 16) = 0;
                }

                if (decoded.AA) {
                    NIA = NIA_ext;
                } else {
                    NIA = CIA + NIA_ext;
                }

                // Take the branch, every iteration the PC will be incremented, subtract 4 to correct the NIA
                registers.program_counter = NIA-4;
            }
        }
            break;
        case BRANCH_CONDITIONAL_LINK: {
            // BO is in big endian notation, reverse the access (4-x)
            if (decoded.BO[4 - 2] == 0) {
                registers.count_register--;
            }
            bool ctr_ok = decoded.BO[4 - 2] | ((registers.count_register != 0) ^ decoded.BO[4 - 3]);
            bool cond_ok = decoded.BO[4 - 0] | (registers.condition_reg.getCR()[decoded.BI] ^ ~decoded.BO[4 - 1]);
            if (ctr_ok && cond_ok) {
                NIA(1, 0) = 0;
                NIA(31, 2) = registers.link_register(31, 2);

                // Take the branch, every iteration the PC will be incremented, subtract 4 to correct the NIA
                registers.program_counter = NIA-4;
            }
        }
            break;
        case BRANCH_CONDITIONAL_COUNT: {
            // BO is in big endian notation, reverse the access (4-x)
            bool cond_ok = decoded.BO[4-0] | (registers.condition_reg.getCR()[decoded.BI] ^ ~decoded.BO[4 - 1]);
            if(cond_ok) {
                NIA(1, 0) = 0;
                NIA(31, 2) = registers.link_register(31, 2);

                // Take the branch, every iteration the PC will be incremented, subtract 4 to correct the NIA
                registers.program_counter = NIA-4;
            }
        }
            break;
    }

    if (decoded.LK) {
        // Store return address to the link register
        registers.link_register = CIA+4;
    }
}

void branch::condition(condition_decode_t decoded, registers_t &registers) {
    ap_uint<32> CR = registers.condition_reg.getCR();
    switch(decoded.operation) {
        case condition::AND:
            CR[decoded.CR_result_reg_address] = CR[decoded.CR_op1_reg_address] & CR[decoded.CR_op2_reg_address];
            break;
        case condition::OR:
            CR[decoded.CR_result_reg_address] = CR[decoded.CR_op1_reg_address] | CR[decoded.CR_op2_reg_address];
            break;
        case condition::XOR:
            CR[decoded.CR_result_reg_address] = CR[decoded.CR_op1_reg_address] ^ CR[decoded.CR_op2_reg_address];
            break;
        case condition::NAND:
            CR[decoded.CR_result_reg_address] = !(CR[decoded.CR_op1_reg_address] & CR[decoded.CR_op2_reg_address]);
            break;
        case condition::NOR:
            CR[decoded.CR_result_reg_address] = !(CR[decoded.CR_op1_reg_address] | CR[decoded.CR_op2_reg_address]);
            break;
        case condition::EQUIVALENT:
            CR[decoded.CR_result_reg_address] = CR[decoded.CR_op1_reg_address] ^ ~CR[decoded.CR_op2_reg_address];
            break;
        case condition::AND_COMPLEMENT:
            CR[decoded.CR_result_reg_address] = CR[decoded.CR_op1_reg_address] & ~CR[decoded.CR_op2_reg_address];
            break;
        case condition::OR_COMPLEMENT:
            CR[decoded.CR_result_reg_address] = CR[decoded.CR_op1_reg_address] | ~CR[decoded.CR_op2_reg_address];
            break;
        case condition::MOVE: {
            // Big endian notation, reverse (7-x)
            ap_uint<3> BF = 7-decoded.CR_result_reg_address(4, 1);
            ap_uint<3> BFA = 7-decoded.CR_op1_reg_address(4, 1);
            CR(4*(BF+1)-1, 4*BF) = CR(4*(BFA+1)-1, 4*BFA);
        }
            break;
    }
    registers.condition_reg = CR;
}

void branch::system_call(system_call_decode_t decoded, registers_t &registers) {
    // TODO: implement with book III
}