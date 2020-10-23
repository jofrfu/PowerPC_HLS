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


#ifndef __FIXED_POINT_PROCESSOR__
#define __FIXED_POINT_PROCESSOR__

#include <cstdint>
#include <ap_int.h>
#include "ppc_types.h"

namespace fixed_point {
void load(bool execute, load_store_decode_t decoded, registers_t &registers, ap_uint<32> *data_memory);

void store(bool execute, load_store_decode_t decoded, registers_t &registers, ap_uint<32> *data_memory);

void add_sub(bool execute, add_sub_decode_t decoded, registers_t &registers);

void multiply(bool execute, mul_decode_t decoded, registers_t &registers);

void divide(bool execute, div_decode_t decoded, registers_t &registers);

void compare(bool execute, cmp_decode_t decoded, registers_t &registers);

bool trap(bool execute, trap_decode_t decoded, registers_t &registers);

void logical(bool execute, log_decode_t decoded, registers_t &registers);

void rotate(bool execute, rotate_decode_t decoded, registers_t &registers);

void shift(bool execute, shift_decode_t decoded, registers_t &registers);

void system(bool execute, system_decode_t decoded, registers_t &registers);
}
#endif
