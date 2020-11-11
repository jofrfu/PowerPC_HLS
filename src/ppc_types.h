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
#include <ap_int.h>
#include "registers.hpp"

typedef struct {
	ap_uint<32> instruction_bits;
	void operator=(ap_uint<32> val) {
	    instruction_bits = val;
	    I_Form = val;
	    B_Form = val;
	    SC_Form = val;
	    D_Form = val;
	    DS_Form = val;
	    X_Form = val;
	    XL_Form = val;
	    XFX_Form = val;
	    XFL_Form = val;
	    XS_Form = val;
	    XO_Form = val;
	    A_Form = val;
	    M_Form = val;
	    MD_Form = val;
	    MDS_Form = val;
	}

	struct {
		ap_uint<1> LK;
		ap_uint<1> AA;
		ap_uint<24> LI;
		ap_uint<6> OPCD;
		void operator=(ap_uint<32> &val) {
		    LK = val[0];
		    AA = val[1];
		    LI = val(25, 2);
		    OPCD = val(31, 26);
		}
	} I_Form;

	struct {
		ap_uint<1> LK;
        ap_uint<1> AA;
        ap_uint<14> BD;
        ap_uint<5> BI;
        ap_uint<5> BO;
        ap_uint<6> OPCD;
        void operator=(ap_uint<32> &val) {
            LK = val[0];
            AA = val[1];
            BD = val(15, 2);
            BI = val(20, 16);
            BO = val(25, 21);
            OPCD = val(31, 26);
        }
	} B_Form;

	struct {
		ap_uint<1> UNUSED_5;
		ap_uint<1> ALWAYS_ONE;
		ap_uint<3> UNUSED_4;
		ap_uint<7> LEV;
		ap_uint<4> UNUSED_3;
		ap_uint<5> UNUSED_2;
		ap_uint<5> UNUSED_1;
		ap_uint<6> OPCD;
        void operator=(ap_uint<32> &val) {
            UNUSED_5 = val[0];
            ALWAYS_ONE = val[1];
            UNUSED_4 = val(4, 2);
            LEV = val(11, 5);
            UNUSED_3 = val(15, 12);
            UNUSED_2 = val(20, 16);
            UNUSED_1 = val(25, 21);
            OPCD = val(31, 26);
        }
	} SC_Form;

	struct {
		ap_uint<16> D;
		ap_uint<5> RA;
		ap_uint<5> RT;
		ap_uint<6> OPCD;
        void operator=(ap_uint<32> &val) {
            D = val(15, 0);
            RA = val(20, 16);
            RT = val(25, 21);
            OPCD = val(31, 26);
        }
	} D_Form;

	struct {
		ap_uint<2> XO;
		ap_uint<14> DS;
		ap_uint<5> RA;
		ap_uint<5> RT;
		ap_uint<6> OPCD;
        void operator=(ap_uint<32> &val) {
            XO = val(1, 0);
            DS = val(15, 2);
            RA = val(20, 16);
            RT = val(25, 21);
            OPCD = val(31, 26);
        }
	} DS_Form;

	struct {
		ap_uint<1> Rc;
		ap_uint<10> XO;
		ap_uint<5> RB;
		ap_uint<5> RA;
		ap_uint<5> RT;
		ap_uint<6> OPCD;
        void operator=(ap_uint<32> &val) {
            Rc = val[0];
            XO = val(10, 1);
            RB = val(15, 11);
            RA = val(20, 16);
            RT = val(25, 21);
            OPCD = val(31, 26);
        }
	} X_Form;

	struct {
		ap_uint<1> LK;
		ap_uint<10> XO;
		ap_uint<5> BB;
		ap_uint<5> BA;
		ap_uint<5> BT;
		ap_uint<6> OPCD;
        void operator=(ap_uint<32> &val) {
            LK = val[0];
            XO = val(10, 1);
            BB = val(15, 11);
            BA = val(20, 16);
            BT = val(25, 21);
            OPCD = val(31, 26);
        }
	} XL_Form;

	struct {
		ap_uint<1> UNUSED_1;
		ap_uint<10> XO;
		ap_uint<10> spr;
		ap_uint<5> RT;
		ap_uint<6> OPCD;
        void operator=(ap_uint<32> &val) {
            UNUSED_1 = val[0];
            XO = val(10, 1);
            spr = val(20, 11);
            RT = val(25, 21);
            OPCD = val(31, 26);
        }
	} XFX_Form;

	struct {
		ap_uint<1> Rc;
		ap_uint<10> XO;
		ap_uint<5> FRB;
		ap_uint<1> UNUSED_2;
		ap_uint<8> FLM;
		ap_uint<1> UNUSED_1;
		ap_uint<6> OPCD;
        void operator=(ap_uint<32> &val) {
            Rc = val[0];
            XO = val(10, 1);
            FRB = val(15, 11);
            UNUSED_2 = val[16];
            FLM = val(24, 17);
            UNUSED_1 = val[25];
            OPCD = val(31, 26);
        }
	} XFL_Form;

	struct {
		ap_uint<1> Rc;
		ap_uint<1> SH_2;
		ap_uint<9> XO;
		ap_uint<5> SH_1;
		ap_uint<5> RA;
		ap_uint<5> RS;
		ap_uint<6> OPCD;
        void operator=(ap_uint<32> &val) {
            Rc = val[0];
            SH_2 = val[1];
            XO = val(10, 2);
            SH_1 = val(15, 11);
            RA = val(20, 16);
            RS = val(25, 21);
            OPCD = val(31, 26);
        }
	} XS_Form;

	struct {
		ap_uint<1> Rc;
		ap_uint<9> XO;
		ap_uint<1> OE;
		ap_uint<5> RB;
		ap_uint<5> RA;
		ap_uint<5> RT;
		ap_uint<6> OPCD;
        void operator=(ap_uint<32> &val) {
            Rc = val[0];
            XO = val(9, 1);
            OE = val[10];
            RB = val(15, 11);
            RA = val(20, 16);
            RT = val(25, 21);
            OPCD = val(31, 26);
        }
	} XO_Form;

	struct {
		ap_uint<1> Rc;
		ap_uint<5> XO;
		ap_uint<5> FRC;
		ap_uint<5> FRB;
		ap_uint<5> FRA;
		ap_uint<5> FRT;
		ap_uint<6> OPCD;
        void operator=(ap_uint<32> &val) {
            Rc = val[0];
            XO = val(5, 1);
            FRC = val(10, 6);
            FRB = val(15, 11);
            FRA = val(20, 16);
            FRT = val(25, 21);
            OPCD = val(31, 26);
        }
	} A_Form;

	struct {
		ap_uint<1> Rc;
		ap_uint<5> ME;
		ap_uint<5> MB;
        ap_uint<5> RB;
        ap_uint<5> RA;
        ap_uint<5> RS;
        ap_uint<6> OPCD;
        void operator=(ap_uint<32> &val) {
            Rc = val[0];
            ME = val(5, 1);
            MB = val(10, 6);
            RB = val(15, 11);
            RA = val(20, 16);
            RS = val(25, 21);
            OPCD = val(31, 26);
        }
	} M_Form;

	struct {
		ap_uint<1> Rc;
		ap_uint<1> SH_2;
		ap_uint<3> XO;
		ap_uint<6> MB;
        ap_uint<5> SH_1;
        ap_uint<5> RA;
        ap_uint<5> RS;
        ap_uint<6> OPCD;
        void operator=(ap_uint<32> &val) {
            Rc = val[0];
            SH_2 = val[1];
            XO = val(4, 2);
            MB = val(10, 5);
            SH_1 = val(15, 11);
            RA = val(20, 16);
            RS = val(25, 21);
            OPCD = val(31, 26);
        }
	} MD_Form;

	struct {
		ap_uint<1> Rc;
		ap_uint<4> XO;
		ap_uint<6> MB;
		ap_uint<5> RB;
		ap_uint<5> RA;
		ap_uint<5> RS;
		ap_uint<6> OPCD;
        void operator=(ap_uint<32> &val) {
            Rc = val[0];
            XO = val(4, 1);
            MB = val(10, 5);
            RB = val(15, 11);
            RA = val(20, 16);
            RS = val(25, 21);
            OPCD = val(31, 26);
        }
	} MDS_Form;
} instruction_t;

