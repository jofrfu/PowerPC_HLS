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

/*
 * Author: Jonas Fuhrmann
 * Date: 23.09.2020
 */

#ifndef __PPC_TYPES__
#define __PPC_TYPES__

#include <stdint.h>

typedef union {
	uint32_t instruction_bits;

	struct {
		uint8_t LK:1;
		uint8_t AA:1;
		uint32_t LI:24;
		uint8_t OPCD:6;
	} I_Form;

	struct {
		uint8_t LK:1;
		uint8_t AA:1;
		uint16_t BD:14;
		uint8_t BI:5;
		uint8_t BO:5;
		uint8_t OPCD:6;
	} B_Form;

	struct {
		uint8_t UNUSED_5:1;
		uint8_t ALWAYS_ONE:1;
		uint8_t UNUSED_4:3;
		uint8_t LEV:7;
		uint8_t UNUSED_3:4;
		uint8_t UNUSED_2:5;
		uint8_t UNUSED_1:5;
		uint8_t OPCD:6;
	} SC_Form;

	struct {
		uint16_t D:16;
		uint8_t RA:5;
		uint8_t RT:5;
		uint8_t OPCD:6;
	} D_Form;

	struct {
		uint8_t XO:2;
		uint16_t DS:14;
		uint8_t RA:5;
		uint8_t RT:5;
		uint8_t OPCD:6;
	} DS_Form;

	struct {
		uint8_t Rc:1;
		uint16_t XO:10;
		uint8_t RB:5;
		uint8_t RA:5;
		uint8_t RT:5;
		uint8_t OPCD:6;
	} X_Form;

	struct {
		uint8_t LK:1;
		uint16_t XO:10;
		uint8_t BB:5;
		uint8_t BA:5;
		uint8_t BT:5;
		uint8_t OPCD:6;
	} XL_Form;

	struct {
		uint8_t UNUSED_1:1;
		uint16_t XO:10;
		uint16_t spr:10;
		uint8_t RT:5;
		uint8_t OPCD:6;
	} XFX_Form;

	struct {
		uint8_t Rc:1;
		uint16_t XO:10;
		uint8_t FRB:5;
		uint8_t UNUSED_2:1;
		uint8_t FLM:8;
		uint8_t UNUSED_1:1;
		uint8_t OPCD:6;
	} XFL_Form;

	struct {
		uint8_t Rc:1;
		uint8_t SH_2:1;
		uint16_t XO:9;
		uint8_t SH_1:5;
		uint8_t RA:5;
		uint8_t RS:5;
		uint8_t OPCD:6;
	} XS_Form;

	struct {
		uint8_t Rc:1;
		uint16_t XO:9;
		uint8_t OE:1;
		uint8_t RB:5;
		uint8_t RA:5;
		uint8_t RT:5;
		uint8_t OPCD:6;
	} XO_Form;

	struct {
		uint8_t Rc:1;
		uint8_t XO:5;
		uint8_t FRC:5;
		uint8_t FRB:5;
		uint8_t FRA:5;
		uint8_t FRT:5;
		uint8_t OPCD:6;
	} A_Form;

	struct {
		uint8_t Rc:1;
		uint8_t ME:5;
		uint8_t MB:5;
		uint8_t RB:5;
		uint8_t RA:5;
		uint8_t RS:5;
		uint8_t OPCD:6;
	} M_Form;

	struct {
		uint8_t Rc:1;
		uint8_t SH_2:1;
		uint8_t XO:3;
		uint8_t MB:6;
		uint8_t SH_1:5;
		uint8_t RA:5;
		uint8_t RS:5;
		uint8_t OPCD:6;
	} MD_Form;

	struct {
		uint8_t Rc:1;
		uint8_t XO:4;
		uint8_t MB:6;
		uint8_t RB:5;
		uint8_t RA:5;
		uint8_t RS:5;
		uint8_t OPCD:6;
	} MDS_Form;
} instruction_t;

// Types for branch processor
typedef enum {BRANCH, BRANCH_CONDITIONAL, BRANCH_CONDITIONAL_LINK, BRANCH_CONDITIONAL_COUNT} branch_op_t;

typedef struct {
	branch_op_t operation;
	uint8_t LK:1;
	uint8_t AA:1;
	uint32_t LI:24;
	uint16_t BD:14;
	uint8_t BI:5;
	uint8_t BO:5;
	uint8_t BH:2;
} branch_decode_t;

typedef uint8_t system_call_decode_t;

namespace condition {
	typedef enum {AND, OR, XOR, NAND, NOR, EQUIVALENT, AND_COMPLEMENT, OR_COMPLEMENT, MOVE} condition_op_t;
}

