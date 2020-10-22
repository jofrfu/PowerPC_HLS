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

#define GCC_LOCATION "/opt/devkitpro/devkitPPC/bin/powerpc-eabi-as"
#define OBJCOPY_LOCATION "/opt/devkitpro/devkitPPC/bin/powerpc-eabi-objcopy"

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

            auto before = config["Before"];
            auto after = config["After"];
            if(after.is_null()) {
                FAIL("Config " + file.string() + " has no After entries!!!");
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
                if(system(compile_cmd.c_str())) {
                    FAIL("Error while compiling assembly code of file " + file.string() + " !!!");
                }
                if(system(objcopy_cmd.c_str())) {
                    FAIL("Error while converting elf to binary with objcopy of file " + file.string() + " !!!");
                }
                program_size = read_byte_code("as.bin", i_mem, I_MEM_SIZE);
                if(program_size < 0) {
                    FAIL("Config " + file.string() + " has no valid instructions or the compiler path is incorrect!!!");
                }
            } else {
                FAIL("Config " + file.string() + " has no valid instructions!!!");
            }

            auto data_file = config["Data File"];
            auto data = config["Data"];
            if(data_file.is_string()) {
                auto bin_name = data_file.get<std::string>();
                if(read_data(bin_name.c_str(), d_mem, D_MEM_SIZE) < 0) {
                    FAIL("Config " + file.string() + " has no valid data memory!!!");
                }
            } else if(data.is_binary()) {
                auto &binary = data.get_binary();
                if(binary.size() > D_MEM_SIZE*4) {
                    FAIL("Config " + file.string() + " has no valid data memory!!!");
                }
                // Ceiling division will add padding to word boundary
                for(uint32_t i = 0; i < (binary.size()+3)/4; i++) {
                    ap_uint<32> big;
                    if(i*4+3 > binary.size())
                        big(0, 7) = binary[i*4+3];
                    if(i*4+2 > binary.size())
                        big(8, 15) = binary[i*4+2];
                    if(i*4+1 > binary.size())
                        big(16, 23) = binary[i*4+1];

                    // Always in bounds
                    big(24, 31) = binary[i*4+0];
                }
            }

            // Set all values according to the "Before" field
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
                            registers.GPR[i] = GPR[i].get<int32_t>();
                        }
                    }
                }

                // Check FPRs
                if(!FPR.is_null()) {
                    for(uint32_t i = 0; i < 32; i++) {
                        if(!FPR[i].is_null()) {
                            registers.FPR[i] = FPR[i].get<uint32_t>();
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

                // Initialize data (single addressed data support)
                // Address is supplied on a per byte basis
                auto sa_data = before["Data"];
                if(!data.is_null()) {
                    for (uint32_t i = 0; i < D_MEM_SIZE; i++) {
                        if (!data[i*4].is_null()) {
                            ap_uint<32> little = sa_data[i*4].get<int32_t>();
                            ap_uint<32> big;
                            big(0, 7) = little(24, 31);
                            big(8, 15) = little(16, 23);
                            big(16, 23) = little(8, 15);
                            big(24, 31) = little(0, 7);
                            d_mem[i] = big;
                        }
                    }
                }
            }

            // Actual program execution
            execute_program(i_mem, program_size, registers, d_mem);

            {
                // Check all values according to the "After" field
                // Get registers
                auto GPR = after["GPR"];
                auto FPR = after["FPR"];
                auto CR = after["CR"];
                auto XER = after["FPR"];
                auto LR = after["LR"];

                // Check GPRs
                if (!GPR.is_null()) {
                    for (uint32_t i = 0; i < 32; i++) {
                        if (!GPR[i].is_null()) {
                            INFO("Checking GPR" + std::to_string(i) + ".");
                            REQUIRE((int32_t) registers.GPR[i] == GPR[i].get<int32_t>());
                            INFO("Check successful.");
                        }
                    }
                }

                // Check FPRs
                if (!FPR.is_null()) {
                    for (uint32_t i = 0; i < 32; i++) {
                        if (!FPR[i].is_null()) {
                            INFO("Checking FPR" + std::to_string(i) + ".");
                            REQUIRE(registers.FPR[i] == FPR[i].get<uint32_t>());
                            INFO("Check successful.");
                        }
                    }
                }

                // Check CRs
                if (!CR.is_null()) {
                    INFO("Checking CRs.");
                    REQUIRE(registers.condition_reg.condition_bits == CR.get<uint32_t>());
                    INFO("Check successful.");
                }

                // Check XER
                if (!XER.is_null()) {
                    INFO("Checking XER.");
                    REQUIRE(registers.fixed_exception_reg.exception_bits == XER.get<uint32_t>());
                    INFO("Check successful.");
                }

                // Check LR
                if (!LR.is_null()) {
                    INFO("Checking LR.");
                    REQUIRE(registers.link_register == LR.get<uint32_t>());
                    INFO("Check successful.");
                }

                // Compare data (single addressed data support)
                // Address is supplied on a per byte basis
                auto sa_data = after["Data"];
                if (!data.is_null()) {
                    for (uint32_t i = 0; i < D_MEM_SIZE; i++) {
                        if (!data[i * 4].is_null()) {
                            ap_uint<32> little = sa_data[i * 4].get<int32_t>();
                            ap_uint<32> big;
                            big(0, 7) = little(24, 31);
                            big(8, 15) = little(16, 23);
                            big(16, 23) = little(8, 15);
                            big(24, 31) = little(0, 7);
                            INFO("Checking data memory address " + std::to_string(i*4) + ".");
                            REQUIRE(d_mem[i] == big);
                            INFO("Check successful.");
                        }
                    }
                }
            }
        }
    }
}
