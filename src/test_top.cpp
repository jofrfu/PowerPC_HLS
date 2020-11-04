
#include "ppc_types.h"
#include "instruction_decode.hpp"
#include "fixed_point_processor.hpp"
#include <ap_int.h>

bool top(uint32_t *instruction_memory, ap_uint<32> *data_memory) {
#pragma HLS interface m_axi port=instruction_memory
#pragma HLS interface m_axi port=data_memory
	registers_t registers;
#pragma HLS ARRAY_PARTITION variable=registers.GPR complete dim=1
#pragma HLS ARRAY_PARTITION variable=registers.FPR complete dim=1
#pragma HLS ARRAY_PARTITION variable=registers.condition_reg.CR complete dim=1

	//for(int32_t i = 0; i < 1024; i++) {
	decode_result_t decoded = decode(instruction_memory[0]);
	fixed_point::load<ap_uint<32>*>(
			decoded.fixed_point_decode_result.execute_load,
			decoded.fixed_point_decode_result.load_store_decoded,
			registers, data_memory);
	fixed_point::store<ap_uint<32>*>(
			decoded.fixed_point_decode_result.execute_store,
			decoded.fixed_point_decode_result.load_store_decoded,
			registers, data_memory);
	fixed_point::load_string<ap_uint<32>*>(
            decoded.fixed_point_decode_result.execute_load_string,
            decoded.fixed_point_decode_result.load_store_decoded,
            registers, data_memory
	        );
    fixed_point::store_string<ap_uint<32>*>(
            decoded.fixed_point_decode_result.execute_store_string,
            decoded.fixed_point_decode_result.load_store_decoded,
            registers, data_memory);
	fixed_point::add_sub(
			decoded.fixed_point_decode_result.execute_add_sub,
			decoded.fixed_point_decode_result.add_sub_decoded,
			registers);
	fixed_point::multiply(
			decoded.fixed_point_decode_result.execute_mul,
			decoded.fixed_point_decode_result.mul_decoded,
			registers);
	fixed_point::divide(
			decoded.fixed_point_decode_result.execute_div,
			decoded.fixed_point_decode_result.div_decoded,
			registers);
	fixed_point::compare(
			decoded.fixed_point_decode_result.execute_compare,
			decoded.fixed_point_decode_result.cmp_decoded,
			registers);
	bool trap = fixed_point::trap(
			decoded.fixed_point_decode_result.execute_trap,
			decoded.fixed_point_decode_result.trap_decoded,
			registers);
	fixed_point::logical(
			decoded.fixed_point_decode_result.execute_logical,
			decoded.fixed_point_decode_result.log_decoded,
			registers);
	fixed_point::rotate(
			decoded.fixed_point_decode_result.execute_rotate,
			decoded.fixed_point_decode_result.rotate_decoded,
			registers);
	fixed_point::shift(
			decoded.fixed_point_decode_result.execute_shift,
			decoded.fixed_point_decode_result.shift_decoded,
			registers);
	fixed_point::system(
			decoded.fixed_point_decode_result.execute_system,
			decoded.fixed_point_decode_result.system_decoded,
			registers);

	if(trap)
		return trap;
	//}
	return false;
}
