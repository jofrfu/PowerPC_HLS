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


#ifndef __FIXED_POINT_UTILS__
#define __FIXED_POINT_UTILS__

#include <cstdint>
#include <ap_int.h>
#include "ppc_types.h"

namespace fixed_point {

condition_field_t& select_CR(ap_uint<3> address, registers_t &registers);

void check_condition(int32_t result, registers_t &registers);

void copy_summary_overflow(registers_t &registers);

void set_overflow(ap_uint<1> overflow, registers_t &registers);

}
#endif
