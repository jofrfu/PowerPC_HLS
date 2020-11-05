
#include "ppc_types.h"
#include "instruction_decode.hpp"
#include "fixed_point_processor.hpp"
#include "pipeline.hpp"
#include <ap_int.h>

static registers_t registers;

bool top(uint32_t *instruction_memory, ap_uint<32> *data_memory) {
#pragma HLS interface m_axi port=instruction_memory
#pragma HLS interface m_axi port=data_memory

//#pragma HLS ARRAY_PARTITION variable=registers.GPR complete dim=1
//#pragma HLS ARRAY_PARTITION variable=registers.FPR complete dim=1
//#pragma HLS ARRAY_PARTITION variable=registers.condition_reg.CR complete dim=1

	//for(int32_t i = 0; i < 1024; i++) {
	decode_result_t decoded = pipeline::decode(instruction_memory[0]);
	return pipeline::execute(decoded, registers, data_memory);
}
