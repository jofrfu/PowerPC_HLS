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

#ifndef __DECODE_UTILS__
#define __DECODE_UTILS__

// Load and Store macros for easy access to the structs

#define init_load_store(decoded_struct)		\
	decoded_struct.sum1_imm = false;		\
	decoded_struct.sum1_immediate = 0;		\
	decoded_struct.sum1_reg_address = 0;	\
	decoded_struct.sum2_imm = false;		\
	decoded_struct.sum2_immediate = 0;		\
	decoded_struct.sum2_reg_address = 0;	\
	decoded_struct.sign_extend = false;		\
	decoded_struct.word_size = 0;			\
	decoded_struct.ea_reg_address = 0;		\
	decoded_struct.result_reg_address = 0;	\
	decoded_struct.write_ea = 0;

#define decode_load_store_and_zero(decoded_struct, instruction_struct, bytes) 		\
	if(instruction_struct.D_Form.RA == 0) { 										\
		decoded_struct.sum1_imm = true; 											\
		decoded_struct.sum1_immediate = 0;											\
		decoded_struct.sum1_reg_address = 0;										\
	} else {																		\
		decoded_struct.sum1_imm = false;											\
		decoded_struct.sum1_reg_address = instruction_struct.D_Form.RA; 			\
		decoded_struct.sum1_immediate = 0;											\
	}																				\
	decoded_struct.sum2_imm = true;													\
	decoded_struct.sum2_immediate = (int16_t)instruction_struct.D_Form.D;			\
	decoded_struct.word_size = bytes-1;												\
	decoded_struct.result_reg_address = instruction_struct.D_Form.RT;				\
	decoded_struct.write_ea = false;												\
	decoded_struct.ea_reg_address = 0;												\
	decoded_struct.sign_extend = false;

#define decode_load_store_and_zero_x_form(decoded_struct, instruction_struct, bytes)\
	if(instruction_struct.X_Form.RA == 0) {											\
		decoded_struct.sum1_imm = true;												\
		decoded_struct.sum1_immediate = 0;											\
		decoded_struct.sum1_reg_address = 0;										\
	} else {																		\
		decoded_struct.sum1_imm = false;											\
		decoded_struct.sum1_reg_address = instruction_struct.X_Form.RA;				\
		decoded_struct.sum1_immediate = 0;											\
	}																				\
	decoded_struct.sum2_imm = false;												\
	decoded_struct.sum2_immediate = 0;												\
	decoded_struct.sum2_reg_address = instruction_struct.X_Form.RB;					\
	decoded_struct.word_size = 0;													\
	decoded_struct.result_reg_address = instruction_struct.X_Form.RT;				\
	decoded_struct.write_ea = false;												\
	decoded_struct.ea_reg_address = 0;												\
	decoded_struct.sign_extend = false;

#define decode_load_store_and_zero_with_update(decoded_struct, instruction_struct, bytes)	\
	decoded_struct.sum1_imm = false;												\
	decoded_struct.sum1_reg_address = instruction_struct.D_Form.RA;					\
	decoded_struct.sum1_immediate = 0;												\
	decoded_struct.sum2_imm = true;													\
	decoded_struct.sum2_immediate = (int16_t)instruction_struct.D_Form.D;			\
	decoded_struct.sum2_reg_address = 0;											\
	decoded_struct.word_size = bytes-1;												\
	decoded_struct.result_reg_address = instruction_struct.D_Form.RT;				\
	decoded_struct.write_ea = true;													\
	decoded_struct.ea_reg_address = instruction_struct.D_Form.RA;					\
	decoded_struct.sign_extend = false;

#define decode_load_store_and_zero_with_update_x_form(decoded_struct, instruction_struct, bytes)	\
	decoded_struct.sum1_imm = false;												\
	decoded_struct.sum1_reg_address = instruction_struct.X_Form.RA;					\
	decoded_struct.sum1_immediate = 0;												\
	decoded_struct.sum2_imm = false;												\
	decoded_struct.sum2_immediate = 0;												\
	decoded_struct.sum2_reg_address = instruction_struct.X_Form.RB;					\
	decoded_struct.word_size = bytes-1;												\
	decoded_struct.result_reg_address = instruction_struct.D_Form.RT;				\
	decoded_struct.write_ea = true;													\
	decoded_struct.ea_reg_address = instruction_struct.D_Form.RA;					\
	decoded_struct.sign_extend = false;

