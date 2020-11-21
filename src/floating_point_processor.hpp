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
// Created by Jonas Fuhrmann on 18.11.20.
//

#ifndef POWERPC_HLS_FLOATING_POINT_PROCESSOR_HPP
#define POWERPC_HLS_FLOATING_POINT_PROCESSOR_HPP

#include "pipeline.hpp"
#include "ppc_types.h"

namespace floating_point {
    template<typename T>
    pipeline::float_result_t load(float_load_store_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands, T data_memory) {

    }

    template<typename T>
    pipeline::float_result_t store(float_load_store_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands, T data_memory) {

    }

    pipeline::float_result_t move(float_move_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands);

    pipeline::float_result_t arithmetic(float_arithmetic_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands);

    pipeline::float_result_t multiply_add(float_madd_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands);

    pipeline::float_result_t convert(float_convert_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands);

    pipeline::float_result_t compare(float_compare_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands);

    pipeline::float_result_t status(float_status_decode_t decoded, registers_t &registers, pipeline::float_operands_t operands);
}

#endif //POWERPC_HLS_FLOATING_POINT_PROCESSOR_HPP
