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

#ifndef POWERPC_HLS_PIPELINE_HPP
#define POWERPC_HLS_PIPELINE_HPP

#include <ap_int.h>
#include "instruction_decode.hpp"

namespace pipeline {
    typedef struct {
        ap_uint<32> op1;
        ap_uint<32> op2;
        // For load/store string only!
        ap_uint<7> n_count;
    } operands_t;

    ap_uint<32> instruction_fetch(ap_uint<32> *instruction_memory, registers_t &registers);
    // For testing only
    ap_uint<32> fetch_index(ap_uint<32> *instruction_memory, uint32_t index);

    operands_t fetch_operands(decode_result_t decoded, registers_t &registers);
    bool execute(decode_result_t decoded, registers_t &registers, ap_uint<32> *data_memory);
}
#endif //POWERPC_HLS_PIPELINE_HPP