// Types for branch processor
typedef enum {BRANCH, BRANCH_CONDITIONAL, BRANCH_CONDITIONAL_LINK, BRANCH_CONDITIONAL_COUNT} branch_op_t;

typedef struct {
	branch_op_t operation;
	ap_uint<1> LK;
	ap_uint<1> AA;
	ap_uint<24> LI;
	ap_uint<14> BD;
	ap_uint<5> BI;
	ap_uint<5> BO;
	ap_uint<2> BH;
} branch_decode_t;

typedef uint8_t system_call_decode_t;

namespace condition {
	typedef enum {AND, OR, XOR, NAND, NOR, EQUIVALENT, AND_COMPLEMENT, OR_COMPLEMENT, MOVE} condition_op_t;
}

typedef struct {
	condition::condition_op_t operation;
	ap_uint<5> CR_op1_reg_address;
	ap_uint<5> CR_op2_reg_address;
	ap_uint<5> CR_result_reg_address;
} condition_decode_t;

namespace branch {
    typedef enum {
        NONE, BRANCH, SYSTEM_CALL, CONDITION
    } execute_t;
}

typedef struct {
	branch::execute_t execute;
	branch_decode_t branch_decoded;
	system_call_decode_t system_call_decoded;
	condition_decode_t condition_decoded;
} branch_decode_result_t;

