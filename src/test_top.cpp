
#include "ppc_types.h"
#include "instruction_decode.hpp"
#include "branch_processor.hpp"
#include "pipeline.hpp"
#include <ap_int.h>

static registers_t registers;

void process(ap_uint<32> *instruction_memory, ap_uint<32> *data_memory) {
    ap_uint<32> current_instruction = pipeline::instruction_fetch(instruction_memory, registers);
    decode_result_t decoded = pipeline::decode(current_instruction);
    pipeline::result_t result = {0, 0, false, false};
	if(decoded.branch_decode_result.execute == branch::BRANCH) {
		// Extracting branch from the "pipeline" reduces the minimal execution time.
		branch::branch(decoded.branch_decode_result.branch_decoded, registers);
	} else {
		auto operands = pipeline::fetch_operands(decoded, registers);
		result = pipeline::execute(decoded, registers, operands, data_memory);
		pipeline::write_back(result, registers);
	}
	registers.program_counter += 4;
}

void PowerPC(ap_uint<32> instruction_memory[1024], ap_uint<32> data_memory[1024]) {
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS interface bram port=instruction_memory
#pragma HLS interface bram port=data_memory
#pragma HLS ARRAY_PARTITION variable=registers.GPR complete dim=1
#pragma HLS ARRAY_PARTITION variable=registers.FPR complete dim=1
#pragma HLS ARRAY_PARTITION variable=registers.condition_reg.CR complete dim=1

	registers.program_counter = 0;

	//while(true) {
	for(int i = 0; i < 1024; i++) {
		process(instruction_memory, data_memory);
	}
}
