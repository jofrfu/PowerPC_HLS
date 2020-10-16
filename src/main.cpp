#include <stdint.h>
#include <vector>

#include "test_bench_utils.hpp"

#define I_MEM_SIZE 1024
#define D_MEM_SIZE 1024

typedef int32_t(*test_function_t)(uint32_t*, ap_uint<32>*, registers_t&);

int main(void) {
	uint32_t i_mem[I_MEM_SIZE];
	ap_uint<32> d_mem[D_MEM_SIZE];
	registers_t registers;

	std::vector<test_function_t> function_list;

	test_function_t function = [](uint32_t *i_mem, ap_uint<32> *d_mem, registers_t &registers) {
			if(read_byte_code("C:/Users/Jonas/Desktop/PowerPC/PowerPC_HLS/tests/addi.bin", i_mem, I_MEM_SIZE)) return -1;
			execute_single_instruction(i_mem[0], registers, d_mem);
			return 0;
		};
	function_list.push_back(function);

	for(uint32_t i = 0; i < function_list.size(); i++) {
		function_list[i](i_mem, d_mem, registers);
	}
}
