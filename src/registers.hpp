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


#ifndef __REGISTERS__
#define __REGISTERS__

#include <stdint.h>
#include <ap_int.h>

union condition_field {
    condition_field() : condition_fixed_point({0,0,0,0}) {}
    ~condition_field() {}

	struct condition_fixed_point {
		ap_uint<1> SO; // Bit 3
        ap_uint<1> EQ; // Bit 2
        ap_uint<1> GT; // Bit 1
        ap_uint<1> LT; // Bit 0
	} condition_fixed_point;

	// Special type for CR1
	struct condition_floating_point {
        ap_uint<1> OX; 	// Bit 7
        ap_uint<1> VX; 	// Bit 6
        ap_uint<1> FEX;	// Bit 5
        ap_uint<1> FX; 	// Bit 4
	} condition_floating_point;

	struct condition_floating_point_compare {
        ap_uint<1> FU; // Bit 3
        ap_uint<1> FE; // Bit 2
        ap_uint<1> FG; // Bit 1
        ap_uint<1> FL; // Bit 0
	} condition_floating_point_compare;
};

struct condition_reg {
public:
    condition_reg& operator=(uint32_t value) {
        ap_uint<32> temp = value;
        for(int32_t i = 0; i < 8; i++) {
#pragma HLS unroll
            CR[i].condition_fixed_point.SO = temp[i*4+0];
            CR[i].condition_fixed_point.EQ = temp[i*4+1];
            CR[i].condition_fixed_point.GT = temp[i*4+2];
            CR[i].condition_fixed_point.LT = temp[i*4+3];
        }
        return *this;
    }

    ap_uint<32> getCR() {
        ap_uint<32> temp;
        for(int32_t i = 0; i < 8; i++) {
#pragma HLS unroll
            temp[i*4+0] = CR[i].condition_fixed_point.SO;
            temp[i*4+1] = CR[i].condition_fixed_point.EQ;
            temp[i*4+2] = CR[i].condition_fixed_point.GT;
            temp[i*4+3] = CR[i].condition_fixed_point.LT;
        }
        return temp;
    }

    condition_field& operator[](uint32_t i) {
        return CR[i];
    }
private:
	condition_field CR[8];
};

struct fixed_point_exception_reg {
	struct {
		ap_uint<7> string_bytes; // For load/store string
		ap_uint<22> RESERVED_2; // Reserved
		ap_uint<1> CA; 			// Carry bit
		ap_uint<1> OV; 			// Overflow bit
		ap_uint<1> SO; 			// Summary overflow bit
		//ap_uint<32> RESERVED_1; 	// Reserved
	} exception_fields;
	fixed_point_exception_reg& operator=(uint32_t XER) {
	    ap_uint<32> val = XER;
        exception_fields.string_bytes = val(6, 0);
        exception_fields.RESERVED_2 = val(28, 7);
        exception_fields.CA = val[29];
        exception_fields.OV = val[30];
        exception_fields.SO = val[31];
	    return *this;
	}
	ap_uint<32> getXER() {
        ap_uint<32> val;
        val(6, 0) = exception_fields.string_bytes;
        val(28, 7) = exception_fields.RESERVED_2;
        val[29] = exception_fields.CA;
        val[30] = exception_fields.OV;
        val[31] = exception_fields.SO;
        return val;
	}
};

typedef struct {
	ap_uint<32> GPR[32]; // General purpose registers
	ap_uint<64> FPR[32]; // Floating point registers
	condition_reg condition_reg; // Condition register
	ap_uint<32> link_register; // Link register
	fixed_point_exception_reg fixed_exception_reg; // Fixed point exception register
} registers_t;

#endif
