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

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <json.hpp>
#include <vector>
#include <iostream>
#include <filesystem>
#include <string>

#include "test_bench_utils.hpp"

#define PROGRAM_PATH "../tests/programs"
#define INSTRUCTION_PATH "../tests/instructions"

#define GCC_LOCATION ""
#define OBJCOPY_LOCATION ""

#define I_MEM_SIZE 1024
#define D_MEM_SIZE 1024

TEST_CASE("Automatic program execution", "[program execution]") {
    uint32_t i_mem[I_MEM_SIZE];
    ap_uint<32> d_mem[D_MEM_SIZE];
    registers_t registers;

    std::vector<std::filesystem::path> filenames;
    for (const auto &entry : std::filesystem::directory_iterator(PROGRAM_PATH)) {
        auto &path = entry.path();
        if(path.extension() == ".json") {
            // File is a json configuration
            filenames.push_back(path);
        }
    }

    for(const auto &file : filenames) {
        SECTION("Testing file " + file.string()) {
            std::ifstream input(file);
            nlohmann::json config;
            input >> config;

            int32_t program_size;

            if(config.is_null()) {
                FAIL("Config is empty for " + file.string() + "!!!");
            }

            auto instruction_file = config["Instructions File"];
            auto instructions = config["Instructions"];
            auto assembly = config["Assembly"];
            if(instruction_file.is_null() && instructions.is_null() && assembly.is_null()) {
                FAIL("Config " + file.string() + " has no instructions!!!");
            }

            if(instruction_file.is_string()) {
                auto bin_name = instruction_file.get<std::string>();
                program_size = read_byte_code(bin_name.c_str(), i_mem, I_MEM_SIZE);
                if(program_size < 0) {
                    FAIL("Config " + file.string() + " has no valid instructions!!!");
                }
            } else if(instructions.is_binary()) {
                auto &binary = instructions.get_binary();
                memcpy(i_mem, binary.data(), binary.size());
                program_size = binary.size()/4;
            } else if(assembly.is_string()) {
                auto as_program = assembly.get<std::string>();
                auto compile_cmd = "echo \"" + as_program + "\" | " + GCC_LOCATION + " -o as.elf";
                auto objcopy_cmd = std::string(OBJCOPY_LOCATION) + " -O binary as.elf as.bin";
                system(compile_cmd.c_str());
                system(objcopy_cmd.c_str());
                program_size = read_byte_code("as.bin", i_mem, I_MEM_SIZE);
                if(program_size < 0) {
                    FAIL("Config " + file.string() + " has no valid instructions or the compiler path is incorrect!!!");
                }
            }else {
                FAIL("Config " + file.string() + " has no valid instructions!!!");
            }

            auto before = config["Before"];
            auto after = config["After"];
            if(after.is_null()) {
                FAIL("Config " + file.string() + " has no After entries!!!");
            }

            if(!before.is_null()) {
                // Initialize registers
                auto GPR = before["GPR"];
                auto FPR = before["FPR"];
                auto CR = before["CR"];
                auto XER = before["FPR"];
                auto LR = before["LR"];

                // Check GPRs
                if(!GPR.is_null()) {
                    for(uint32_t i = 0; i < 32; i++) {
                        if(!GPR[i].is_null()) {
                            registers.GPR[i] = GPR[i];
                        }
                    }
                }

                // Check FPRs
                if(!FPR.is_null()) {
                    for(uint32_t i = 0; i < 32; i++) {
                        if(!FPR[i].is_null()) {
                            registers.FPR[i] = FPR[i];
                        }
                    }
                }

                // Check CRs
                if(!CR.is_null()) {
                    registers.condition_reg.condition_bits = CR.get<uint32_t>();
                }

                // Check XER
                if(!XER.is_null()) {
                    registers.fixed_exception_reg.exception_bits = XER.get<uint32_t>();
                }

                // Check LR
                if(!LR.is_null()) {
                    registers.link_register = LR.get<uint32_t>();
                }
            }
        }
    }
}
