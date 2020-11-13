
#include "ppc_types.h"
#include "instruction_decode.hpp"
#include "branch_processor.hpp"
#include "pipeline.hpp"
#include <ap_int.h>

static registers_t registers;

void process(ap_uint<32> *instruction_memory, ap_uint<32> *data_memory) {
    ap_uint<32> current_instruction = pipeline::instruction_fetch(instruction_memory, registers);
	decode_result_t decoded = pipeline::decode(current_instruction);
	if(decoded.branch_decode_result.execute == branch::BRANCH) {
		// Extracting branch from the "pipeline" reduces the minimal execution time.
		branch::branch(decoded.branch_decode_result.branch_decoded, registers);
	} else {
		pipeline::execute(decoded, registers, data_memory);
	}
	registers.program_counter += 4;
}

void PowerPC(ap_uint<32> *instruction_memory, ap_uint<32> *data_memory) {
#pragma HLS interface m_axi port=instruction_memory
#pragma HLS interface m_axi port=data_memory
#pragma HLS ARRAY_PARTITION variable=registers.GPR complete dim=1
#pragma HLS ARRAY_PARTITION variable=registers.FPR complete dim=1
#pragma HLS ARRAY_PARTITION variable=registers.condition_reg.CR complete dim=1

	while(true) {
		process(instruction_memory, data_memory);
	}
}
