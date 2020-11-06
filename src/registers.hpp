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
	struct condition_fixed_point {
		uint8_t SO:1; // Bit 3
		uint8_t EQ:1; // Bit 2
		uint8_t GT:1; // Bit 1
		uint8_t LT:1; // Bit 0
	} condition_fixed_point;

	// Special type for CR1
	struct condition_floating_point {
		uint8_t OX:1; 	// Bit 7
		uint8_t VX:1; 	// Bit 6
		uint8_t FEX:1;	// Bit 5
		uint8_t FX:1; 	// Bit 4
	} condition_floating_point;

	struct condition_floating_point_compare {
		uint8_t FU:1; // Bit 3
		uint8_t FE:1; // Bit 2
		uint8_t FG:1; // Bit 1
		uint8_t FL:1; // Bit 0
	} condition_floating_point_compare;
};

// CR is in big endian notation, hence the order is reversed with 7-i
struct condition_reg {
public:
    condition_reg& operator=(uint32_t value) {
        ap_uint<32> temp = value;
        for(int32_t i = 0; i < 8; i++) {
#pragma HLS unroll
            CR[7-i].condition_fixed_point.SO = temp[i*4+0];
            CR[7-i].condition_fixed_point.EQ = temp[i*4+1];
            CR[7-i].condition_fixed_point.GT = temp[i*4+2];
            CR[7-i].condition_fixed_point.LT = temp[i*4+3];
        }
        return *this;
    }

    ap_uint<32> getCR() {
        ap_uint<32> temp;
        for(int32_t i = 0; i < 8; i++) {
#pragma HLS unroll
            temp[i*4+0] = CR[7-i].condition_fixed_point.SO;
            temp[i*4+1] = CR[7-i].condition_fixed_point.EQ;
            temp[i*4+2] = CR[7-i].condition_fixed_point.GT;
            temp[i*4+3] = CR[7-i].condition_fixed_point.LT;
        }
        return temp;
    }

    condition_field& operator[](uint32_t i) {
        return CR[7-i];
    }

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
	ap_uint<32> count_register; // Count register
} registers_t;

#endif
