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

#include "test_bench_utils.hpp"

#include <iostream>
#include <fstream>

#include "instruction_decode.hpp"
#include "pipeline.hpp"
#include "branch_processor.hpp"

int32_t read_byte_code(const char *file_name, ap_uint<32> *instruction_memory, uint32_t memory_size) {
	std::ifstream byte_code(file_name, std::ios::binary);
	if(byte_code.is_open()) {
		std::ios::pos_type begin = byte_code.tellg();
		byte_code.seekg(0, std::ios::end);
		std::ios::pos_type end = byte_code.tellg();
		byte_code.seekg(0, std::ios::beg);
		std::ios::pos_type size = end - begin;
		if(size % 4 != 0) {
			std::cout << "File size is not divisible by 4, therefore these aren't proper instructions!" << std::endl;
			byte_code.close();
			return -1;
		}

		if(size/4 > memory_size) {
			std::cout << "The instruction memory is not big enough!" << std::endl;
			byte_code.close();
			return -1;
		}

		byte_code.seekg(0, std::ios::beg);
		byte_code.read((char *)instruction_memory, size);
		byte_code.close();

		return size/4;
	} else {
		std::cout << "Failed to open file " << file_name << "!" << std::endl;
		return -1;
	}
}

int32_t read_data(const char *file_name, ap_uint<32> *data_memory, uint32_t memory_size) {
    std::ifstream byte_code(file_name, std::ios::binary);
    if(byte_code.is_open()) {
        std::ios::pos_type begin = byte_code.tellg();
        byte_code.seekg(0, std::ios::end);
        std::ios::pos_type end = byte_code.tellg();
        byte_code.seekg(0, std::ios::beg);
        std::ios::pos_type size = end - begin;

        if(size > memory_size*4) {
            std::cout << "The data memory is not big enough!" << std::endl;
            byte_code.close();
            return -1;
        }

        // Pad to word boundary
        uint32_t temp_size = size + size % 4;
        char temp[temp_size];

        byte_code.seekg(0, std::ios::beg);
        byte_code.read(temp, size);
        byte_code.close();

        for(uint32_t i = 0; i < temp_size/4; i++) {
            ap_uint<32> big;
            big(31, 24) = temp[i*4+3];
            big(23, 16) = temp[i*4+2];
            big(15, 8) = temp[i*4+1];
            big(7, 0) = temp[i*4+0];
            data_memory[i] = big;
        }

        return size;
    } else {
        std::cout << "Failed to open file " << file_name << "!" << std::endl;
        return -1;
    }
}

bool execute_single_instruction(ap_uint<32> instruction, registers_t &registers, ap_uint<32> *data_memory) {
    decode_result_t decoded = pipeline::decode(instruction);
    pipeline::result_t result = {0, 0, false, false};
    if(decoded.branch_decode_result.execute == branch::BRANCH) {
        // Extracting branch from the "pipeline" reduces the minimal execution time.
        branch::branch(decoded.branch_decode_result.branch_decoded, registers);
    } else {
        auto operands = pipeline::fetch_operands(decoded, registers);
        result = pipeline::execute(decoded, registers, operands, data_memory);
        pipeline::write_back(result, registers);
    }
    registers.program_counter += 4;
    return result.trap;
}

void execute_program(ap_uint<32> *instruction_memory, uint32_t size, registers_t &registers, ap_uint<32> *data_memory, trap_handler_t trap_handler) {
    for(uint32_t i = 0; i < size; i++) {
		if(execute_single_instruction(pipeline::fetch_index(instruction_memory, i), registers, data_memory)) {
		    trap_handler(i);
		}
	}
}
