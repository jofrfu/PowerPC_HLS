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

#include "instruction_decode.hpp"
#include "decode_utils.hpp"

decode_result_t decode(uint32_t instruction_port) {
	instruction_t instruction;
	instruction.instruction_bits = instruction_port;

	decode_result_t decode_result;

	// branch processor decode structures
	branch_decode_result_t branch_result;
	branch_result.execute_branch = false;
	branch_result.execute_system_call = false;
	branch_result.execute_condition = false;

	branch_decode_t branch_decoded;
	init_branch(branch_decoded)

	system_call_decode_t system_call_decoded = 0;

	condition_decode_t condition_decoded;
	init_condition(condition_decoded)

	// fixed point processor decode structures
	fixed_point_decode_result_t fixed_point_decode_result;
	fixed_point_decode_result.execute_load = false;
	fixed_point_decode_result.execute_store = false;
	fixed_point_decode_result.execute_add_sub = false;
	fixed_point_decode_result.execute_mul = false;
	fixed_point_decode_result.execute_div = false;
	fixed_point_decode_result.execute_compare = false;
	fixed_point_decode_result.execute_trap = false;
	fixed_point_decode_result.execute_logical = false;
	fixed_point_decode_result.execute_rotate = false;
	fixed_point_decode_result.execute_shift = false;
	fixed_point_decode_result.execute_system = false;

	load_store_decode_t load_store_decoded;
	init_load_store(load_store_decoded)

	add_sub_decode_t add_sub_decoded;
	init_add(add_sub_decoded)

	mul_decode_t mul_decoded;
	init_mul(mul_decoded)

	div_decode_t div_decoded;
	init_div(div_decoded)

	cmp_decode_t cmp_decoded;
	init_cmp(cmp_decoded)

	trap_decode_t trap_decoded;
	init_trap(trap_decoded)

	log_decode_t log_decoded;
	init_log(log_decoded)

	rotate_decode_t rotate_decoded;
	init_rotate(rotate_decoded)

	shift_decode_t shift_decoded;
	init_shift(shift_decoded)

	system_decode_t system_decoded;
	init_system(system_decoded)

	// floating point processor decode structures
	floating_point_decode_result_t floating_point_decode_result;
	floating_point_decode_result.execute_load = false;
	floating_point_decode_result.execute_store = false;
	floating_point_decode_result.execute_move = false;
	floating_point_decode_result.execute_arithmetic = false;
	floating_point_decode_result.execute_madd = false;
	floating_point_decode_result.execute_convert = false;
	floating_point_decode_result.execute_compare = false;
	floating_point_decode_result.execute_status = false;

	float_load_store_decode_t float_load_store_decoded;
	init_load_store(float_load_store_decoded)

	float_move_decode_t float_move_decoded;
	init_float_move(float_move_decoded)

	float_arithmetic_decode_t float_arithmetic_decoded;
	init_float_arithmetic(float_arithmetic_decoded)

	float_madd_decode_t float_madd_decoded;
	init_float_madd(float_madd_decoded)

	float_convert_decode_t float_convert_decoded;
	init_float_convert(float_convert_decoded)

	float_compare_decode_t float_compare_decoded;
	init_float_compare(float_compare_decoded)

	float_status_decode_t float_status_decoded;
	init_float_status(float_status_decoded)

	switch(instruction.I_Form.OPCD) {
		// B Form Branch instructions
		case 16: // bc, bca, bcl, bcla
			branch_result.execute_branch = true;
			branch_decoded.operation = BRANCH;
			branch_decoded.LK = instruction.B_Form.LK;
			branch_decoded.AA = instruction.B_Form.AA;
			branch_decoded.LI = 0;
			branch_decoded.BD = instruction.B_Form.BD;
			branch_decoded.BI = instruction.B_Form.BI;
			branch_decoded.BO = instruction.B_Form.BO;
			branch_decoded.BH = 0;
			break;
		// I Form Branch instructions
		case 18: // b, ba, bl, bla
			branch_result.execute_branch = true;
			branch_decoded.operation = BRANCH_CONDITIONAL;
			branch_decoded.LK = instruction.I_Form.LK;
			branch_decoded.AA = instruction.I_Form.AA;
			branch_decoded.LI = instruction.I_Form.LI;
			branch_decoded.BD = 0;
			branch_decoded.BI = 0;
			branch_decoded.BO = 0;
			branch_decoded.BH = 0;
			break;
		case 19:
			switch(instruction.XL_Form.XO) {
				// XL Form Branch instructions
				case 16: // bclr, bclrl
					branch_result.execute_branch = true;
					branch_decoded.operation = BRANCH_CONDITIONAL_LINK;
					branch_decoded.LK = instruction.XL_Form.LK;
					branch_decoded.AA = 0;
					branch_decoded.LI = 0;
					branch_decoded.BD = 0;
					branch_decoded.BI = instruction.XL_Form.BA; // BI is BA in XL Form
					branch_decoded.BO = instruction.XL_Form.BT; // BO is BT in XL Form
					branch_decoded.BH = instruction.XL_Form.BB; // BH is part of BH in XL Form
					break;
				case 528: // bcctr, bcctrl
					branch_result.execute_branch = true;
					branch_decoded.operation = BRANCH_CONDITIONAL_COUNT;
					branch_decoded.LK = instruction.XL_Form.LK;
					branch_decoded.AA = 0;
					branch_decoded.LI = 0;
					branch_decoded.BD = 0;
					branch_decoded.BI = instruction.XL_Form.BA; // BI is BA in XL Form
					branch_decoded.BO = instruction.XL_Form.BT; // BO is BT in XL Form
					branch_decoded.BH = instruction.XL_Form.BB; // BH is part of BH in XL Form
					break;
				// XL Form Condition instructions
				case 0: // mcrf
					branch_result.execute_condition = true;
					condition_decoded.operation = condition::MOVE;
					condition_decoded.CR_op1_reg_address = instruction.XL_Form.BA;
					condition_decoded.CR_op2_reg_address = instruction.XL_Form.BB;
					condition_decoded.CR_result_reg_address = instruction.XL_Form.BT;
					break;
				case 33: // crnor
					branch_result.execute_condition = true;
					condition_decoded.operation = condition::NOR;
					condition_decoded.CR_op1_reg_address = instruction.XL_Form.BA;
					condition_decoded.CR_op2_reg_address = instruction.XL_Form.BB;
					condition_decoded.CR_result_reg_address = instruction.XL_Form.BT;
					break;
				case 129: // crandc
					branch_result.execute_condition = true;
					condition_decoded.operation = condition::AND_COMPLEMENT;
					condition_decoded.CR_op1_reg_address = instruction.XL_Form.BA;
					condition_decoded.CR_op2_reg_address = instruction.XL_Form.BB;
					condition_decoded.CR_result_reg_address = instruction.XL_Form.BT;
					break;
				case 193: // crxor
					branch_result.execute_condition = true;
					condition_decoded.operation = condition::XOR;
					condition_decoded.CR_op1_reg_address = instruction.XL_Form.BA;
					condition_decoded.CR_op2_reg_address = instruction.XL_Form.BB;
					condition_decoded.CR_result_reg_address = instruction.XL_Form.BT;
					break;
				case 257: // crand
					branch_result.execute_condition = true;
					condition_decoded.operation = condition::AND;
					condition_decoded.CR_op1_reg_address = instruction.XL_Form.BA;
					condition_decoded.CR_op2_reg_address = instruction.XL_Form.BB;
					condition_decoded.CR_result_reg_address = instruction.XL_Form.BT;
					break;
				case 289: // creqv
					branch_result.execute_condition = true;
					condition_decoded.operation = condition::EQUIVALENT;
					condition_decoded.CR_op1_reg_address = instruction.XL_Form.BA;
					condition_decoded.CR_op2_reg_address = instruction.XL_Form.BB;
					condition_decoded.CR_result_reg_address = instruction.XL_Form.BT;
					break;
				case 417: // crorc
					branch_result.execute_condition = true;
					condition_decoded.operation = condition::OR_COMPLEMENT;
					condition_decoded.CR_op1_reg_address = instruction.XL_Form.BA;
					condition_decoded.CR_op2_reg_address = instruction.XL_Form.BB;
					condition_decoded.CR_result_reg_address = instruction.XL_Form.BT;
					break;
				case 449: // cror
					branch_result.execute_condition = true;
					condition_decoded.operation = condition::OR;
					condition_decoded.CR_op1_reg_address = instruction.XL_Form.BA;
					condition_decoded.CR_op2_reg_address = instruction.XL_Form.BB;
					condition_decoded.CR_result_reg_address = instruction.XL_Form.BT;
					break;
			}
			break;
		case 17:
			switch(instruction.SC_Form.ALWAYS_ONE) {
				// SC Form System Call instructions
				case 0: // Invalid instruction!
					break;
				case 1: // sc
					branch_result.execute_system_call = true;
					system_call_decoded = instruction.SC_Form.LEV;
					break;
			}
			break;
		// M Form rotate instructions
		case 20: // rlwimi, rlwimi.
			fixed_point_decode_result.execute_rotate = true;
			rotate_decoded.shift_imm = true;
			rotate_decoded.shift_immediate = instruction.M_Form.RB;
			rotate_decoded.shift_reg_address = 0;
			rotate_decoded.source_reg_address = instruction.M_Form.RS;
			rotate_decoded.target_reg_address = instruction.M_Form.RA;
			rotate_decoded.MB = instruction.M_Form.MB;
			rotate_decoded.ME = instruction.M_Form.ME;
			rotate_decoded.mask_insert = true;
			if(instruction.M_Form.Rc == 1) {
				rotate_decoded.alter_CR0 = true;
			} else {
				rotate_decoded.alter_CR0 = false;
			}
			break;
		case 21: // rlwinm, rlwinm.
			fixed_point_decode_result.execute_rotate = true;
			rotate_decoded.shift_imm = true;
			rotate_decoded.shift_immediate = instruction.M_Form.RB;
			rotate_decoded.shift_reg_address = 0;
			rotate_decoded.source_reg_address = instruction.M_Form.RS;
			rotate_decoded.target_reg_address = instruction.M_Form.RA;
			rotate_decoded.MB = instruction.M_Form.MB;
			rotate_decoded.ME = instruction.M_Form.ME;
			rotate_decoded.mask_insert = false;
			if(instruction.M_Form.Rc == 1) {
				rotate_decoded.alter_CR0 = true;
			} else {
				rotate_decoded.alter_CR0 = false;
			}
			break;
		case 23: // rlwnm, rlwnm.
			fixed_point_decode_result.execute_rotate = true;
			rotate_decoded.shift_imm = true;
			rotate_decoded.shift_immediate = 0;
			rotate_decoded.shift_reg_address = instruction.M_Form.RB;
			rotate_decoded.source_reg_address = instruction.M_Form.RS;
			rotate_decoded.target_reg_address = instruction.M_Form.RA;
			rotate_decoded.MB = instruction.M_Form.MB;
			rotate_decoded.ME = instruction.M_Form.ME;
			rotate_decoded.mask_insert = false;
			if(instruction.M_Form.Rc == 1) {
				rotate_decoded.alter_CR0 = true;
			} else {
				rotate_decoded.alter_CR0 = false;
			}
			break;
		case 30:
			switch(instruction.MD_Form.XO) {
				// MD Form rotate instructions
				case 0: // rldicl, rldicl.
					// NOT SUPPORTED!!!
					break;
				case 1: // rldicr, rldicr.
					// NOT SUPPORTED!!!
					break;
				case 2: // rldic, rldic.
					// NOT SUPPORTED!!!
					break;
				case 3: // rldimi, rldimi.
					// NOT SUPPORTED!!!
					break;
			}
			switch(instruction.MDS_Form.XO) {
				// MDS Form rotate instructions
				case 8: // rldcl, rldcl.
					// NOT SUPPORTED!!!
					break;
				case 9: // rldcr, rldcr.
					// NOT SUPPORTED!!!
					break;
			}
			break;
		case 31:
			switch(instruction.X_Form.XO) {
				// X Form load instructions
				case 21: // ldx
					// NOT SUPPORTED!!!
					break;
				case 23: // lwzx
					fixed_point_decode_result.execute_load = true;
					decode_load_store_and_zero_x_form(load_store_decoded, instruction, 4)
					break;
				case 53: // ldux
					// NOT SUPPORTED!!!
					break;
				case 55: // lwzux
					fixed_point_decode_result.execute_load = true;
					decode_load_store_and_zero_with_update_x_form(load_store_decoded, instruction, 4)
					break;
				case 87: // lbzx
					fixed_point_decode_result.execute_load = true;
					decode_load_store_and_zero_x_form(load_store_decoded, instruction, 1)
					break;
				case 119: // lbzux
					fixed_point_decode_result.execute_load = true;
					decode_load_store_and_zero_with_update_x_form(load_store_decoded, instruction, 1)
					break;
				case 279: // lhzx
					fixed_point_decode_result.execute_load = true;
					decode_load_store_and_zero_x_form(load_store_decoded, instruction, 2)
					break;
				case 311: // lhzux
					fixed_point_decode_result.execute_load = true;
					decode_load_store_and_zero_with_update_x_form(load_store_decoded, instruction, 2)
					break;
				case 341: // lwax
					fixed_point_decode_result.execute_load = true;
					decode_load_algebraic_x_form(load_store_decoded, instruction, 4)
					break;
				case 343: // lhax
					fixed_point_decode_result.execute_load = true;
					decode_load_algebraic_x_form(load_store_decoded, instruction, 2)
					break;
				case 373: // lwaux
					fixed_point_decode_result.execute_load = true;
					decode_load_algebraic_with_update_x_form(load_store_decoded, instruction, 4);
					break;
				case 375: // lhaux
					fixed_point_decode_result.execute_load = true;
					decode_load_algebraic_with_update_x_form(load_store_decoded, instruction, 2)
					break;
				// X Form store instructions
				case 149: // stdx
					// NOT SUPPORTED!!!
					break;
				case 151: // stwx
					fixed_point_decode_result.execute_store = true;
					decode_load_store_and_zero_x_form(load_store_decoded, instruction, 4)
					break;
				case 181: // stdux
					// NOT SUPPORTED!!!
					break;
				case 183: // stwux
					fixed_point_decode_result.execute_store = true;
					decode_load_store_and_zero_with_update_x_form(load_store_decoded, instruction, 4)
					break;
				case 215: // stbx
					fixed_point_decode_result.execute_store = true;
					decode_load_store_and_zero_x_form(load_store_decoded, instruction, 1)
					break;
				case 247: // stbux
					fixed_point_decode_result.execute_store = true;
					decode_load_store_and_zero_with_update_x_form(load_store_decoded, instruction, 1)
					break;
				case 407: // sthx
					fixed_point_decode_result.execute_store = true;
					decode_load_store_and_zero_x_form(load_store_decoded, instruction, 2)
					break;
				case 439: // sthux
					fixed_point_decode_result.execute_store = true;
					decode_load_store_and_zero_with_update_x_form(load_store_decoded, instruction, 2)
					break;
				// Load/Store reverse order (little endian)
				case 534: // lwbrx
					// NOT SUPPORTED!!! TODO: Support
					break;
				case 662: // stwbrx
					// NOT SUPPORTED!!! TODO: Support
					break;
				case 790: // lhbrx
					// NOT SUPPORTED!!! TODO: Support
					break;
				case 918: // sthbrx
					// NOT SUPPORTED!!! TODO: Support
					break;
				// Load/Store string word
				case 533: // lswx
					// NOT SUPPORTED!!! TODO: Support
					break;
				case 597: // lswi
					// NOT SUPPORTED!!! TODO: Support
					break;
				case 661: // stswx
					// NOT SUPPORTED!!! TODO: Support
					break;
				case 725: // stswi
					// NOT SUPPORTED!!! TODO: Support
					break;
				// XO Form Add/Sub instructions
				case 8 | 0b1000000000:
				case 8: // addc, addc., addco, addco.
					fixed_point_decode_result.execute_add_sub = true;
					add_sub_decoded.subtract = true;
					add_sub_decoded.op1_imm = false;
					add_sub_decoded.op1_immediate = 0;
					add_sub_decoded.op1_reg_address = instruction.XO_Form.RA;
					add_sub_decoded.op2_imm = false;
					add_sub_decoded.op2_immediate = 0;
					add_sub_decoded.op2_reg_address = instruction.XO_Form.RB;
					add_sub_decoded.result_reg_address = instruction.XO_Form.RT;
					add_sub_decoded.alter_CA = true;
					if(instruction.XO_Form.Rc == 1) {
						add_sub_decoded.alter_CR0 = true;
					} else {
						add_sub_decoded.alter_CR0 = false;
					}
					if(instruction.XO_Form.OE == 1) {
						add_sub_decoded.alter_OV = true;
					} else {
						add_sub_decoded.alter_OV = false;
					}
					add_sub_decoded.add_CA = false;
					add_sub_decoded.sub_one = false;
					break;
				case 10 | 0b1000000000:
				case 10: // addc, addc., addco, addco.
					fixed_point_decode_result.execute_add_sub = true;
					add_sub_decoded.subtract = false;
					add_sub_decoded.op1_imm = false;
					add_sub_decoded.op1_immediate = 0;
					add_sub_decoded.op1_reg_address = instruction.XO_Form.RA;
					add_sub_decoded.op2_imm = false;
					add_sub_decoded.op2_immediate = 0;
					add_sub_decoded.op2_reg_address = instruction.XO_Form.RB;
					add_sub_decoded.result_reg_address = instruction.XO_Form.RT;
					add_sub_decoded.alter_CA = true;
					if(instruction.XO_Form.Rc == 1) {
						add_sub_decoded.alter_CR0 = true;
					} else {
						add_sub_decoded.alter_CR0 = false;
					}
					if(instruction.XO_Form.OE == 1) {
						add_sub_decoded.alter_OV = true;
					} else {
						add_sub_decoded.alter_OV = false;
					}
					add_sub_decoded.add_CA = false;
					add_sub_decoded.sub_one = false;
					break;
				case 40 | 0b1000000000:
				case 40: // subf, subf., subfo, subfo.
					fixed_point_decode_result.execute_add_sub = true;
					add_sub_decoded.subtract = true;
					add_sub_decoded.op1_imm = false;
					add_sub_decoded.op1_immediate = 0;
					add_sub_decoded.op1_reg_address = instruction.XO_Form.RA;
					add_sub_decoded.op2_imm = false;
					add_sub_decoded.op2_immediate = 0;
					add_sub_decoded.op2_reg_address = instruction.XO_Form.RB;
					add_sub_decoded.result_reg_address = instruction.XO_Form.RT;
					add_sub_decoded.alter_CA = false;
					if(instruction.XO_Form.Rc == 1) {
						add_sub_decoded.alter_CR0 = true;
					} else {
						add_sub_decoded.alter_CR0 = false;
					}
					if(instruction.XO_Form.OE == 1) {
						add_sub_decoded.alter_OV = true;
					} else {
						add_sub_decoded.alter_OV = false;
					}
					add_sub_decoded.add_CA = false;
					add_sub_decoded.sub_one = false;
					break;
				case 104 | 0b1000000000:
				case 104: // neg, neg., nego, nego.
					fixed_point_decode_result.execute_add_sub = true;
					add_sub_decoded.subtract = true;
					add_sub_decoded.op1_imm = false;
					add_sub_decoded.op1_immediate = 0;
					add_sub_decoded.op1_reg_address = instruction.XO_Form.RA;
					add_sub_decoded.op2_imm = true;
					add_sub_decoded.op2_immediate = 0;
					add_sub_decoded.op2_reg_address = 0;
					add_sub_decoded.result_reg_address = instruction.XO_Form.RT;
					add_sub_decoded.alter_CA = false;
					if(instruction.XO_Form.Rc == 1) {
						add_sub_decoded.alter_CR0 = true;
					} else {
						add_sub_decoded.alter_CR0 = false;
					}
					if(instruction.XO_Form.OE == 1) {
						add_sub_decoded.alter_OV = true;
					} else {
						add_sub_decoded.alter_OV = false;
					}
					add_sub_decoded.add_CA = false;
					add_sub_decoded.sub_one = false;
					break;
				case 136 | 0b1000000000:
				case 136: // subfe, subfe., subfeo, subfeo.
					fixed_point_decode_result.execute_add_sub = true;
					add_sub_decoded.subtract = true;
					add_sub_decoded.op1_imm = false;
					add_sub_decoded.op1_immediate = 0;
					add_sub_decoded.op1_reg_address = instruction.XO_Form.RA;
					add_sub_decoded.op2_imm = false;
					add_sub_decoded.op2_immediate = 0;
					add_sub_decoded.op2_reg_address = instruction.XO_Form.RB;
					add_sub_decoded.result_reg_address = instruction.XO_Form.RT;
					add_sub_decoded.alter_CA = true;
					if(instruction.XO_Form.Rc == 1) {
						add_sub_decoded.alter_CR0 = true;
					} else {
						add_sub_decoded.alter_CR0 = false;
					}
					if(instruction.XO_Form.OE == 1) {
						add_sub_decoded.alter_OV = true;
					} else {
						add_sub_decoded.alter_OV = false;
					}
					add_sub_decoded.add_CA = true;
					add_sub_decoded.sub_one = false;
					break;
				case 138 | 0b1000000000:
				case 138: // adde, adde., addeo, addeo.
					fixed_point_decode_result.execute_add_sub = true;
					add_sub_decoded.subtract = false;
					add_sub_decoded.op1_imm = false;
					add_sub_decoded.op1_immediate = 0;
					add_sub_decoded.op1_reg_address = instruction.XO_Form.RA;
					add_sub_decoded.op2_imm = false;
					add_sub_decoded.op2_immediate = 0;
					add_sub_decoded.op2_reg_address = instruction.XO_Form.RB;
					add_sub_decoded.result_reg_address = instruction.XO_Form.RT;
					add_sub_decoded.alter_CA = true;
					if(instruction.XO_Form.Rc == 1) {
						add_sub_decoded.alter_CR0 = true;
					} else {
						add_sub_decoded.alter_CR0 = false;
					}
					if(instruction.XO_Form.OE == 1) {
						add_sub_decoded.alter_OV = true;
					} else {
						add_sub_decoded.alter_OV = false;
					}
					add_sub_decoded.add_CA = true;
					add_sub_decoded.sub_one = false;
					break;
				case 200 | 0b1000000000:
				case 200: // subfze, subfze., subfzeo, subfzeo.
					fixed_point_decode_result.execute_add_sub = true;
					add_sub_decoded.subtract = true;
					add_sub_decoded.op1_imm = false;
					add_sub_decoded.op1_immediate = 0;
					add_sub_decoded.op1_reg_address = instruction.XO_Form.RA;
					add_sub_decoded.op2_imm = true;
					add_sub_decoded.op2_immediate = 0;
					add_sub_decoded.op2_reg_address = 0;
					add_sub_decoded.result_reg_address = instruction.XO_Form.RT;
					add_sub_decoded.alter_CA = true;
					if(instruction.XO_Form.Rc == 1) {
						add_sub_decoded.alter_CR0 = true;
					} else {
						add_sub_decoded.alter_CR0 = false;
					}
					if(instruction.XO_Form.OE == 1) {
						add_sub_decoded.alter_OV = true;
					} else {
						add_sub_decoded.alter_OV = false;
					}
					add_sub_decoded.add_CA = true;
					add_sub_decoded.sub_one = true;
					break;
				case 202 | 0b1000000000:
				case 202: // addze, addze., addzeo, addzeo.
					fixed_point_decode_result.execute_add_sub = true;
					add_sub_decoded.subtract = false;
					add_sub_decoded.op1_imm = false;
					add_sub_decoded.op1_immediate = 0;
					add_sub_decoded.op1_reg_address = instruction.XO_Form.RA;
					add_sub_decoded.op2_imm = true;
					add_sub_decoded.op2_immediate = 0;
					add_sub_decoded.op2_reg_address = 0;
					add_sub_decoded.result_reg_address = instruction.XO_Form.RT;
					add_sub_decoded.alter_CA = true;
					if(instruction.XO_Form.Rc == 1) {
						add_sub_decoded.alter_CR0 = true;
					} else {
						add_sub_decoded.alter_CR0 = false;
					}
					if(instruction.XO_Form.OE == 1) {
						add_sub_decoded.alter_OV = true;
					} else {
						add_sub_decoded.alter_OV = false;
					}
					add_sub_decoded.add_CA = true;
					add_sub_decoded.sub_one = false;
					break;
				case 232 | 0b1000000000:
				case 232: // subfme, subfme., subfmeo, subfmeo.
					fixed_point_decode_result.execute_add_sub = true;
					add_sub_decoded.subtract = true;
					add_sub_decoded.op1_imm = false;
					add_sub_decoded.op1_immediate = 0;
					add_sub_decoded.op1_reg_address = instruction.XO_Form.RA;
					add_sub_decoded.op2_imm = true;
					add_sub_decoded.op2_immediate = 0;
					add_sub_decoded.op2_reg_address = 0;
					add_sub_decoded.result_reg_address = instruction.XO_Form.RT;
					add_sub_decoded.alter_CA = true;
					if(instruction.XO_Form.Rc == 1) {
						add_sub_decoded.alter_CR0 = true;
					} else {
						add_sub_decoded.alter_CR0 = false;
					}
					if(instruction.XO_Form.OE == 1) {
						add_sub_decoded.alter_OV = true;
					} else {
						add_sub_decoded.alter_OV = false;
					}
					add_sub_decoded.add_CA = true;
					add_sub_decoded.sub_one = true;
					break;
				case 234 | 0b1000000000:
				case 234: // addme, addme., addmeo, addmeo.
					fixed_point_decode_result.execute_add_sub = true;
					add_sub_decoded.subtract = false;
					add_sub_decoded.op1_imm = false;
					add_sub_decoded.op1_immediate = 0;
					add_sub_decoded.op1_reg_address = instruction.XO_Form.RA;
					add_sub_decoded.op2_imm = true;
					add_sub_decoded.op2_immediate = 0;
					add_sub_decoded.op2_reg_address = 0;
					add_sub_decoded.result_reg_address = instruction.XO_Form.RT;
					add_sub_decoded.alter_CA = true;
					if(instruction.XO_Form.Rc == 1) {
						add_sub_decoded.alter_CR0 = true;
					} else {
						add_sub_decoded.alter_CR0 = false;
					}
					if(instruction.XO_Form.OE == 1) {
						add_sub_decoded.alter_OV = true;
					} else {
						add_sub_decoded.alter_OV = false;
					}
					add_sub_decoded.add_CA = true;
					add_sub_decoded.sub_one = true;
					break;
				case 266 | 0b1000000000:
				case 266: // add, add., addo, addo.
					fixed_point_decode_result.execute_add_sub = true;
					add_sub_decoded.subtract = false;
					add_sub_decoded.op1_imm = false;
					add_sub_decoded.op1_immediate = 0;
					add_sub_decoded.op1_reg_address = instruction.XO_Form.RA;
					add_sub_decoded.op2_imm = false;
					add_sub_decoded.op2_immediate = 0;
					add_sub_decoded.op2_reg_address = instruction.XO_Form.RB;
					add_sub_decoded.result_reg_address = instruction.XO_Form.RT;
					add_sub_decoded.alter_CA = false;
					if(instruction.XO_Form.Rc == 1) {
						add_sub_decoded.alter_CR0 = true;
					} else {
						add_sub_decoded.alter_CR0 = false;
					}
					if(instruction.XO_Form.OE == 1) {
						add_sub_decoded.alter_OV = true;
					} else {
						add_sub_decoded.alter_OV = false;
					}
					add_sub_decoded.add_CA = false;
					add_sub_decoded.sub_one = false;
					break;
				// XO Form Mul instructions
				case 9 | 0b1000000000:
				case 9: // mullhdu, mullhdu.
					// NOT SUPPORTED!!!
					break;
				case 11 | 0b1000000000:
				case 11: // mullhwu, mullhwu.
					fixed_point_decode_result.execute_mul = true;
					mul_decoded.op1_reg_address = instruction.XO_Form.RA;
					mul_decoded.op2_imm = false;
					mul_decoded.op2_immediate = 0;
					mul_decoded.op2_reg_address = instruction.XO_Form.RB;
					mul_decoded.mul_signed = true;
					mul_decoded.mul_higher = true;
					if(instruction.XO_Form.Rc == 1) {
						mul_decoded.alter_CR0 = true;
					} else {
						mul_decoded.alter_CR0 = false;
					}
					mul_decoded.alter_OV = false;
					break;
				case 73 | 0b1000000000:
				case 73: // mullhd, mullhd., mullhdo, mullhdo.
					// NOT SUPPORTED!!!
					break;
				case 75 | 0b1000000000:
				case 75: // mullhw, mullhw., mullhwo, mullhwo.
					fixed_point_decode_result.execute_mul = true;
					mul_decoded.op1_reg_address = instruction.XO_Form.RA;
					mul_decoded.op2_imm = false;
					mul_decoded.op2_immediate = 0;
					mul_decoded.op2_reg_address = instruction.XO_Form.RB;
					mul_decoded.mul_signed = true;
					mul_decoded.mul_higher = true;
					if(instruction.XO_Form.Rc == 1) {
						mul_decoded.alter_CR0 = true;
					} else {
						mul_decoded.alter_CR0 = false;
					}
					if(instruction.XO_Form.OE == 1) {
						mul_decoded.alter_OV = true;
					} else {
						mul_decoded.alter_OV = false;
					}
					break;
				case 233 | 0b1000000000:
				case 233: // mulld, mulld., mulldo, mulldo.
					// NOT SUPPORTED!!!
					break;
				case 235 | 0b1000000000:
				case 235: // mullw, mullw., mullwo, mullwo.
					fixed_point_decode_result.execute_mul = true;
					mul_decoded.op1_reg_address = instruction.XO_Form.RA;
					mul_decoded.op2_imm = false;
					mul_decoded.op2_immediate = 0;
					mul_decoded.op2_reg_address = instruction.XO_Form.RB;
					mul_decoded.mul_signed = true;
					mul_decoded.mul_higher = false;
					if(instruction.XO_Form.Rc == 1) {
						mul_decoded.alter_CR0 = true;
					} else {
						mul_decoded.alter_CR0 = false;
					}
					if(instruction.XO_Form.OE == 1) {
						mul_decoded.alter_OV = true;
					} else {
						mul_decoded.alter_OV = false;
					}
					break;
				// XO Form Div instructions
				case 457 | 0b1000000000:
				case 457: // divdu, divdu., divduo, divduo.
					// NOT SUPPORTED!!!
					break;
				case 459 | 0b1000000000:
				case 459: // divwu, divwu., divwuo, divwuo.
					fixed_point_decode_result.execute_div = true;
					div_decoded.dividend_reg_address = instruction.XO_Form.RA;
					div_decoded.divisor_reg_address = instruction.XO_Form.RB;
					div_decoded.result_reg_address = instruction.XO_Form.RT;
					div_decoded.div_signed = false;
					if(instruction.XO_Form.Rc == 1) {
						div_decoded.alter_CR0 = true;
					} else {
						div_decoded.alter_CR0 = false;
					}
					if(instruction.XO_Form.OE == 1) {
						div_decoded.alter_OV = true;
					} else {
						div_decoded.alter_OV = false;
					}
					break;
				case 489 | 0b1000000000:
				case 489: // divd, divd., divdo, divdo.
					// NOT SUPPORTED!!!
					break;
				case 491 | 0b1000000000:
				case 491: // divw, divw., divwo, divwo.
					fixed_point_decode_result.execute_div = true;
					div_decoded.dividend_reg_address = instruction.XO_Form.RA;
					div_decoded.divisor_reg_address = instruction.XO_Form.RB;
					div_decoded.result_reg_address = instruction.XO_Form.RT;
					div_decoded.div_signed = true;
					if(instruction.XO_Form.Rc == 1) {
						div_decoded.alter_CR0 = true;
					} else {
						div_decoded.alter_CR0 = false;
					}
					if(instruction.XO_Form.OE == 1) {
						div_decoded.alter_OV = true;
					} else {
						div_decoded.alter_OV = false;
					}
					break;
				// X Form Cmp instructions
				case 0: // cmp
					{
						fixed_point_decode_result.execute_compare = true;
						uint8_t L = instruction.X_Form.RT & 0b00001;
						uint8_t BF = instruction.X_Form.RT >> 2;
						if(L == 1) {
							cmp_decoded.cmp_signed = true;
						} else {
							cmp_decoded.cmp_signed = false;
						}
						cmp_decoded.op1_reg_address = instruction.X_Form.RA;
						cmp_decoded.op2_imm = false;
						cmp_decoded.op2_immediate = 0;
						cmp_decoded.op2_reg_address = instruction.X_Form.RB;
						cmp_decoded.BF = BF;
					}
					break;
				case 32: // cmpl
					{
						fixed_point_decode_result.execute_compare = true;
						uint8_t BF = instruction.D_Form.RT >> 2;
						cmp_decoded.cmp_signed = false;
						cmp_decoded.op1_reg_address = instruction.X_Form.RA;
						cmp_decoded.op2_imm = false;
						cmp_decoded.op2_immediate = 0;
						cmp_decoded.op2_reg_address = instruction.X_Form.RB;
						cmp_decoded.BF = BF;
					}
					break;
				// X Form Trap instructions
				case 68: // td
					// NOT SUPPORTED!!!
					break;
				case 4: // tw
					fixed_point_decode_result.execute_trap = true;
					trap_decoded.op1_reg_address = instruction.X_Form.RA;
					trap_decoded.op2_imm = false;
					trap_decoded.op2_immediate = 0;
					trap_decoded.op2_reg_address = instruction.X_Form.RB;
					trap_decoded.TO = instruction.X_Form.RT;
					break;
				// X Form Logical instructions
				case 26: // cntlzw, cntlzw.
					fixed_point_decode_result.execute_logical = true;
					log_decoded.operation = logical::COUNT_LEDING_ZEROS_WORD;
					log_decoded.op1_reg_address = instruction.X_Form.RT;
					log_decoded.op2_imm = true;
					log_decoded.op2_immediate = 0;
					log_decoded.op2_reg_address = 0;
					log_decoded.result_reg_address = instruction.X_Form.RA;
					if(instruction.X_Form.Rc == 1) {
						log_decoded.alter_CR0 = true;
					} else {
						log_decoded.alter_CR0 = false;
					}
					break;
				case 28: // and, and.
					fixed_point_decode_result.execute_logical = true;
					log_decoded.operation = logical::AND;
					log_decoded.op1_reg_address = instruction.X_Form.RT;
					log_decoded.op2_imm = false;
					log_decoded.op2_immediate = 0;
					log_decoded.op2_reg_address = instruction.X_Form.RB;
					log_decoded.result_reg_address = instruction.X_Form.RA;
					if(instruction.X_Form.Rc == 1) {
						log_decoded.alter_CR0 = true;
					} else {
						log_decoded.alter_CR0 = false;
					}
					break;
				case 58: // cntlzd, cntlzd.
					// NOT SUPPORTED!!!
					break;
				case 60: // andc, andc.
					fixed_point_decode_result.execute_logical = true;
					log_decoded.operation = logical::AND_COMPLEMENT;
					log_decoded.op1_reg_address = instruction.X_Form.RT;
					log_decoded.op2_imm = false;
					log_decoded.op2_immediate = 0;
					log_decoded.op2_reg_address = instruction.X_Form.RB;
					log_decoded.result_reg_address = instruction.X_Form.RA;
					if(instruction.X_Form.Rc == 1) {
						log_decoded.alter_CR0 = true;
					} else {
						log_decoded.alter_CR0 = false;
					}
					break;
				case 122: // popcntb
					fixed_point_decode_result.execute_logical = true;
					log_decoded.operation = logical::POPULATION_COUNT_BYTES;
					log_decoded.op1_reg_address = instruction.X_Form.RT;
					log_decoded.op2_imm = true;
					log_decoded.op2_immediate = 0;
					log_decoded.op2_reg_address = 0;
					log_decoded.result_reg_address = instruction.X_Form.RA;
					log_decoded.alter_CR0 = false;
					break;
				case 124: // nor, nor.
					fixed_point_decode_result.execute_logical = true;
					log_decoded.operation = logical::NOR;
					log_decoded.op1_reg_address = instruction.X_Form.RT;
					log_decoded.op2_imm = false;
					log_decoded.op2_immediate = 0;
					log_decoded.op2_reg_address = instruction.X_Form.RB;
					log_decoded.result_reg_address = instruction.X_Form.RA;
					if(instruction.X_Form.Rc == 1) {
						log_decoded.alter_CR0 = true;
					} else {
						log_decoded.alter_CR0 = false;
					}
					break;
				case 284: // eqv, eqv.
					fixed_point_decode_result.execute_logical = true;
					log_decoded.operation = logical::EQUIVALENT;
					log_decoded.op1_reg_address = instruction.X_Form.RT;
					log_decoded.op2_imm = false;
					log_decoded.op2_immediate = 0;
					log_decoded.op2_reg_address = instruction.X_Form.RB;
					log_decoded.result_reg_address = instruction.X_Form.RA;
					if(instruction.X_Form.Rc == 1) {
						log_decoded.alter_CR0 = true;
					} else {
						log_decoded.alter_CR0 = false;
					}
					break;
				case 316: // xor, xor.
					fixed_point_decode_result.execute_logical = true;
					log_decoded.operation = logical::XOR;
					log_decoded.op1_reg_address = instruction.X_Form.RT;
					log_decoded.op2_imm = false;
					log_decoded.op2_immediate = 0;
					log_decoded.op2_reg_address = instruction.X_Form.RB;
					log_decoded.result_reg_address = instruction.X_Form.RA;
					if(instruction.X_Form.Rc == 1) {
						log_decoded.alter_CR0 = true;
					} else {
						log_decoded.alter_CR0 = false;
					}
					break;
				case 412: // orc, orc.
					fixed_point_decode_result.execute_logical = true;
					log_decoded.operation = logical::OR_COMPLEMENT;
					log_decoded.op1_reg_address = instruction.X_Form.RT;
					log_decoded.op2_imm = false;
					log_decoded.op2_immediate = 0;
					log_decoded.op2_reg_address = instruction.X_Form.RB;
					log_decoded.result_reg_address = instruction.X_Form.RA;
					if(instruction.X_Form.Rc == 1) {
						log_decoded.alter_CR0 = true;
					} else {
						log_decoded.alter_CR0 = false;
					}
					break;
				case 444: // or, or.
					fixed_point_decode_result.execute_logical = true;
					log_decoded.operation = logical::OR;
					log_decoded.op1_reg_address = instruction.X_Form.RT;
					log_decoded.op2_imm = false;
					log_decoded.op2_immediate = 0;
					log_decoded.op2_reg_address = instruction.X_Form.RB;
					log_decoded.result_reg_address = instruction.X_Form.RA;
					if(instruction.X_Form.Rc == 1) {
						log_decoded.alter_CR0 = true;
					} else {
						log_decoded.alter_CR0 = false;
					}
					break;
				case 476: // nand, nand.
					fixed_point_decode_result.execute_logical = true;
					log_decoded.operation = logical::NAND;
					log_decoded.op1_reg_address = instruction.X_Form.RT;
					log_decoded.op2_imm = false;
					log_decoded.op2_immediate = 0;
					log_decoded.op2_reg_address = instruction.X_Form.RB;
					log_decoded.result_reg_address = instruction.X_Form.RA;
					if(instruction.X_Form.Rc == 1) {
						log_decoded.alter_CR0 = true;
					} else {
						log_decoded.alter_CR0 = false;
					}
					break;
				case 922: // extsh, extsh.
					fixed_point_decode_result.execute_logical = true;
					log_decoded.operation = logical::EXTEND_SIGN_HALFWORD;
					log_decoded.op1_reg_address = instruction.X_Form.RT;
					log_decoded.op2_imm = true;
					log_decoded.op2_immediate = 0;
					log_decoded.op2_reg_address = 0;
					log_decoded.result_reg_address = instruction.X_Form.RA;
					if(instruction.X_Form.Rc == 1) {
						log_decoded.alter_CR0 = true;
					} else {
						log_decoded.alter_CR0 = false;
					}
					break;
				case 954: // extsb, extsb.
					fixed_point_decode_result.execute_logical = true;
					log_decoded.operation = logical::EXTEND_SIGN_BYTE;
					log_decoded.op1_reg_address = instruction.X_Form.RT;
					log_decoded.op2_imm = true;
					log_decoded.op2_immediate = 0;
					log_decoded.op2_reg_address = 0;
					log_decoded.result_reg_address = instruction.X_Form.RA;
					if(instruction.X_Form.Rc == 1) {
						log_decoded.alter_CR0 = true;
					} else {
						log_decoded.alter_CR0 = false;
					}
					break;
				case 986: // extsw, extsw.
					// NOT SUPPORTED!!!
					break;
				// X Form shift instructions
				case 24: // slw, slw.
					fixed_point_decode_result.execute_shift = true;
					shift_decoded.shift_imm = false;
					shift_decoded.shift_immediate = 0;
					shift_decoded.shift_reg_address = instruction.X_Form.RB;
					shift_decoded.source_reg_address = instruction.X_Form.RT;
					shift_decoded.target_reg_address = instruction.X_Form.RA;
					shift_decoded.shift_left = true;
					shift_decoded.sign_extend = false;
					shift_decoded.alter_CA = false;
					if(instruction.X_Form.Rc == 1) {
						shift_decoded.alter_CR0 = true;
					} else {
						shift_decoded.alter_CR0 = false;
					}
					break;
				case 27: // sld, sld.
					// NOT SUPPORTED!!!
					break;
				case (413 << 1) | 0b1:
				case 413 << 1: // sradi, sradi.
					// NOT SUPPORTED!!!
					break;
				case 536: // srw, srw.
					fixed_point_decode_result.execute_shift = true;
					shift_decoded.shift_imm = false;
					shift_decoded.shift_immediate = 0;
					shift_decoded.shift_reg_address = instruction.X_Form.RB;
					shift_decoded.source_reg_address = instruction.X_Form.RT;
					shift_decoded.target_reg_address = instruction.X_Form.RA;
					shift_decoded.shift_left = false;
					shift_decoded.sign_extend = false;
					shift_decoded.alter_CA = false;
					if(instruction.X_Form.Rc == 1) {
						shift_decoded.alter_CR0 = true;
					} else {
						shift_decoded.alter_CR0 = false;
					}
					break;
				case 539: // srd, srd.
					// NOT SUPPORTED!!!
					break;
				case 792: // sraw, sraw.
					fixed_point_decode_result.execute_shift = true;
					shift_decoded.shift_imm = false;
					shift_decoded.shift_immediate = 0;
					shift_decoded.shift_reg_address = instruction.X_Form.RB;
					shift_decoded.source_reg_address = instruction.X_Form.RT;
					shift_decoded.target_reg_address = instruction.X_Form.RA;
					shift_decoded.shift_left = false;
					shift_decoded.sign_extend = true;
					shift_decoded.alter_CA = true;
					if(instruction.X_Form.Rc == 1) {
						shift_decoded.alter_CR0 = true;
					} else {
						shift_decoded.alter_CR0 = false;
					}
					break;
				case 794: // srad, srad.
					// NOT SUPPORTED!!!
					break;
				case 824: // srawi, srawi.
					fixed_point_decode_result.execute_shift = true;
					shift_decoded.shift_imm = true;
					shift_decoded.shift_immediate = instruction.X_Form.RB;
					shift_decoded.shift_reg_address = 0;
					shift_decoded.source_reg_address = instruction.X_Form.RT;
					shift_decoded.target_reg_address = instruction.X_Form.RA;
					shift_decoded.shift_left = false;
					shift_decoded.sign_extend = true;
					shift_decoded.alter_CA = true;
					if(instruction.X_Form.Rc == 1) {
						shift_decoded.alter_CR0 = true;
					} else {
						shift_decoded.alter_CR0 = false;
					}
					break;
				// XFX Form move system register instructions
				case 19: // mfcr
					fixed_point_decode_result.execute_system = true;
					if((instruction.XFX_Form.spr >> 9) == 1) break; // Field has to be 0
					system_decoded.operation = system_ppc::MOVE_FROM_CR;
					system_decoded.RS_RT = instruction.XFX_Form.RT;
					system_decoded.SPR = 0;
					system_decoded.FXM = 0;
					break;
				case 144: // mtcrf
					fixed_point_decode_result.execute_system = true;
					if((instruction.XFX_Form.spr >> 9) == 1) break; // Field has to be 0
					system_decoded.operation = system_ppc::MOVE_TO_CR;
					system_decoded.RS_RT = instruction.XFX_Form.RT;
					system_decoded.SPR = 0;
					system_decoded.FXM = instruction.XFX_Form.spr >> 1;
					break;
				case 339: // mfspr
					fixed_point_decode_result.execute_system = true;
					system_decoded.operation = system_ppc::MOVE_FROM_SPR;
					system_decoded.RS_RT = instruction.XFX_Form.RT;
					system_decoded.SPR = instruction.XFX_Form.spr;
					system_decoded.FXM = 0;
					break;
				case 467: // mtspr
					fixed_point_decode_result.execute_system = true;
					system_decoded.operation = system_ppc::MOVE_TO_SPR;
					system_decoded.RS_RT = instruction.XFX_Form.RT;
					system_decoded.SPR = instruction.XFX_Form.spr;
					system_decoded.FXM = 0;
					break;
				// X Form floating point Load/Store instructions
				case 535: // lfsx
					floating_point_decode_result.execute_load = true;
					decode_load_store_and_zero_x_form(float_load_store_decoded, instruction, 4)
					break;
				case 567: // lfsux
					floating_point_decode_result.execute_load = true;
					decode_load_store_and_zero_with_update_x_form(float_load_store_decoded, instruction, 4)
					break;
				case 599: // lfdx
					floating_point_decode_result.execute_load = true;
					decode_load_store_and_zero_x_form(float_load_store_decoded, instruction, 8)
					break;
				case 631: // lfdux
					floating_point_decode_result.execute_load = true;
					decode_load_store_and_zero_with_update_x_form(float_load_store_decoded, instruction, 8)
					break;
				case 663: // stfsx
					floating_point_decode_result.execute_store = true;
					decode_load_store_and_zero_x_form(float_load_store_decoded, instruction, 4)
					break;
				case 695: // stfsux
					floating_point_decode_result.execute_store = true;
					decode_load_store_and_zero_with_update_x_form(float_load_store_decoded, instruction, 4)
					break;
				case 727: // stfdx
					floating_point_decode_result.execute_store = true;
					decode_load_store_and_zero_x_form(float_load_store_decoded, instruction, 8)
					break;
				case 759: // stfdux
					floating_point_decode_result.execute_store = true;
					decode_load_store_and_zero_with_update_x_form(float_load_store_decoded, instruction, 8)
					break;
				case 983: // stfiwx
					floating_point_decode_result.execute_store = true;
					// Load decode is used here to be able to reduce the amount of macros, sign extend means that float should be stored as integer
					decode_load_algebraic_x_form(float_load_store_decoded, instruction, 4)
					break;
			}
			break;
		// D Form load instructions
		case 32: // lwz
			fixed_point_decode_result.execute_load = true;
			decode_load_store_and_zero(load_store_decoded, instruction, 4)
			break;
		case 33: // lwzu
			fixed_point_decode_result.execute_load = true;
			decode_load_store_and_zero_with_update(load_store_decoded, instruction, 4)
			break;
		case 34: // lbz
			fixed_point_decode_result.execute_load = true;
			decode_load_store_and_zero(load_store_decoded, instruction, 1)
			break;
		case 35: // lbzu
			fixed_point_decode_result.execute_load = true;
			decode_load_store_and_zero_with_update(load_store_decoded, instruction, 1)
			break;
		case 40: // lhz
			fixed_point_decode_result.execute_load = true;
			decode_load_store_and_zero(load_store_decoded, instruction, 2)
			break;
		case 41: // lhzu
			fixed_point_decode_result.execute_load = true;
			decode_load_store_and_zero_with_update(load_store_decoded, instruction, 2)
			break;
		case 42: // lha
			fixed_point_decode_result.execute_load = true;
			decode_load_algebraic(load_store_decoded, instruction, 2);
			break;
		case 43: // lhau
			fixed_point_decode_result.execute_load = true;
			decode_load_algebraic_with_update(load_store_decoded, instruction, 2)
			break;
		// D Form store instructions
		case 36: // stw
			fixed_point_decode_result.execute_store = true;
			decode_load_store_and_zero(load_store_decoded, instruction, 4)
			break;
		case 37: // stwu
			fixed_point_decode_result.execute_store = true;
			decode_load_store_and_zero_with_update(load_store_decoded, instruction, 4)
			break;
		case 38: // stb
			fixed_point_decode_result.execute_store = true;
			decode_load_store_and_zero(load_store_decoded, instruction, 1)
			break;
		case 39: // stbu
			fixed_point_decode_result.execute_store = true;
			decode_load_store_and_zero_with_update(load_store_decoded, instruction, 1)
			break;
		case 44: // sth
			fixed_point_decode_result.execute_store = true;
			decode_load_store_and_zero(load_store_decoded, instruction, 2)
			break;
		case 45: // sthu
			fixed_point_decode_result.execute_store = true;
			decode_load_store_and_zero_with_update(load_store_decoded, instruction, 2)
			break;
		case 58:
			switch(instruction.DS_Form.XO) {
				// DS Form load instructions
				case 0: // ld
					// NOT SUPPORTED!!!
					break;
				case 1: // ldu
					// NOT SUPPORTED!!!
					break;
				case 2: // lwa
					// NOT SUPPORTED!!!
					break;
			}
			break;
		case 62:
			switch(instruction.DS_Form.XO) {
				// DS Form store instructions
				case 0: // std
					// NOT SUPPORTED!!!
					break;
				case 1: // stdu
					// NOT SUPPORTED!!!
					break;
			}
			break;
		case 46: // lmw
			// Load Multiple Word not supported!!! TODO: Support
			break;
		case 47: // stmw
			// Store Multiple Word not supported!!! TODO: Support
			break;
		// D Form Add/Sub instructions
		case 8: // subfic
			fixed_point_decode_result.execute_add_sub = true;
			add_sub_decoded.subtract = true;
			add_sub_decoded.op1_imm = false;
			add_sub_decoded.op1_immediate = 0;
			add_sub_decoded.op1_reg_address = instruction.D_Form.RA;
			add_sub_decoded.op2_imm = true;
			add_sub_decoded.op2_immediate = instruction.D_Form.D;
			add_sub_decoded.op2_reg_address = 0;
			add_sub_decoded.result_reg_address = instruction.D_Form.RT;
			add_sub_decoded.alter_CA = true;
			add_sub_decoded.alter_CR0 = false;
			add_sub_decoded.alter_OV = false;
			add_sub_decoded.add_CA = false;
			add_sub_decoded.sub_one = false;
			break;
		case 12: // addic
			fixed_point_decode_result.execute_add_sub = true;
			add_sub_decoded.subtract = false;
			add_sub_decoded.op1_imm = false;
			add_sub_decoded.op1_immediate = 0;
			add_sub_decoded.op1_reg_address = instruction.D_Form.RA;
			add_sub_decoded.op2_imm = true;
			add_sub_decoded.op2_immediate = instruction.D_Form.D;
			add_sub_decoded.op2_reg_address = 0;
			add_sub_decoded.result_reg_address = instruction.D_Form.RT;
			add_sub_decoded.alter_CA = true;
			add_sub_decoded.alter_CR0 = false;
			add_sub_decoded.alter_OV = false;
			add_sub_decoded.add_CA = false;
			add_sub_decoded.sub_one = false;
			break;
		case 13: // addic.
			fixed_point_decode_result.execute_add_sub = true;
			add_sub_decoded.subtract = false;
			add_sub_decoded.op1_imm = false;
			add_sub_decoded.op1_immediate = 0;
			add_sub_decoded.op1_reg_address = instruction.D_Form.RA;
			add_sub_decoded.op2_imm = true;
			add_sub_decoded.op2_immediate = instruction.D_Form.D;
			add_sub_decoded.op2_reg_address = 0;
			add_sub_decoded.result_reg_address = instruction.D_Form.RT;
			add_sub_decoded.alter_CA = true;
			add_sub_decoded.alter_CR0 = true;
			add_sub_decoded.alter_OV = false;
			add_sub_decoded.add_CA = false;
			add_sub_decoded.sub_one = false;
			break;
		case 14: // addi
			fixed_point_decode_result.execute_add_sub = true;
			add_sub_decoded.subtract = false;
			if(instruction.D_Form.RA == 0) {
				add_sub_decoded.op1_imm = true;
				add_sub_decoded.op1_immediate = 0;
				add_sub_decoded.op1_reg_address = 0;
			} else {
				add_sub_decoded.op1_imm = false;
				add_sub_decoded.op1_immediate = 0;
				add_sub_decoded.op1_reg_address = instruction.D_Form.RA;
			}
			add_sub_decoded.op2_imm = true;
			add_sub_decoded.op2_immediate = (int16_t)instruction.D_Form.D;
			add_sub_decoded.op2_reg_address = 0;
			add_sub_decoded.result_reg_address = instruction.D_Form.RT;
			add_sub_decoded.alter_CA = false;
			add_sub_decoded.alter_CR0 = false;
			add_sub_decoded.alter_OV = false;
			add_sub_decoded.add_CA = false;
			add_sub_decoded.sub_one = false;
			break;
		case 15: // addis
			fixed_point_decode_result.execute_add_sub = true;
			add_sub_decoded.subtract = false;
			if(instruction.D_Form.RA == 0) {
				add_sub_decoded.op1_imm = true;
				add_sub_decoded.op1_immediate = 0;
				add_sub_decoded.op1_reg_address = 0;
			} else {
				add_sub_decoded.op1_imm = false;
				add_sub_decoded.op1_immediate = 0;
				add_sub_decoded.op1_reg_address = instruction.D_Form.RA;
			}
			add_sub_decoded.op2_imm = true;
			add_sub_decoded.op2_immediate = (int16_t)instruction.D_Form.D << 16;
			add_sub_decoded.op2_reg_address = 0;
			add_sub_decoded.result_reg_address = instruction.D_Form.RT;
			add_sub_decoded.alter_CA = false;
			add_sub_decoded.alter_CR0 = false;
			add_sub_decoded.alter_OV = false;
			add_sub_decoded.add_CA = false;
			add_sub_decoded.sub_one = false;
			break;
		// D Form Mul instructions
		case 7: // mulli
			fixed_point_decode_result.execute_mul = true;
			mul_decoded.op1_reg_address = instruction.D_Form.RA;
			mul_decoded.op2_imm = true;
			mul_decoded.op2_immediate = instruction.D_Form.D;
			mul_decoded.op2_reg_address = 0;
			mul_decoded.result_reg_address = instruction.D_Form.RT;
			mul_decoded.mul_signed = true;
			mul_decoded.mul_higher = false;
			mul_decoded.alter_CR0 = false;
			mul_decoded.alter_OV = false;
			break;
		// D Form Cmp instructions
		case 10: // cmpli
			{
				fixed_point_decode_result.execute_compare = true;
				uint8_t BF = instruction.D_Form.RT >> 2;
				cmp_decoded.cmp_signed = false;
				cmp_decoded.op1_reg_address = instruction.D_Form.RA;
				cmp_decoded.op2_imm = true;
				cmp_decoded.op2_immediate = (uint32_t)instruction.D_Form.D;
				cmp_decoded.op2_reg_address = 0;
				cmp_decoded.BF = BF;
			}
			break;
		case 11: // cmpi
			{
				fixed_point_decode_result.execute_compare = true;
				uint8_t L = instruction.D_Form.RT & 0b00001;
				uint8_t BF = instruction.D_Form.RT >> 2;
				if(L == 1) {
					cmp_decoded.cmp_signed = true;
				} else {
					cmp_decoded.cmp_signed = false;
				}
				cmp_decoded.op1_reg_address = instruction.D_Form.RA;
				cmp_decoded.op2_imm = true;
				cmp_decoded.op2_immediate = instruction.D_Form.D;
				cmp_decoded.op2_reg_address = 0;
				cmp_decoded.BF = BF;
			}
			break;
		// D Form Trap instructions
		case 2: // tdi
			// NOT SUPPORTED!!!
			break;
		case 3: // twi
			fixed_point_decode_result.execute_trap = true;
			trap_decoded.op1_reg_address = instruction.D_Form.RA;
			trap_decoded.op2_imm = true;
			trap_decoded.op2_immediate = instruction.D_Form.D;
			trap_decoded.op2_reg_address = 0;
			trap_decoded.TO = instruction.D_Form.RT;
			break;
		// D Form Logical instructions
		case 24: // ori
			fixed_point_decode_result.execute_logical = true;
			log_decoded.operation = logical::OR;
			log_decoded.op1_reg_address = instruction.D_Form.RT;
			log_decoded.op2_imm = true;
			log_decoded.op2_immediate = instruction.D_Form.D;
			log_decoded.op2_reg_address = 0;
			log_decoded.result_reg_address = instruction.D_Form.RA;
			log_decoded.alter_CR0 = false;
			break;
		case 25: // oris
			fixed_point_decode_result.execute_logical = true;
			log_decoded.operation = logical::OR;
			log_decoded.op1_reg_address = instruction.D_Form.RT;
			log_decoded.op2_imm = true;
			log_decoded.op2_immediate = instruction.D_Form.D << 16;
			log_decoded.op2_reg_address = 0;
			log_decoded.result_reg_address = instruction.D_Form.RA;
			log_decoded.alter_CR0 = false;
			break;
		case 26: // xori
			fixed_point_decode_result.execute_logical = true;
			log_decoded.operation = logical::XOR;
			log_decoded.op1_reg_address = instruction.D_Form.RT;
			log_decoded.op2_imm = true;
			log_decoded.op2_immediate = instruction.D_Form.D;
			log_decoded.op2_reg_address = 0;
			log_decoded.result_reg_address = instruction.D_Form.RA;
			log_decoded.alter_CR0 = false;
			break;
		case 27: // xoris
			fixed_point_decode_result.execute_logical = true;
			log_decoded.operation = logical::XOR;
			log_decoded.op1_reg_address = instruction.D_Form.RT;
			log_decoded.op2_imm = true;
			log_decoded.op2_immediate = instruction.D_Form.D << 16;
			log_decoded.op2_reg_address = 0;
			log_decoded.result_reg_address = instruction.D_Form.RA;
			log_decoded.alter_CR0 = false;
			break;
		case 28: // andi.
			fixed_point_decode_result.execute_logical = true;
			log_decoded.operation = logical::AND;
			log_decoded.op1_reg_address = instruction.D_Form.RT;
			log_decoded.op2_imm = true;
			log_decoded.op2_immediate = instruction.D_Form.D;
			log_decoded.op2_reg_address = 0;
			log_decoded.result_reg_address = instruction.D_Form.RA;
			log_decoded.alter_CR0 = true;
			break;
		case 29: // andis.
			fixed_point_decode_result.execute_logical = true;
			log_decoded.operation = logical::AND;
			log_decoded.op1_reg_address = instruction.D_Form.RT;
			log_decoded.op2_imm = true;
			log_decoded.op2_immediate = instruction.D_Form.D << 16;
			log_decoded.op2_reg_address = 0;
			log_decoded.result_reg_address = instruction.D_Form.RA;
			log_decoded.alter_CR0 = true;
			break;
		// D Form floating point Load/Store instructions
		case 48: // lfs
			floating_point_decode_result.execute_load = true;
			decode_load_store_and_zero(float_load_store_decoded, instruction, 4)
			break;
		case 49: // lfsu
			floating_point_decode_result.execute_load = true;
			decode_load_store_and_zero_with_update(float_load_store_decoded, instruction, 4)
			break;
		case 50: // lfd
			floating_point_decode_result.execute_load = true;
			decode_load_store_and_zero(float_load_store_decoded, instruction, 8)
			break;
		case 51: // lfdu
			floating_point_decode_result.execute_load = true;
			decode_load_store_and_zero_with_update(float_load_store_decoded, instruction, 8)
			break;
		case 52: // stfs
			floating_point_decode_result.execute_store = true;
			decode_load_store_and_zero(float_load_store_decoded, instruction, 4)
			break;
		case 53: // stfsu
			floating_point_decode_result.execute_store = true;
			decode_load_store_and_zero_with_update(float_load_store_decoded, instruction, 4)
			break;
		case 54: // stfd
			floating_point_decode_result.execute_store = true;
			decode_load_store_and_zero(float_load_store_decoded, instruction, 8)
			break;
		case 55: // stfdu
			floating_point_decode_result.execute_store = true;
			decode_load_store_and_zero_with_update(float_load_store_decoded, instruction, 8)
			break;
		case 63:
			switch(instruction.X_Form.XO) {
				// X Form floating point Move instructions
				case 40: // fneg, fneg.
					floating_point_decode_result.execute_move = true;
					float_move_decoded.operation = NEGATE;
					float_move_decoded.source_reg_address = instruction.X_Form.RB;
					float_move_decoded.target_reg_address = instruction.X_Form.RT;
					if(instruction.X_Form.Rc == 1) {
						float_move_decoded.alter_CR1 = true;
					} else {
						float_move_decoded.alter_CR1 = false;
					}
					break;
				case 72: // fmr, fmr.
					floating_point_decode_result.execute_move = true;
					float_move_decoded.operation = MOVE;
					float_move_decoded.source_reg_address = instruction.X_Form.RB;
					float_move_decoded.target_reg_address = instruction.X_Form.RT;
					if(instruction.X_Form.Rc == 1) {
						float_move_decoded.alter_CR1 = true;
					} else {
						float_move_decoded.alter_CR1 = false;
					}
					break;
				case 136: // fnabs, fnabs.
					floating_point_decode_result.execute_move = true;
					float_move_decoded.operation = NEGATIVE_ABSOLUTE;
					float_move_decoded.source_reg_address = instruction.X_Form.RB;
					float_move_decoded.target_reg_address = instruction.X_Form.RT;
					if(instruction.X_Form.Rc == 1) {
						float_move_decoded.alter_CR1 = true;
					} else {
						float_move_decoded.alter_CR1 = false;
					}
					break;
				case 264: // fabs, fabs.
					floating_point_decode_result.execute_move = true;
					float_move_decoded.operation = ABSOLUTE;
					float_move_decoded.source_reg_address = instruction.X_Form.RB;
					float_move_decoded.target_reg_address = instruction.X_Form.RT;
					if(instruction.X_Form.Rc == 1) {
						float_move_decoded.alter_CR1 = true;
					} else {
						float_move_decoded.alter_CR1 = false;
					}
					break;
				// X Form floating point Convert instructions
				case 12: // frsp, frsp.
					floating_point_decode_result.execute_convert = true;
					float_convert_decoded.source_reg_address = instruction.X_Form.RB;
					float_convert_decoded.target_reg_address = instruction.X_Form.RT;
					float_convert_decoded.round_to_single = true;
					float_convert_decoded.convert_to_integer = false;
					float_convert_decoded.round_toward_zero = false;
					if(instruction.X_Form.Rc == 1) {
						float_convert_decoded.alter_CR1 = true;
					} else {
						float_convert_decoded.alter_CR1 = false;
					}
					break;
				case 14: // fctiw, fctiw.
					floating_point_decode_result.execute_convert = true;
					float_convert_decoded.source_reg_address = instruction.X_Form.RB;
					float_convert_decoded.target_reg_address = instruction.X_Form.RT;
					float_convert_decoded.round_to_single = false;
					float_convert_decoded.convert_to_integer = true;
					float_convert_decoded.round_toward_zero = false;
					if(instruction.X_Form.Rc == 1) {
						float_convert_decoded.alter_CR1 = true;
					} else {
						float_convert_decoded.alter_CR1 = false;
					}
					break;
				case 15: // fctiwz, fctiwz.
					floating_point_decode_result.execute_convert = true;
					float_convert_decoded.source_reg_address = instruction.X_Form.RB;
					float_convert_decoded.target_reg_address = instruction.X_Form.RT;
					float_convert_decoded.round_to_single = false;
					float_convert_decoded.convert_to_integer = true;
					float_convert_decoded.round_toward_zero = true;
					if(instruction.X_Form.Rc == 1) {
						float_convert_decoded.alter_CR1 = true;
					} else {
						float_convert_decoded.alter_CR1 = false;
					}
					break;
				case 814: // fctid, fctid.
					// NOT SUPPORTED!!!
					break;
				case 815: // fctidz, fctidz.
					// NOT SUPPORTED!!!
					break;
				case 846: // fcfid, fcfid.
					// NOT SUPPORTED!!!
					break;
				// X Form floating point compare instructions
				case 0: // fcmpu
					floating_point_decode_result.execute_compare = true;
					float_compare_decoded.FRA = instruction.X_Form.RA;
					float_compare_decoded.FRA = instruction.X_Form.RB;
					float_compare_decoded.BF = instruction.X_Form.RT >> 2;
					float_compare_decoded.unordered = true;
					break;
				case 63: // fcmpo
					floating_point_decode_result.execute_compare = true;
					float_compare_decoded.FRA = instruction.X_Form.RA;
					float_compare_decoded.FRA = instruction.X_Form.RB;
					float_compare_decoded.BF = instruction.X_Form.RT >> 2;
					float_compare_decoded.unordered = false;
					break;
				// X Form floating point status instructions
				case 38: // mtfsb1, mtfsb1.
					floating_point_decode_result.execute_status = true;
					float_status_decoded.FRT_FRB = 0;
					float_status_decoded.BF_BT = instruction.X_Form.RT;
					float_status_decoded.BFA = 0;
					float_status_decoded.U = 0;
					float_status_decoded.FLM = 0;
					float_status_decoded.move_to_FPR = false;
					float_status_decoded.move_to_CR = false;
					float_status_decoded.move_to_FPSCR = true;
					float_status_decoded.use_U = false;
					float_status_decoded.bit_0 = false;
					float_status_decoded.bit_1 = true;
					if(instruction.X_Form.Rc == 1) {
						float_status_decoded.alter_CR1 = true;
					} else {
						float_status_decoded.alter_CR1 = false;
					}
					break;
				case 64: // mcrfs
					floating_point_decode_result.execute_status = true;
					float_status_decoded.FRT_FRB = 0;
					float_status_decoded.BF_BT = instruction.X_Form.RT >> 2;
					float_status_decoded.BFA = instruction.X_Form.RA >> 2;
					float_status_decoded.U = 0;
					float_status_decoded.FLM = 0;
					float_status_decoded.move_to_FPR = false;
					float_status_decoded.move_to_CR = true;
					float_status_decoded.move_to_FPSCR = false;
					float_status_decoded.use_U = false;
					float_status_decoded.bit_0 = false;
					float_status_decoded.bit_1 = false;
					float_status_decoded.alter_CR1 = false;
					break;
				case 70: // mtfsb0, mtfsb0.
					floating_point_decode_result.execute_status = true;
					float_status_decoded.FRT_FRB = 0;
					float_status_decoded.BF_BT = instruction.X_Form.RT;
					float_status_decoded.BFA = 0;
					float_status_decoded.U = 0;
					float_status_decoded.FLM = 0;
					float_status_decoded.move_to_FPR = false;
					float_status_decoded.move_to_CR = false;
					float_status_decoded.move_to_FPSCR = true;
					float_status_decoded.use_U = false;
					float_status_decoded.bit_0 = true;
					float_status_decoded.bit_1 = false;
					if(instruction.X_Form.Rc == 1) {
						float_status_decoded.alter_CR1 = true;
					} else {
						float_status_decoded.alter_CR1 = false;
					}
					break;
				case 134: // mtfsfi, mtfsfi.
					floating_point_decode_result.execute_status = true;
					float_status_decoded.FRT_FRB = 0;
					float_status_decoded.BF_BT = instruction.X_Form.RT >> 2;
					float_status_decoded.BFA = 0;
					float_status_decoded.U = instruction.X_Form.RB >> 1;
					float_status_decoded.FLM = 0;
					float_status_decoded.move_to_FPR = false;
					float_status_decoded.move_to_CR = false;
					float_status_decoded.move_to_FPSCR = true;
					float_status_decoded.use_U = true;
					float_status_decoded.bit_0 = false;
					float_status_decoded.bit_1 = false;
					if(instruction.X_Form.Rc == 1) {
						float_status_decoded.alter_CR1 = true;
					} else {
						float_status_decoded.alter_CR1 = false;
					}
					break;
				case 583: // mffs, mffs.
					floating_point_decode_result.execute_status = true;
					float_status_decoded.FRT_FRB = instruction.X_Form.RT;
					float_status_decoded.BF_BT = 0;
					float_status_decoded.BFA = 0;
					float_status_decoded.U = 0;
					float_status_decoded.FLM = 0;
					float_status_decoded.move_to_FPR = true;
					float_status_decoded.move_to_CR = false;
					float_status_decoded.move_to_FPSCR = false;
					float_status_decoded.use_U = false;
					float_status_decoded.bit_0 = false;
					float_status_decoded.bit_1 = false;
					if(instruction.X_Form.Rc == 1) {
						float_status_decoded.alter_CR1 = true;
					} else {
						float_status_decoded.alter_CR1 = false;
					}
					break;
				// XFL Form floating point status instructions
				case 711: // mtfsf, mtfsf.
					floating_point_decode_result.execute_status = true;
					float_status_decoded.FRT_FRB = instruction.XFL_Form.FRB;
					float_status_decoded.BF_BT = 0;
					float_status_decoded.BFA = 0;
					float_status_decoded.U = 0;
					float_status_decoded.FLM = instruction.XFL_Form.FLM;
					float_status_decoded.move_to_FPR = false;
					float_status_decoded.move_to_CR = false;
					float_status_decoded.move_to_FPSCR = true;
					float_status_decoded.use_U = false;
					float_status_decoded.bit_0 = false;
					float_status_decoded.bit_1 = false;
					if(instruction.XFL_Form.Rc == 1) {
						float_status_decoded.alter_CR1 = true;
					} else {
						float_status_decoded.alter_CR1 = false;
					}
					break;
			}

			switch(instruction.A_Form.XO) {
				// A Form floating point Arithmetic instructions
				case 18: // fdiv, fdiv.
					floating_point_decode_result.execute_arithmetic = true;
					float_arithmetic_decoded.operation = floating_point::DIVIDE;
					float_arithmetic_decoded.op1_reg_address = instruction.A_Form.FRA;
					float_arithmetic_decoded.op2_reg_address = instruction.A_Form.FRB;
					float_arithmetic_decoded.result_reg_address = instruction.A_Form.FRT;
					float_arithmetic_decoded.single_precision = false;
					if(instruction.A_Form.Rc == 1) {
						float_arithmetic_decoded.alter_CR1 = true;
					} else {
						float_arithmetic_decoded.alter_CR1 = false;
					}
					break;
				case 20: // fsub, fsub.
					floating_point_decode_result.execute_arithmetic = true;
					float_arithmetic_decoded.operation = floating_point::SUBTRACT;
					float_arithmetic_decoded.op1_reg_address = instruction.A_Form.FRA;
					float_arithmetic_decoded.op2_reg_address = instruction.A_Form.FRB;
					float_arithmetic_decoded.result_reg_address = instruction.A_Form.FRT;
					float_arithmetic_decoded.single_precision = false;
					if(instruction.A_Form.Rc == 1) {
						float_arithmetic_decoded.alter_CR1 = true;
					} else {
						float_arithmetic_decoded.alter_CR1 = false;
					}
					break;
				case 21: // fadd, fadd.
					floating_point_decode_result.execute_arithmetic = true;
					float_arithmetic_decoded.operation = floating_point::ADD;
					float_arithmetic_decoded.op1_reg_address = instruction.A_Form.FRA;
					float_arithmetic_decoded.op2_reg_address = instruction.A_Form.FRB;
					float_arithmetic_decoded.result_reg_address = instruction.A_Form.FRT;
					float_arithmetic_decoded.single_precision = false;
					if(instruction.A_Form.Rc == 1) {
						float_arithmetic_decoded.alter_CR1 = true;
					} else {
						float_arithmetic_decoded.alter_CR1 = false;
					}
					break;
				case 25: // fmul, fmul.
					floating_point_decode_result.execute_arithmetic = true;
					float_arithmetic_decoded.operation = floating_point::MULTIPLY;
					float_arithmetic_decoded.op1_reg_address = instruction.A_Form.FRA;
					float_arithmetic_decoded.op2_reg_address = instruction.A_Form.FRB;
					float_arithmetic_decoded.result_reg_address = instruction.A_Form.FRT;
					float_arithmetic_decoded.single_precision = false;
					if(instruction.A_Form.Rc == 1) {
						float_arithmetic_decoded.alter_CR1 = true;
					} else {
						float_arithmetic_decoded.alter_CR1 = false;
					}
					break;

				// A Form floating point Madd instructions
				case 28: // fmsub, fmsub.
					floating_point_decode_result.execute_madd = true;
					float_madd_decoded.mul1_reg_address = instruction.A_Form.FRA;
					float_madd_decoded.mul2_reg_address = instruction.A_Form.FRC;
					float_madd_decoded.add_reg_address = instruction.A_Form.FRB;
					float_madd_decoded.single_precision = false;
					float_madd_decoded.negate_add = true;
					float_madd_decoded.negate_result = false;
					if(instruction.A_Form.Rc == 1) {
						float_madd_decoded.alter_CR1 = true;
					} else {
						float_madd_decoded.alter_CR1 = false;
					}
					break;
				case 29: // fmadd, fmadd.
					floating_point_decode_result.execute_madd = true;
					float_madd_decoded.mul1_reg_address = instruction.A_Form.FRA;
					float_madd_decoded.mul2_reg_address = instruction.A_Form.FRC;
					float_madd_decoded.add_reg_address = instruction.A_Form.FRB;
					float_madd_decoded.single_precision = false;
					float_madd_decoded.negate_add = false;
					float_madd_decoded.negate_result = false;
					if(instruction.A_Form.Rc == 1) {
						float_madd_decoded.alter_CR1 = true;
					} else {
						float_madd_decoded.alter_CR1 = false;
					}
					break;
				case 30: // fnmsub, fnmsub.
					floating_point_decode_result.execute_madd = true;
					float_madd_decoded.mul1_reg_address = instruction.A_Form.FRA;
					float_madd_decoded.mul2_reg_address = instruction.A_Form.FRC;
					float_madd_decoded.add_reg_address = instruction.A_Form.FRB;
					float_madd_decoded.single_precision = false;
					float_madd_decoded.negate_add = true;
					float_madd_decoded.negate_result = true;
					if(instruction.A_Form.Rc == 1) {
						float_madd_decoded.alter_CR1 = true;
					} else {
						float_madd_decoded.alter_CR1 = false;
					}
					break;
				case 31: // fnmadd, fnmadd.
					floating_point_decode_result.execute_madd = true;
					float_madd_decoded.mul1_reg_address = instruction.A_Form.FRA;
					float_madd_decoded.mul2_reg_address = instruction.A_Form.FRC;
					float_madd_decoded.add_reg_address = instruction.A_Form.FRB;
					float_madd_decoded.single_precision = false;
					float_madd_decoded.negate_add = false;
					float_madd_decoded.negate_result = true;
					if(instruction.A_Form.Rc == 1) {
						float_madd_decoded.alter_CR1 = true;
					} else {
						float_madd_decoded.alter_CR1 = false;
					}
					break;
			}
			break;

			case 59:
				switch(instruction.A_Form.XO) {
					// A Form floating point Arithmetic instructions
					case 18: // fdivs, fdivs.
						floating_point_decode_result.execute_arithmetic = true;
						float_arithmetic_decoded.operation = floating_point::DIVIDE;
						float_arithmetic_decoded.op1_reg_address = instruction.A_Form.FRA;
						float_arithmetic_decoded.op2_reg_address = instruction.A_Form.FRB;
						float_arithmetic_decoded.result_reg_address = instruction.A_Form.FRT;
						float_arithmetic_decoded.single_precision = true;
						if(instruction.A_Form.Rc == 1) {
							float_arithmetic_decoded.alter_CR1 = true;
						} else {
							float_arithmetic_decoded.alter_CR1 = false;
						}
						break;
					case 20: // fsubs, fsubs.
						floating_point_decode_result.execute_arithmetic = true;
						float_arithmetic_decoded.operation = floating_point::SUBTRACT;
						float_arithmetic_decoded.op1_reg_address = instruction.A_Form.FRA;
						float_arithmetic_decoded.op2_reg_address = instruction.A_Form.FRB;
						float_arithmetic_decoded.result_reg_address = instruction.A_Form.FRT;
						float_arithmetic_decoded.single_precision = true;
						if(instruction.A_Form.Rc == 1) {
							float_arithmetic_decoded.alter_CR1 = true;
						} else {
							float_arithmetic_decoded.alter_CR1 = false;
						}
						break;
					case 21: // fadds, fadds.
						floating_point_decode_result.execute_arithmetic = true;
						float_arithmetic_decoded.operation = floating_point::ADD;
						float_arithmetic_decoded.op1_reg_address = instruction.A_Form.FRA;
						float_arithmetic_decoded.op2_reg_address = instruction.A_Form.FRB;
						float_arithmetic_decoded.result_reg_address = instruction.A_Form.FRT;
						float_arithmetic_decoded.single_precision = true;
						if(instruction.A_Form.Rc == 1) {
							float_arithmetic_decoded.alter_CR1 = true;
						} else {
							float_arithmetic_decoded.alter_CR1 = false;
						}
						break;
					case 25: // fmuls, fmuls.
						floating_point_decode_result.execute_arithmetic = true;
						float_arithmetic_decoded.operation = floating_point::MULTIPLY;
						float_arithmetic_decoded.op1_reg_address = instruction.A_Form.FRA;
						float_arithmetic_decoded.op2_reg_address = instruction.A_Form.FRB;
						float_arithmetic_decoded.result_reg_address = instruction.A_Form.FRT;
						float_arithmetic_decoded.single_precision = true;
						if(instruction.A_Form.Rc == 1) {
							float_arithmetic_decoded.alter_CR1 = true;
						} else {
							float_arithmetic_decoded.alter_CR1 = false;
						}
						break;
					// A Form floating point Madd instructions
					case 28: // fmsubs, fmsubs.
						floating_point_decode_result.execute_madd = true;
						float_madd_decoded.mul1_reg_address = instruction.A_Form.FRA;
						float_madd_decoded.mul2_reg_address = instruction.A_Form.FRC;
						float_madd_decoded.add_reg_address = instruction.A_Form.FRB;
						float_madd_decoded.single_precision = true;
						float_madd_decoded.negate_add = true;
						float_madd_decoded.negate_result = false;
						if(instruction.A_Form.Rc == 1) {
							float_madd_decoded.alter_CR1 = true;
						} else {
							float_madd_decoded.alter_CR1 = false;
						}
						break;
					case 29: // fmadds, fmadds.
						floating_point_decode_result.execute_madd = true;
						float_madd_decoded.mul1_reg_address = instruction.A_Form.FRA;
						float_madd_decoded.mul2_reg_address = instruction.A_Form.FRC;
						float_madd_decoded.add_reg_address = instruction.A_Form.FRB;
						float_madd_decoded.single_precision = true;
						float_madd_decoded.negate_add = false;
						float_madd_decoded.negate_result = false;
						if(instruction.A_Form.Rc == 1) {
							float_madd_decoded.alter_CR1 = true;
						} else {
							float_madd_decoded.alter_CR1 = false;
						}
						break;
					case 30: // fnmsubs, fnmsubs.
						floating_point_decode_result.execute_madd = true;
						float_madd_decoded.mul1_reg_address = instruction.A_Form.FRA;
						float_madd_decoded.mul2_reg_address = instruction.A_Form.FRC;
						float_madd_decoded.add_reg_address = instruction.A_Form.FRB;
						float_madd_decoded.single_precision = true;
						float_madd_decoded.negate_add = true;
						float_madd_decoded.negate_result = true;
						if(instruction.A_Form.Rc == 1) {
							float_madd_decoded.alter_CR1 = true;
						} else {
							float_madd_decoded.alter_CR1 = false;
						}
						break;
					case 31: // fnmadds, fnmadds.
						floating_point_decode_result.execute_madd = true;
						float_madd_decoded.mul1_reg_address = instruction.A_Form.FRA;
						float_madd_decoded.mul2_reg_address = instruction.A_Form.FRC;
						float_madd_decoded.add_reg_address = instruction.A_Form.FRB;
						float_madd_decoded.single_precision = true;
						float_madd_decoded.negate_add = false;
						float_madd_decoded.negate_result = true;
						if(instruction.A_Form.Rc == 1) {
							float_madd_decoded.alter_CR1 = true;
						} else {
							float_madd_decoded.alter_CR1 = false;
						}
						break;
				}
				break;

	}

	branch_result.branch_decoded = branch_decoded;
	branch_result.system_call_decoded = system_call_decoded;
	branch_result.condition_decoded = condition_decoded;

	fixed_point_decode_result.load_store_decoded = load_store_decoded;
	fixed_point_decode_result.add_sub_decoded = add_sub_decoded;
	fixed_point_decode_result.mul_decoded = mul_decoded;
	fixed_point_decode_result.div_decoded = div_decoded;
	fixed_point_decode_result.cmp_decoded = cmp_decoded;
	fixed_point_decode_result.trap_decoded = trap_decoded;
	fixed_point_decode_result.log_decoded = log_decoded;
	fixed_point_decode_result.rotate_decoded = rotate_decoded;
	fixed_point_decode_result.shift_decoded = shift_decoded;
	fixed_point_decode_result.system_decoded = system_decoded;

	floating_point_decode_result.float_load_store_decoded = float_load_store_decoded;
	floating_point_decode_result.float_move_decoded = float_move_decoded;
	floating_point_decode_result.float_arithmetic_decoded = float_arithmetic_decoded;
	floating_point_decode_result.float_madd_decoded = float_madd_decoded;
	floating_point_decode_result.float_convert_decoded = float_convert_decoded;
	floating_point_decode_result.float_compare_decoded = float_compare_decoded;
	floating_point_decode_result.float_status_decoded = float_status_decoded;

	decode_result.branch_decode_result = branch_result;
	decode_result.fixed_point_decode_result = fixed_point_decode_result;
	decode_result.floating_point_decode_result = floating_point_decode_result;

	return decode_result;
}