// Types for fixed point processor
typedef struct {
	ap_uint<2> word_size; // in bytes-1
	bool sum1_imm; // Use immediate for first summand
	ap_int<16> sum1_immediate;
	ap_uint<5> sum1_reg_address;
	bool sum2_imm; // Use immediate for second summand
	ap_int<16> sum2_immediate;
	ap_uint<5> sum2_reg_address;
	bool write_ea; // write effective address
	ap_uint<5> ea_reg_address;
	ap_uint<5> result_reg_address;
	bool sign_extend;
	bool little_endian;
	bool multiple;
} load_store_decode_t;

typedef struct {
	bool subtract;
	bool op1_imm; // Use immediate for first operand
	ap_int<32> op1_immediate;
	ap_uint<5> op1_reg_address;
	bool op2_imm; // Use immediate for first operand
	ap_int<32> op2_immediate;
	ap_uint<5> op2_reg_address;
	ap_uint<5> result_reg_address;
	bool alter_CA;
	bool alter_CR0;
	bool alter_OV;
	bool add_CA;
} add_sub_decode_t;

typedef struct {
	ap_uint<5> op1_reg_address;
	bool op2_imm; // Use immediate for first operand
	ap_int<32> op2_immediate;
	ap_uint<5> op2_reg_address;
	ap_uint<5> result_reg_address;
	bool mul_signed;
	bool mul_higher;
	bool alter_CR0;
	bool alter_OV;
} mul_decode_t;

typedef struct {
    ap_uint<5> dividend_reg_address;
    ap_uint<5> divisor_reg_address;
    ap_uint<5> result_reg_address;
	bool div_signed;
	bool alter_CR0;
	bool alter_OV;
} div_decode_t;

typedef struct {
    ap_uint<5> op1_reg_address;
	bool op2_imm; // Use immediate for second operand
	ap_int<32> op2_immediate;
    ap_uint<5> op2_reg_address;
	bool cmp_signed;
	ap_uint<3> BF;
} cmp_decode_t;

typedef struct {
    ap_uint<5> op1_reg_address;
	bool op2_imm; // Use immediate for second operand
	ap_int<32> op2_immediate;
    ap_uint<5> op2_reg_address;
    ap_uint<5> TO;
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
    ap_uint<5> op1_reg_address;
	bool op2_imm; // Use immediate for second operand
	ap_uint<32> op2_immediate;
    ap_uint<5> op2_reg_address;
    ap_uint<5> result_reg_address;
	bool alter_CR0;
} log_decode_t;

