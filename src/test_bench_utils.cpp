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
#include "fixed_point_processor.hpp"

int32_t read_byte_code(const char *file_name, uint32_t *instruction_memory, uint32_t memory_size) {
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

		// Little endian to big endian conversion
		for(uint32_t i = 0; i < size/4; i++) {
			ap_uint<32> little = instruction_memory[i];
			ap_uint<32> big;
			big(0, 7) = little(24, 31);
			big(8, 15) = little(16, 23);
			big(16, 23) = little(8, 15);
			big(24, 31) = little(0, 7);
			instruction_memory[i] = big;
		}

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
            big(0, 7) = temp[i*4+3];
            big(8, 15) = temp[i*4+2];
            big(16, 23) = temp[i*4+1];
            big(24, 31) = temp[i*4+0];
            data_memory[i] = big;
        }

        return size;
    } else {
        std::cout << "Failed to open file " << file_name << "!" << std::endl;
        return -1;
    }
}

bool execute_single_instruction(uint32_t instruction, registers_t &registers, ap_uint<32> *data_memory) {
	decode_result_t decoded = decode(instruction);
	fixed_point::load<ap_uint<32>*>(
			decoded.fixed_point_decode_result.execute_load,
			decoded.fixed_point_decode_result.load_store_decoded,
			registers, data_memory);
	fixed_point::store<ap_uint<32>*>(
			decoded.fixed_point_decode_result.execute_store,
			decoded.fixed_point_decode_result.load_store_decoded,
			registers, data_memory);
	fixed_point::load_string<ap_uint<32>*>(
            decoded.fixed_point_decode_result.execute_load_string,
            decoded.fixed_point_decode_result.load_store_decoded,
            registers, data_memory
	        );
    fixed_point::store_string<ap_uint<32>*>(
            decoded.fixed_point_decode_result.execute_store_string,
            decoded.fixed_point_decode_result.load_store_decoded,
            registers, data_memory);
	fixed_point::add_sub(
			decoded.fixed_point_decode_result.execute_add_sub,
			decoded.fixed_point_decode_result.add_sub_decoded,
			registers);
	fixed_point::multiply(
			decoded.fixed_point_decode_result.execute_mul,
			decoded.fixed_point_decode_result.mul_decoded,
			registers);
	fixed_point::divide(
			decoded.fixed_point_decode_result.execute_div,
			decoded.fixed_point_decode_result.div_decoded,
			registers);
	fixed_point::compare(
			decoded.fixed_point_decode_result.execute_compare,
			decoded.fixed_point_decode_result.cmp_decoded,
			registers);
	bool trap = fixed_point::trap(
			decoded.fixed_point_decode_result.execute_trap,
			decoded.fixed_point_decode_result.trap_decoded,
			registers);
	fixed_point::logical(
			decoded.fixed_point_decode_result.execute_logical,
			decoded.fixed_point_decode_result.log_decoded,
			registers);
	fixed_point::rotate(
			decoded.fixed_point_decode_result.execute_rotate,
			decoded.fixed_point_decode_result.rotate_decoded,
			registers);
	fixed_point::shift(
			decoded.fixed_point_decode_result.execute_shift,
			decoded.fixed_point_decode_result.shift_decoded,
			registers);
	fixed_point::system(
			decoded.fixed_point_decode_result.execute_system,
			decoded.fixed_point_decode_result.system_decoded,
			registers);

	return trap;
}

void execute_program(uint32_t *instruction_memory, uint32_t size, registers_t &registers, ap_uint<32> *data_memory, trap_handler_t trap_handler) {
	for(uint32_t i = 0; i < size; i++) {
		if(execute_single_instruction(instruction_memory[i], registers, data_memory)) {
		    trap_handler(i);
		}
	}
}
