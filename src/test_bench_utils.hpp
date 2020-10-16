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

int32_t read_byte_code(const char *file_name, uint32_t *instruction_memory, uint32_t memory_size);

void execute_single_instruction(uint32_t instruction, registers_t &registers, ap_uint<32> *data_memory);

void execute_program(uint32_t *instruction_memory, uint32_t size, registers_t &registers, ap_uint<32> *data_memory);

#endif