#define decode_load_algebraic(decoded_struct, instruction_struct, bytes) 			\
	if(instruction_struct.D_Form.RA == 0) { 										\
		decoded_struct.sum1_imm = true; 											\
		decoded_struct.sum1_immediate = 0;											\
		decoded_struct.sum1_reg_address = 0;										\
	} else {																		\
		decoded_struct.sum1_imm = false;											\
		decoded_struct.sum1_reg_address = instruction_struct.D_Form.RA; 			\
		decoded_struct.sum1_immediate = 0;											\
	}																				\
	decoded_struct.sum2_imm = true;													\
	decoded_struct.sum2_immediate = (int16_t)instruction_struct.D_Form.D;			\
	decoded_struct.word_size = bytes-1;												\
	decoded_struct.result_reg_address = instruction_struct.D_Form.RT;				\
	decoded_struct.write_ea = false;												\
	decoded_struct.ea_reg_address = 0;												\
	decoded_struct.sign_extend = true;

#define decode_load_algebraic_with_update(decoded_struct, instruction_struct, bytes)\
	decoded_struct.sum1_imm = false;												\
	decoded_struct.sum1_reg_address = instruction_struct.D_Form.RA;					\
	decoded_struct.sum1_immediate = 0;												\
	decoded_struct.sum2_imm = true;													\
	decoded_struct.sum2_immediate = (int16_t)instruction_struct.D_Form.D;			\
	decoded_struct.sum2_reg_address = 0;											\
	decoded_struct.word_size = bytes-1;												\
	decoded_struct.result_reg_address = instruction_struct.D_Form.RT;				\
	decoded_struct.write_ea = true;													\
	decoded_struct.ea_reg_address = instruction_struct.D_Form.RA;					\
	decoded_struct.sign_extend = true;

#define decode_load_algebraic_x_form(decoded_struct, instruction_struct, bytes)		\
	if(instruction_struct.X_Form.RA == 0) {											\
		decoded_struct.sum1_imm = true;												\
		decoded_struct.sum1_immediate = 0;											\
		decoded_struct.sum1_reg_address = 0;										\
	} else {																		\
		decoded_struct.sum1_imm = false;											\
		decoded_struct.sum1_reg_address = instruction_struct.X_Form.RA;				\
		decoded_struct.sum1_immediate = 0;											\
	}																				\
	decoded_struct.sum2_imm = false;												\
	decoded_struct.sum2_immediate = 0;												\
	decoded_struct.sum2_reg_address = instruction_struct.X_Form.RB;					\
	decoded_struct.word_size = 0;													\
	decoded_struct.result_reg_address = instruction_struct.X_Form.RT;				\
	decoded_struct.write_ea = false;												\
	decoded_struct.ea_reg_address = 0;												\
	decoded_struct.sign_extend = true;

#define decode_load_algebraic_with_update_x_form(decoded_struct, instruction_struct, bytes)	\
	decoded_struct.sum1_imm = false;												\
	decoded_struct.sum1_reg_address = instruction_struct.X_Form.RA;					\
	decoded_struct.sum1_immediate = 0;												\
	decoded_struct.sum2_imm = false;												\
	decoded_struct.sum2_immediate = 0;												\
	decoded_struct.sum2_reg_address = instruction_struct.X_Form.RB;					\
	decoded_struct.word_size = bytes-1;												\
	decoded_struct.result_reg_address = instruction_struct.D_Form.RT;				\
	decoded_struct.write_ea = true;													\
	decoded_struct.ea_reg_address = instruction_struct.D_Form.RA;					\
	decoded_struct.sign_extend = true;

