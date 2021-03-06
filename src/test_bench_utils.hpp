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


#ifndef __test_bench_utils__
#define __test_bench_utils__

#include "registers.hpp"
#include <ap_int.h>
#include <functional>

int32_t read_byte_code(const char *file_name, ap_uint<32> *instruction_memory, uint32_t memory_size);

int32_t read_data(const char *file_name, ap_uint<32> *data_memory, uint32_t memory_size);

bool execute_single_instruction(ap_uint<32> instruction, registers_t &registers, ap_uint<32> *data_memory);

typedef std::function<void(uint32_t)> trap_handler_t;
void execute_program(ap_uint<32> *instruction_memory, uint32_t size, registers_t &registers, ap_uint<32> *data_memory, trap_handler_t trap_handler);

#endif
