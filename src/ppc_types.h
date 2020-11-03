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

#include <cstdint>
#include "registers.hpp"

typedef union {
	uint32_t instruction_bits;

	struct __attribute__ ((__packed__)) {
		uint8_t LK:1;
		uint8_t AA:1;
		uint32_t LI:24;
		uint8_t OPCD:6;
	} I_Form;

	struct __attribute__ ((__packed__)) {
		uint8_t LK:1;
		uint8_t AA:1;
		uint16_t BD:14;
		uint8_t BI:5;
		uint8_t BO:5;
		uint8_t OPCD:6;
	} B_Form;

	struct __attribute__ ((__packed__)) {
		uint8_t UNUSED_5:1;
		uint8_t ALWAYS_ONE:1;
		uint8_t UNUSED_4:3;
		uint8_t LEV:7;
		uint8_t UNUSED_3:4;
		uint8_t UNUSED_2:5;
		uint8_t UNUSED_1:5;
		uint8_t OPCD:6;
	} SC_Form;

	struct __attribute__ ((__packed__)) {
		uint16_t D:16;
		uint8_t RA:5;
		uint8_t RT:5;
		uint8_t OPCD:6;
	} D_Form;

	struct __attribute__ ((__packed__)) {
		uint8_t XO:2;
		uint16_t DS:14;
		uint8_t RA:5;
		uint8_t RT:5;
		uint8_t OPCD:6;
	} DS_Form;

	struct __attribute__ ((__packed__)) {
		uint8_t Rc:1;
		uint16_t XO:10;
		uint8_t RB:5;
		uint8_t RA:5;
		uint8_t RT:5;
		uint8_t OPCD:6;
	} X_Form;

	struct __attribute__ ((__packed__)) {
		uint8_t LK:1;
		uint16_t XO:10;
		uint8_t BB:5;
		uint8_t BA:5;
		uint8_t BT:5;
		uint8_t OPCD:6;
	} XL_Form;

	struct __attribute__ ((__packed__)) {
		uint8_t UNUSED_1:1;
		uint16_t XO:10;
		uint16_t spr:10;
		uint8_t RT:5;
		uint8_t OPCD:6;
	} XFX_Form;

	struct __attribute__ ((__packed__)) {
		uint8_t Rc:1;
		uint16_t XO:10;
		uint8_t FRB:5;
		uint8_t UNUSED_2:1;
		uint8_t FLM:8;
		uint8_t UNUSED_1:1;
		uint8_t OPCD:6;
	} XFL_Form;

	struct __attribute__ ((__packed__)) {
		uint8_t Rc:1;
		uint8_t SH_2:1;
		uint16_t XO:9;
		uint8_t SH_1:5;
		uint8_t RA:5;
		uint8_t RS:5;
		uint8_t OPCD:6;
	} XS_Form;

	struct __attribute__ ((__packed__)) {
		uint8_t Rc:1;
		uint16_t XO:9;
		uint8_t OE:1;
		uint8_t RB:5;
		uint8_t RA:5;
		uint8_t RT:5;
		uint8_t OPCD:6;
	} XO_Form;

	struct __attribute__ ((__packed__)) {
		uint8_t Rc:1;
		uint8_t XO:5;
		uint8_t FRC:5;
		uint8_t FRB:5;
		uint8_t FRA:5;
		uint8_t FRT:5;
		uint8_t OPCD:6;
	} A_Form;

	struct __attribute__ ((__packed__)) {
		uint8_t Rc:1;
		uint8_t ME:5;
		uint8_t MB:5;
		uint8_t RB:5;
		uint8_t RA:5;
		uint8_t RS:5;
		uint8_t OPCD:6;
	} M_Form;

	struct __attribute__ ((__packed__)) {
		uint8_t Rc:1;
		uint8_t SH_2:1;
		uint8_t XO:3;
		uint8_t MB:6;
		uint8_t SH_1:5;
		uint8_t RA:5;
		uint8_t RS:5;
		uint8_t OPCD:6;
	} MD_Form;

	struct __attribute__ ((__packed__)) {
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
	bool execute_branch;
	branch_decode_t branch_decoded;
	bool execute_system_call;
	system_call_decode_t system_call_decoded;
	bool execute_condition;
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
	bool little_endian;
	bool multiple;
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
		AND, OR, XOR, NAND, NOR,
		EQUIVALENT, AND_COMPLEMENT, OR_COMPLEMENT,
		EXTEND_SIGN_BYTE, EXTEND_SIGN_HALFWORD, // EXTEND_SIGN_WORD is not supported
		COUNT_LEDING_ZEROS_WORD, // COOUNT_LEADING_ZEROS_DOUBLEWORD is not supported
		// POPULATION_COUNT_BYTES // popcntb doesn't seem to exist on 32 bit implementations, but it's not mentioned anywhere
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
	bool shift_imm;
	uint8_t shift_immediate:5;
	uint8_t shift_reg_address:5;
	uint8_t source_reg_address:5;
	uint8_t target_reg_address:5;
	uint8_t MB:5;
	uint8_t ME:5;
	bool mask_insert;
	bool alter_CR0;
} rotate_decode_t;

typedef struct {
	bool shift_imm;
	uint8_t shift_immediate:5;
	uint8_t shift_reg_address:5;
	uint8_t source_reg_address:5;
	uint8_t target_reg_address:5;
	bool shift_left;
	bool sign_extend;
	bool alter_CA;
	bool alter_CR0;
} shift_decode_t;

namespace system_ppc {
	typedef enum {MOVE_TO_SPR, MOVE_FROM_SPR, MOVE_TO_CR, MOVE_FROM_CR} system_op_t;
}

typedef struct {
	system_ppc::system_op_t operation;
	uint8_t RS_RT:5; // RS or RT reg address
	uint16_t SPR:10; // Special purpose register address
	uint8_t FXM; // Field mask
} system_decode_t;

typedef struct {
	bool execute_load;
	bool execute_store;
	bool execute_load_string;
	bool execute_store_string;
	load_store_decode_t load_store_decoded;
	bool execute_add_sub;
	add_sub_decode_t add_sub_decoded;
	bool execute_mul;
	mul_decode_t mul_decoded;
	bool execute_div;
	div_decode_t div_decoded;
	bool execute_compare;
	cmp_decode_t cmp_decoded;
	bool execute_trap;
	trap_decode_t trap_decoded;
	bool execute_logical;
	log_decode_t log_decoded;
	bool execute_rotate;
	rotate_decode_t rotate_decoded;
	bool execute_shift;
	shift_decode_t shift_decoded;
	bool execute_system;
	system_decode_t system_decoded;
} fixed_point_decode_result_t;

// Types for floating point processor
typedef struct {
	uint8_t word_size:3; // in bytes-1
	bool sum1_imm; // Use immediate for first summand
	int16_t sum1_immediate;
	uint8_t sum1_reg_address:5;
	bool sum2_imm; // Use immediate for second summand
	int16_t sum2_immediate;
	uint8_t sum2_reg_address:5;
	bool write_ea; // write effective address
	uint8_t ea_reg_address:5;
	uint8_t result_reg_address:5;
	bool sign_extend; // Sign extend means, store as integer word for floating point
	bool little_endian; // Unused for floating point
	bool multiple; // Unused for floating point
} float_load_store_decode_t;

typedef enum {MOVE, NEGATE, ABSOLUTE, NEGATIVE_ABSOLUTE} float_move_op_t;

typedef struct {
	float_move_op_t operation;
	uint8_t source_reg_address:5;
	uint8_t target_reg_address:5;
	bool alter_CR1;
} float_move_decode_t;

namespace floating_point {
	typedef enum {ADD, SUBTRACT, MULTIPLY, DIVIDE} float_arithmetic_op_t;
}

typedef struct {
	floating_point::float_arithmetic_op_t operation;
	uint8_t op1_reg_address:5;
	uint8_t op2_reg_address:5;
	uint8_t result_reg_address:5;
	bool single_precision;
	bool alter_CR1;
} float_arithmetic_decode_t;

typedef struct {
	uint8_t mul1_reg_address:5;
	uint8_t mul2_reg_address:5;
	uint8_t add_reg_address:5;
	uint8_t result_reg_address:5;
	bool single_precision;
	bool negate_add;
	bool negate_result;
	bool alter_CR1;
} float_madd_decode_t;

typedef struct {
	uint8_t source_reg_address:5;
	uint8_t target_reg_address:5;
	bool round_to_single;
	bool convert_to_integer;
	bool round_toward_zero;
	bool alter_CR1;
} float_convert_decode_t;

typedef struct {
	uint8_t FRA:5;
	uint8_t FRB:5;
	uint8_t BF:3;
	bool unordered;
} float_compare_decode_t;

typedef struct {
	uint8_t FRT_FRB:5; // FRT or FRB
	uint8_t BF_BT:5; // BF or BT
	uint8_t BFA:3;
	uint8_t U:4;
	uint8_t FLM;
	bool move_to_FPR;
	bool move_to_CR;
	bool move_to_FPSCR;
	bool use_U;
	bool bit_0;
	bool bit_1;
	bool alter_CR1;
} float_status_decode_t;

typedef struct {
	bool execute_load;
	bool execute_store;
	float_load_store_decode_t float_load_store_decoded;
	bool execute_move;
	float_move_decode_t float_move_decoded;
	bool execute_arithmetic;
	float_arithmetic_decode_t float_arithmetic_decoded;
	bool execute_madd;
	float_madd_decode_t float_madd_decoded;
	bool execute_convert;
	float_convert_decode_t float_convert_decoded;
	bool execute_compare;
	float_compare_decode_t float_compare_decoded;
	bool execute_status;
	float_status_decode_t float_status_decoded;
} floating_point_decode_result_t;

typedef struct {
	branch_decode_result_t branch_decode_result;
	fixed_point_decode_result_t fixed_point_decode_result;
	floating_point_decode_result_t floating_point_decode_result;
} decode_result_t;

#endif
