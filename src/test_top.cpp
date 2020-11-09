
#include "ppc_types.h"
#include "instruction_decode.hpp"
#include "fixed_point_processor.hpp"
#include "branch_processor.hpp"
#include "pipeline.hpp"
#include <ap_int.h>

static registers_t registers;

void process(uint32_t *instruction, ap_uint<32> *data_memory) {
	decode_result_t decoded = pipeline::decode(instruction[registers.program_counter(31, 2)]);
	if(decoded.branch_decode_result.execute == branch::BRANCH) {
		// Extracting branch from the "pipeline" reduces the minimal execution time.
		branch::branch(decoded.branch_decode_result.branch_decoded, registers);
	} else {
		pipeline::execute(decoded, registers, data_memory);
	}
	registers.program_counter += 4;
}

void top(uint32_t instruction_memory[1024], ap_uint<32> data_memory[1024]) {
#pragma HLS interface bram port=instruction_memory
#pragma HLS interface bram port=data_memory
#pragma HLS ARRAY_PARTITION variable=registers.GPR complete dim=1
#pragma HLS ARRAY_PARTITION variable=registers.FPR complete dim=1
#pragma HLS ARRAY_PARTITION variable=registers.condition_reg.CR complete dim=1

	for(int i = 0; i < 1024; i++) {
#pragma HLS dataflow
		process(instruction_memory, data_memory);
	}
}
