#include <stdint.h>

#include "instruction_decode.hpp"

int main(void) {
	uint32_t test = 0x24849283494;
	//load_store_decode_t res = decode_loaded(test);
	decode(test);
}
