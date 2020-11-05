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

bool pipeline::execute(decode_result_t decoded, registers_t &registers, ap_uint<32> *data_memory) {
    bool trap_happened = false;
    switch (decoded.fixed_point_decode_result.execute) {
        case fixed_point::LOAD:
            fixed_point::load<ap_uint<32>*>(decoded.fixed_point_decode_result.load_store_decoded, registers, data_memory);
            break;
        case fixed_point::STORE:
            fixed_point::store<ap_uint<32>*>(decoded.fixed_point_decode_result.load_store_decoded, registers, data_memory);
            break;
        case fixed_point::LOAD_STRING:
            fixed_point::load_string<ap_uint<32>*>(decoded.fixed_point_decode_result.load_store_decoded, registers, data_memory);
            break;
        case fixed_point::STORE_STRING:
            fixed_point::store_string<ap_uint<32>*>(decoded.fixed_point_decode_result.load_store_decoded, registers, data_memory);
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
        case fixed_point::SHIFT:
            fixed_point::shift(decoded.fixed_point_decode_result.shift_decoded, registers);
            break;
        case fixed_point::SYSTEM:
            fixed_point::system(decoded.fixed_point_decode_result.system_decoded, registers);
            break;
    }
    return trap_happened;
}