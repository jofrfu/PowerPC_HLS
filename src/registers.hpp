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

#include <cstdint>
#include <ap_int.h>

typedef union {
	struct __attribute__ ((__packed__)) {
		uint8_t SO:1; // Bit 3
		uint8_t EQ:1; // Bit 2
		uint8_t GT:1; // Bit 1
		uint8_t LT:1; // Bit 0
	} condition_fixed_point;

	// Special type for CR1
	struct __attribute__ ((__packed__)) {
		uint8_t OX:1; 	// Bit 7
		uint8_t VX:1; 	// Bit 6
		uint8_t FEX:1;	// Bit 5
		uint8_t FX:1; 	// Bit 4
	} condition_floating_point;

	struct __attribute__ ((__packed__)) {
		uint8_t FU:1; // Bit 3
		uint8_t FE:1; // Bit 2
		uint8_t FG:1; // Bit 1
		uint8_t FL:1; // Bit 0
	} condition_floating_point_compare;
} condition_field_t;

typedef struct condition_reg {
public:
    void setCR(uint32_t value) {
        ap_uint<32> temp = value;
        for(int32_t i = 0; i < 8; i++) {
#pragma HLS unroll
            CR[i].condition_fixed_point.SO = temp[i*4+0];
            CR[i].condition_fixed_point.EQ = temp[i*4+1];
            CR[i].condition_fixed_point.GT = temp[i*4+2];
            CR[i].condition_fixed_point.LT = temp[i*4+3];
        }
    }

    uint32_t getCR() {
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

    condition_field_t& operator[](uint32_t i) {
        return CR[i];
    }
private:
	condition_field_t CR[8];
} condition_reg_t;

typedef union {
	uint32_t exception_bits;
	struct __attribute__ ((__packed__)) {
		uint8_t string_bytes:7; // For load/store string
		uint32_t RESERVED_2:22; // Reserved
		uint8_t CA:1; 			// Carry bit
		uint8_t OV:1; 			// Overflow bit
		uint8_t SO:1; 			// Summary overflow bit
		//uint32_t RESERVED_1; 	// Reserved
	} exception_fields;
} fixed_point_exception_reg_t;

typedef struct {
	ap_uint<32> GPR[32]; // General purpose registers
	uint64_t FPR[32]; // Floating point registers
	condition_reg_t condition_reg; // Condition register
	uint32_t link_register; // Link register
	fixed_point_exception_reg_t fixed_exception_reg; // Fixed point exception register
} registers_t;

#endif
