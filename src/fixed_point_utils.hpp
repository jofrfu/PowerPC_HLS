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

#include <stdint.h>
#include <ap_int.h>
#include "ppc_types.h"

namespace fixed_point {

    void check_condition(int32_t result, registers_t &registers);

    void copy_summary_overflow(registers_t &registers);

    void set_overflow(ap_uint<1> overflow, registers_t &registers);

    template<int T>
    struct add_result {
        ap_uint<T> sum;
        ap_uint<1> carry;
        ap_uint<1> overflow;
    };

    template<int T>
    add_result<T> add(ap_uint<T> op1, ap_uint<T> op2, ap_uint<1> carry_in) {
        fixed_point::add_result<T> result;

#ifdef OWN_ADDER
        ap_uint<T+1> carry;

        carry[0] = carry_in;

        // helper signals
        ap_uint<T> sum = op1 ^ op2;
        ap_uint<T> carry_generate = op1 & op2;
        ap_uint<T> carry_propagate = op1 | op2;

        // adder
        for(int32_t i = 0; i < T; i++) {
#pragma HLS unroll
            carry[i + 1] = carry_generate[i] | (carry_propagate[i] & carry[i]);
        }

        result.sum = sum ^ carry(T-1, 0);
        result.carry = carry[T];
        result.overflow = carry[T] ^ carry[T-1];
#else
        ap_uint<T+1> sum = op1 + op2 + carry_in;
        if((op1[T-1] == 1 && op2[T-1] == 1 && sum[T-1] == 0) ||
         (op1[T-1] == 0 && op2[T-1] == 0 && sum[T-1] == 1)) {
          result.overflow = 1;
        } else {
          result.overflow = 0;
        }

        result.carry = sum[T];
        result.sum = sum(T-1, 0);
#endif
        return result;
    }

}
#endif
