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

bool pipeline::execute(decode_result_t decoded, registers_t &registers, ap_uint<32> *data_memory) {
	bool trap_happened = false;
	if(decoded.fixed_point_decode_result.execute != fixed_point::NONE) {
        switch (decoded.fixed_point_decode_result.execute) {
            case fixed_point::LOAD:
                fixed_point::load<ap_uint<32> *>(decoded.fixed_point_decode_result.load_store_decoded, registers,
                                                 data_memory);
                break;
            case fixed_point::STORE:
                fixed_point::store<ap_uint<32> *>(decoded.fixed_point_decode_result.load_store_decoded, registers,
                                                  data_memory);
                break;
            case fixed_point::LOAD_STRING:
                fixed_point::load_string<ap_uint<32> *>(decoded.fixed_point_decode_result.load_store_decoded, registers,
                                                        data_memory);
                break;
            case fixed_point::STORE_STRING:
                fixed_point::store_string<ap_uint<32> *>(decoded.fixed_point_decode_result.load_store_decoded,registers,
                                                         data_memory);
                break;
            case fixed_point::ADD_SUB:
                fixed_point::add_sub(decoded.fixed_point_decode_result.add_sub_decoded, registers);
                break;
            case fixed_point::MUL:
                fixed_point::multiply(decoded.fixed_point_decode_result.mul_decoded, registers);
                break;
            case fixed_point::DIV:
                fixed_point::divide(decoded.fixed_point_decode_result.div_decoded, registers);
                break;
            case fixed_point::COMPARE:
                fixed_point::compare(decoded.fixed_point_decode_result.cmp_decoded, registers);
                break;
            case fixed_point::TRAP:
                trap_happened = fixed_point::trap(decoded.fixed_point_decode_result.trap_decoded, registers);
                break;
            case fixed_point::LOGICAL:
                fixed_point::logical(decoded.fixed_point_decode_result.log_decoded, registers);
                break;
            case fixed_point::ROTATE:
                fixed_point::rotate(decoded.fixed_point_decode_result.rotate_decoded, registers);
                break;
            case fixed_point::SYSTEM:
                fixed_point::system(decoded.fixed_point_decode_result.system_decoded, registers);
                break;
            case fixed_point::NONE:
                break;
        }
    } else if(decoded.branch_decode_result.execute != branch::NONE) {
	    switch(decoded.branch_decode_result.execute) {
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
	}

    return trap_happened;
}