#define decode_load_algebraic_ds_form(decoded_struct, instruction_struct, bytes) 	\
	if(instruction_struct.DS_Form.RA == 0) { 										\
		decoded_struct.sum1_imm = true; 											\
		decoded_struct.sum1_immediate = 0;											\
		decoded_struct.sum1_reg_address = 0;										\
	} else {																		\
		decoded_struct.sum1_imm = false;											\
		decoded_struct.sum1_reg_address = instruction_struct.DS_Form.RA; 			\
		decoded_struct.sum1_immediate = 0;											\
	}																				\
	decoded_struct.sum2_imm = true;													\
	decoded_struct.sum2_immediate = ((int16_t)(instruction_struct.DS_Form.DS << 2);	\
	decoded_struct.word_size = bytes-1;												\
	decoded_struct.result_reg_address = instruction_struct.DS_Form.RT;				\
	decoded_struct.write_ea = false;												\
	decoded_struct.ea_reg_address = 0;												\
	decoded_struct.sign_extend = true;

#define decode_load_store_ds_form(decoded_struct, instruction_struct, bytes) 		\
	if(instruction_struct.DS_Form.RA == 0) { 										\
		decoded_struct.sum1_imm = true; 											\
		decoded_struct.sum1_immediate = 0;											\
		decoded_struct.sum1_reg_address = 0;										\
	} else {																		\
		decoded_struct.sum1_imm = false;											\
		decoded_struct.sum1_reg_address = instruction_struct.DS_Form.RA; 			\
		decoded_struct.sum1_immediate = 0;											\
	}																				\
	decoded_struct.sum2_imm = true;													\
	decoded_struct.sum2_immediate = ((int16_t)(instruction_struct.DS_Form.DS << 2);	\
	decoded_struct.word_size = bytes-1;												\
	decoded_struct.result_reg_address = instruction_struct.DS_Form.RT;				\
	decoded_struct.write_ea = false;												\
	decoded_struct.ea_reg_address = 0;												\
	decoded_struct.sign_extend = false;

#define decode_load_store_with_update_ds_form(decoded_struct, instruction_struct, bytes) 	\
	decoded_struct.sum1_imm = false;												\
	decoded_struct.sum1_reg_address = instruction_struct.DS_Form.RA;				\
	decoded_struct.sum1_immediate = 0;												\
	decoded_struct.sum2_imm = true;													\
	decoded_struct.sum2_immediate = ((int16_t)(instruction_struct.DS_Form.DS << 2);	\
	decoded_struct.sum2_reg_address = 0;											\
	decoded_struct.word_size = bytes-1;												\
	decoded_struct.result_reg_address = instruction_struct.DS_Form.RT;				\
	decoded_struct.write_ea = true;													\
	decoded_struct.ea_reg_address = instruction_struct.DS_Form.RA;					\
	decoded_struct.sign_extend = false;

#define init_add(decoded_struct)			\
	decoded_struct.subtract = false;		\
	decoded_struct.op1_imm = false;			\
	decoded_struct.op1_immediate = 0;		\
	decoded_struct.op1_reg_address = 0;		\
	decoded_struct.op2_imm = false;			\
	decoded_struct.op2_immediate = 0;		\
	decoded_struct.op2_reg_address = 0;		\
	decoded_struct.result_reg_address = 0;	\
	decoded_struct.alter_CA = false;		\
	decoded_struct.alter_CR0 = false;		\
	decoded_struct.alter_OV = false;		\
	decoded_struct.add_CA = false;			\
	decoded_struct.sub_one = false;

#define init_mul(decoded_struct)			\
	decoded_struct.op1_reg_address = 0;		\
	decoded_struct.op2_imm = false;			\
	decoded_struct.op2_immediate = 0;		\
	decoded_struct.op2_reg_address = 0;		\
	decoded_struct.result_reg_address = 0;	\
	decoded_struct.mul_signed = false;		\
	decoded_struct.mul_higher = false;		\
	decoded_struct.alter_CR0 = false;		\
	decoded_struct.alter_OV = false;

#define init_div(decoded_struct)			\
	decoded_struct.dividend_reg_address = 0;\
	decoded_struct.divisor_reg_address = 0;	\
	decoded_struct.result_reg_address = 0;	\
	decoded_struct.div_signed = false;		\
	decoded_struct.alter_CR0 = false;		\
	decoded_struct.alter_OV = false;

#define init_cmp(decoded_struct)			\
	decoded_struct.op1_reg_address = 0;		\
	decoded_struct.op2_imm = false;			\
	decoded_struct.op2_immediate = 0;		\
	decoded_struct.op2_reg_address = 0;		\
	decoded_struct.cmp_signed = false;		\
	decoded_struct.BF = 0;

#define init_trap(decoded_struct)			\
	decoded_struct.op1_reg_address = 0;		\
	decoded_struct.op2_imm = false;			\
	decoded_struct.op2_immediate = 0;		\
	decoded_struct.op2_reg_address = 0;		\
	decoded_struct.TO = 0;

#define init_log(decoded_struct)			\
	decoded_struct.operation = logical::AND;			\
	decoded_struct.op1_reg_address = 0;		\
	decoded_struct.op2_imm = false;			\
	decoded_struct.op2_immediate = 0;		\
	decoded_struct.op2_reg_address = 0;		\
	decoded_struct.result_reg_address = 0;	\
	decoded_struct.alter_CR0 = false;

#define init_rotate(decoded_struct)			\
	decoded_struct.shift_imm = false;		\
	decoded_struct.shift_immediate = 0;		\
	decoded_struct.shift_reg_address = 0;	\
	decoded_struct.source_reg_address = 0;	\
	decoded_struct.target_reg_address = 0;	\
	decoded_struct.MB = 0;					\
	decoded_struct.ME = 0;					\
	decoded_struct.mask_insert = false;		\
	decoded_struct.alter_CR0 = false;

#define init_shift(decoded_struct)			\
	decoded_struct.shift_imm = false;		\
	decoded_struct.shift_immediate = 0;		\
	decoded_struct.shift_reg_address = 0;	\
	decoded_struct.source_reg_address = 0;	\
	decoded_struct.target_reg_address = 0;	\
	decoded_struct.shift_left = false;		\
	decoded_struct.sign_extend = false;		\
	decoded_struct.alter_CA = false;		\
	decoded_struct.alter_CR0 = false;

#define init_branch(decoded_struct)			\
	decoded_struct.operation = BRANCH;		\
	decoded_struct.LK = 0;					\
	decoded_struct.AA = 0;					\
	decoded_struct.LI = 0;					\
	decoded_struct.BD = 0;					\
	decoded_struct.BI = 0;					\
	decoded_struct.BO = 0;					\
	decoded_struct.BH = 0;

#define init_condition(decoded_struct)		\
	decoded_struct.operation = condition::MOVE;		\
	decoded_struct.CR_op1_reg_address = 0;	\
	decoded_struct.CR_op2_reg_address = 0;	\
	decoded_struct.CR_result_reg_address = 0;

#define init_float_move(decoded_struct)		\
	decoded_struct.operation = MOVE;		\
	decoded_struct.source_reg_address = 0;	\
	decoded_struct.target_reg_address = 0;	\
	decoded_struct.alter_CR1 = false;

#define init_float_arithmetic(decoded_struct)		\
	decoded_struct.operation = floating_point::ADD;	\
	decoded_struct.op1_reg_address = 0;				\
	decoded_struct.op2_reg_address = 0;				\
	decoded_struct.result_reg_address = 0;			\
	decoded_struct.single_precision = false;		\
	decoded_struct.alter_CR1 = false;

#define init_float_madd(decoded_struct)		\
	decoded_struct.mul1_reg_address = 0;	\
	decoded_struct.mul2_reg_address = 0;	\
	decoded_struct.add_reg_address = 0;		\
	decoded_struct.result_reg_address = 0;	\
	decoded_struct.single_precision = false;\
	decoded_struct.negate_add = false;		\
	decoded_struct.negate_result = false;	\
	decoded_struct.alter_CR1 = false;

#define init_float_convert(decoded_struct)		\
	decoded_struct.source_reg_address = 0;		\
	decoded_struct.target_reg_address = 0;		\
	decoded_struct.round_to_single = false;		\
	decoded_struct.convert_to_integer = false;	\
	decoded_struct.round_toward_zero = false;	\
	decoded_struct.alter_CR1 = false;

#define init_float_compare(decoded_struct)	\
	decoded_struct.FRA = 0;					\
	decoded_struct.FRB = 0;					\
	decoded_struct.BF = 0;				\
	decoded_struct.unordered = false;

#define init_float_status(decoded_struct)	\
	decoded_struct.FRT_FRB = 0;				\
	decoded_struct.BF_BT = 0;				\
	decoded_struct.BFA = 0;					\
	decoded_struct.U = 0;					\
	decoded_struct.FLM = 0;					\
	decoded_struct.move_to_FPR = false;		\
	decoded_struct.move_to_CR = false;		\
	decoded_struct.move_to_FPSCR = false;	\
	decoded_struct.use_U = false;			\
	decoded_struct.bit_0 = false;			\
	decoded_struct.bit_1 = false;			\
	decoded_struct.alter_CR1 = false;

#endif