typedef struct {
	condition::condition_op_t operation;
	uint8_t CR_op1_reg_address:5;
	uint8_t CR_op2_reg_address:5;
	uint8_t CR_result_reg_address:5;
} condition_decode_t;

typedef struct {
	branch_decode_t branch_decoded;
	system_call_decode_t system_call_decoded;
	condition_decode_t condition_decoded;
} branch_decode_result_t;

// Types for fixed point processor
typedef struct {
	uint8_t word_size:2; // in bytes-1
	bool sum1_imm; // Use immediate for first summand
	int16_t sum1_immediate;
	uint8_t sum1_reg_address:5;
	bool sum2_imm; // Use immediate for second summand
	int16_t sum2_immediate;
	uint8_t sum2_reg_address:5;
	bool write_ea; // write effective address
	uint8_t ea_reg_address:5;
	uint8_t result_reg_address:5;
	bool sign_extend;
} load_store_decode_t;

typedef struct {
	bool subtract;
	bool op1_imm; // Use immediate for first operand
	int32_t op1_immediate;
	uint8_t op1_reg_address:5;
	bool op2_imm; // Use immediate for first operand
	int32_t op2_immediate;
	uint8_t op2_reg_address:5;
	uint8_t result_reg_address:5;
	bool alter_CA;
	bool alter_CR0;
	bool alter_OV;
	bool add_CA;
	bool sub_one;
} add_sub_decode_t;

typedef struct {
	uint8_t op1_reg_address:5;
	bool op2_imm; // Use immediate for first operand
	int32_t op2_immediate;
	uint8_t op2_reg_address:5;
	uint8_t result_reg_address:5;
	bool mul_signed;
	bool mul_higher;
	bool alter_CR0;
	bool alter_OV;
} mul_decode_t;

typedef struct {
	uint8_t dividend_reg_address:5;
	uint8_t divisor_reg_address:5;
	uint8_t result_reg_address:5;
	bool div_signed;
	bool alter_CR0;
	bool alter_OV;
} div_decode_t;

typedef struct {
	uint8_t op1_reg_address:5;
	bool op2_imm; // Use immediate for second operand
	int32_t op2_immediate;
	uint8_t op2_reg_address:5;
	bool cmp_signed;
	uint8_t BF:3;
} cmp_decode_t;

typedef struct {
	uint8_t op1_reg_address:5;
	bool op2_imm; // Use immediate for second operand
	int32_t op2_immediate;
	uint8_t op2_reg_address:5;
	uint8_t TO:5;
} trap_decode_t;

namespace logical {
	typedef enum {
		AND, OR, NOT, XOR, NAND, NOR,
		EQUIVALENT, AND_COMPLEMENT, OR_COMPLEMENT,
		EXTEND_SIGN_BYTE, EXTEND_SIGN_HALFWORD, // EXTEND_SIGN_WORD is not supported
		COUNT_LEDING_ZEROS_WORD, // COOUNT_LEADING_ZEROS_DOUBLEWORD is not supported
		POPULATION_COUNT_BYTES
	} logical_op_t;
}

typedef struct {
	logical::logical_op_t operation;
	uint8_t op1_reg_address:5;
	bool op2_imm; // Use immediate for second operand
	uint32_t op2_immediate;
	uint8_t op2_reg_address:5;
	uint8_t result_reg_address:5;
	bool alter_CR0;
} log_decode_t;

typedef struct {
	load_store_decode_t load_store_decoded;
	add_sub_decode_t add_sub_decoded;
	mul_decode_t mul_decoded;
	div_decode_t div_decoded;
	cmp_decode_t cmp_decoded;
	trap_decode_t trap_decoded;
	log_decode_t log_decoded;
} fixed_point_decode_result_t;

typedef struct {
	branch_decode_result_t branch_decode_result;
	fixed_point_decode_result_t fixed_point_result;
} decode_result_t;

/*
typedef enum processing_type {BRANCH_PROCESSOR, FIXED_POINT_PROCESSOR, FLOATING_POINT_PROCESSOR} processing_type_t;
typedef enum branch_type {} branch_type_t;
typedef enum fixed_point_type {LOGICAL, ARITHMETIC, STORAGE} fixed_point_type_t;
typedef enum floating_point_type {ARITHMETIC, STORAGE} floating_point_type_t;
typedef enum branch_operation {} branch_operation_t;
typedef enum fixed_point_operation {NONE, LOAD} fixed_point_operation_t;
typedef enum floating_point_operation {} floating_point_operation_t;
*/
#endif
