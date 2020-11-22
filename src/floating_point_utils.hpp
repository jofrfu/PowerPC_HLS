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
// Created by Jonas Fuhrmann on 21.11.20.
//

#ifndef POWERPC_HLS_FLOATING_POINT_UTILS_HPP
#define POWERPC_HLS_FLOATING_POINT_UTILS_HPP

#include <ap_int.h>
#include "registers.hpp"
#include <stdint.h>

namespace floating_point {
    void copy_condition(registers_t &registers);

    typedef union {
        uint64_t x;
        double y;
    } convert_t;

    double convert_to_double(ap_uint<64> val);

    ap_uint<64> convert_to_uint(double val);

    void check_exceptions(double result, registers_t &registers);
}

#endif //POWERPC_HLS_FLOATING_POINT_UTILS_HPP