typedef struct {
	bool shift_imm;
    ap_uint<5> shift_immediate;
    ap_uint<5> shift_reg_address;
    ap_uint<5> source_reg_address;
    ap_uint<5> target_reg_address;
    ap_uint<5> MB;
    ap_uint<5> ME;
	bool mask_insert;
	// Shift specific variables
	bool shift;
    bool left;
    bool sign_extend;

	bool alter_CR0;
} rotate_decode_t;

namespace system_ppc {
	typedef enum {MOVE_TO_SPR, MOVE_FROM_SPR, MOVE_TO_CR, MOVE_FROM_CR} system_op_t;
}

typedef struct {
	system_ppc::system_op_t operation;
    ap_uint<5> RS_RT; // RS or RT reg address
    ap_uint<10> SPR; // Special purpose register address
    ap_uint<8> FXM; // Field mask
} system_decode_t;

namespace fixed_point {
    typedef enum {
        NONE, LOAD, STORE, LOAD_STRING, STORE_STRING, ADD_SUB, MUL, DIV, COMPARE, TRAP, LOGICAL, ROTATE, /*SHIFT,*/ SYSTEM
    } execute_t;
}

typedef struct {
    fixed_point::execute_t execute;
	load_store_decode_t load_store_decoded;
	add_sub_decode_t add_sub_decoded;
	mul_decode_t mul_decoded;
	div_decode_t div_decoded;
	cmp_decode_t cmp_decoded;
	trap_decode_t trap_decoded;
	log_decode_t log_decoded;
	rotate_decode_t rotate_decoded;
	system_decode_t system_decoded;
} fixed_point_decode_result_t;

// Types for floating point processor
typedef struct {
    ap_uint<3> word_size; // in bytes-1
	bool sum1_imm; // Use immediate for first summand
    ap_int<16> sum1_immediate;
    ap_uint<5> sum1_reg_address;
	bool sum2_imm; // Use immediate for second summand
    ap_int<16> sum2_immediate;
    ap_uint<5> sum2_reg_address;
	bool write_ea; // write effective address
    ap_uint<5> ea_reg_address;
    ap_uint<5> result_reg_address;
	bool sign_extend; // Sign extend means, store as integer word for floating point
	bool little_endian; // Unused for floating point
	bool multiple; // Unused for floating point
} float_load_store_decode_t;

typedef enum {MOVE, NEGATE, ABSOLUTE, NEGATIVE_ABSOLUTE} float_move_op_t;

typedef struct {
	float_move_op_t operation;
    ap_uint<5> source_reg_address;
    ap_uint<5> target_reg_address;
	bool alter_CR1;
} float_move_decode_t;

namespace floating_point {
	typedef enum {ADD, SUBTRACT, MULTIPLY, DIVIDE} float_arithmetic_op_t;
}

typedef struct {
	floating_point::float_arithmetic_op_t operation;
    ap_uint<5> op1_reg_address;
    ap_uint<5> op2_reg_address;
    ap_uint<5> result_reg_address;
	bool single_precision;
	bool alter_CR1;
} float_arithmetic_decode_t;

typedef struct {
    ap_uint<5> mul1_reg_address;
    ap_uint<5> mul2_reg_address;
    ap_uint<5> add_reg_address;
    ap_uint<5> result_reg_address;
	bool single_precision;
	bool negate_add;
	bool negate_result;
	bool alter_CR1;
} float_madd_decode_t;

typedef struct {
    ap_uint<5> source_reg_address;
    ap_uint<5> target_reg_address;
	bool round_to_single;
	bool convert_to_integer;
	bool round_toward_zero;
	bool alter_CR1;
} float_convert_decode_t;

typedef struct {
    ap_uint<5> FRA;
    ap_uint<5> FRB;
    ap_uint<3> BF;
	bool unordered;
} float_compare_decode_t;

typedef struct {
    ap_uint<5> FRT_FRB; // FRT or FRB
    ap_uint<5> BF_BT; // BF or BT
    ap_uint<3> BFA;
    ap_uint<4> U;
    ap_uint<8> FLM;
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
