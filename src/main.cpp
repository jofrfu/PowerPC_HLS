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
#include "fixed_point_utils.hpp"

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

    // Use environment variable [PROGRAM_TO_DEBUG] to ONLY execute a single test for one JSON program file
    // Usage: PROGRAM_TO_DEBUG=[my_test.json | test_directory]
    const char* program_to_debug_env = std::getenv("PROGRAM_TO_DEBUG");


    for (const auto &entry : std::filesystem::recursive_directory_iterator(PROGRAM_PATH)) {
        auto &path = entry.path();
        if (path.extension() == ".json") {

            // Only add single program or directory to debug if required ; Skip all other tests
            if (program_to_debug_env != nullptr) {
                if (path.filename() != program_to_debug_env && path.parent_path().filename() != program_to_debug_env) {
                    continue;
                }
            }

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
                        big(7, 0) = binary[i*4+3];
                    if(i*4+2 > binary.size())
                        big(15, 8) = binary[i*4+2];
                    if(i*4+1 > binary.size())
                        big(23, 16) = binary[i*4+1];

                    // Always in bounds
                    big(31, 24) = binary[i*4+0];
                }
            }

            // Set all values according to the "Before" field
            if(!before.is_null()) {
                // Initialize registers
                auto GPR = before["GPR"];
                auto FPR = before["FPR"];
                auto CR = before["CR"];
                auto XER = before["XER"];
                auto LR = before["LR"];

                // Check GPRs
                if(!GPR.is_null()) {
                    for(uint32_t i = 0; i < 32; i++) {
                        if(!GPR[std::to_string(i)].is_null()) {
                            registers.GPR[i] = GPR[std::to_string(i)].get<int32_t>();
                        }
                    }
                }

                // Check FPRs
                if(!FPR.is_null()) {
                    for(uint32_t i = 0; i < 32; i++) {
                        if(!FPR[std::to_string(i)].is_null()) {
                            registers.FPR[i] = FPR[std::to_string(i)].get<uint32_t>();
                        }
                    }
                }

                // Check CRs
                if(!CR.is_null()) {
                    if(CR.is_object()) {
                        for(uint32_t i = 0; i < 8; i++) {
                            auto CR_i = CR["CR" + std::to_string(i)];
                            if (CR_i.is_object()) {
                                condition_field_t &reg = registers.condition_reg[i];
                                // Fixed Point conditions
                                if(CR_i["LT"].is_boolean()) {
                                    reg.condition_fixed_point.LT = CR_i["LT"].get<bool>();
                                }
                                if(CR_i["GT"].is_boolean()) {
                                    reg.condition_fixed_point.GT = CR_i["GT"].get<bool>();
                                }
                                if(CR_i["EQ"].is_boolean()) {
                                    reg.condition_fixed_point.EQ = CR_i["EQ"].get<bool>();
                                }
                                if(CR_i["SO"].is_boolean()) {
                                    reg.condition_fixed_point.SO = CR_i["SO"].get<bool>();
                                }
                                // Floating Point conditions
                                if(CR_i["FL"].is_boolean()) {
                                    reg.condition_floating_point_compare.FL = CR_i["FL"].get<bool>();
                                }
                                if(CR_i["FG"].is_boolean()) {
                                    reg.condition_floating_point_compare.FG = CR_i["FG"].get<bool>();
                                }
                                if(CR_i["FE"].is_boolean()) {
                                    reg.condition_floating_point_compare.FE = CR_i["FE"].get<bool>();
                                }
                                if(CR_i["FU"].is_boolean()) {
                                    reg.condition_floating_point_compare.FU = CR_i["FU"].get<bool>();
                                }
                                // Floating Point conditions CR1
                                if(i == 1) {
                                    if(CR_i["OX"].is_boolean()) {
                                        reg.condition_floating_point.OX = CR_i["OX"].get<bool>();
                                    }
                                    if(CR_i["VX"].is_boolean()) {
                                        reg.condition_floating_point.VX = CR_i["VX"].get<bool>();
                                    }
                                    if(CR_i["FEX"].is_boolean()) {
                                        reg.condition_floating_point.FEX = CR_i["FEX"].get<bool>();
                                    }
                                    if(CR_i["FX"].is_boolean()) {
                                        reg.condition_floating_point.FX = CR_i["FX"].get<bool>();
                                    }
                                }
                            }
                        }
                    } else {
                        registers.condition_reg.setCR(CR.get<uint32_t>());
                    }
                }

                // Check XER
                if(!XER.is_null()) {
                    if(XER.is_object()) {
                        if(XER["SO"].is_boolean()) {
                            registers.fixed_exception_reg.exception_fields.SO = XER["SO"].get<bool>();
                        }
                        if(XER["OV"].is_boolean()) {
                            registers.fixed_exception_reg.exception_fields.OV = XER["OV"].get<bool>();
                        }
                        if(XER["CA"].is_boolean()) {
                            registers.fixed_exception_reg.exception_fields.CA = XER["CA"].get<bool>();
                        }
                        if(XER["String_Bytes"].is_number()) {
                            registers.fixed_exception_reg.exception_fields.string_bytes = XER["String_Bytes"].get<uint8_t>();
                        }
                    } else {
                        registers.fixed_exception_reg.exception_bits = XER.get<uint32_t>();
                    }
                }

                // Check LR
                if(!LR.is_null()) {
                    registers.link_register = LR.get<uint32_t>();
                }

                // Initialize data (single addressed data support)
                // Address is supplied on a per byte basis
                auto sa_data = before["Data"];
                if(!sa_data.is_null()) {
                    for (uint32_t i = 0; i < D_MEM_SIZE; i++) {
                        if (!sa_data[std::to_string(i*4)].is_null()) {
                            ap_uint<32> little = sa_data[std::to_string(i*4)].get<int32_t>();
                            ap_uint<32> big;
                            big(7, 0) = little(31, 24);
                            big(15, 8) = little(23, 16);
                            big(23, 16) = little(15, 8);
                            big(31, 24) = little(7, 0);
                            d_mem[i] = big;
                        }
                    }
                }
            }

            bool trap_happened = false;
            uint32_t trap_instructions_pos = 0;
            trap_handler_t trap_handler = [&trap_happened, &trap_instructions_pos](uint32_t i) {
                trap_happened = true;
                trap_instructions_pos = i;
            };

            // Actual program execution
            execute_program(i_mem, program_size, registers, d_mem, trap_handler);

            {
                // Check all values according to the "After" field
                // Get registers
                auto GPR = after["GPR"];
                auto FPR = after["FPR"];
                auto CR = after["CR"];
                auto XER = after["XER"];
                auto LR = after["LR"];

                // Check GPRs
                if (!GPR.is_null()) {
                    for (uint32_t i = 0; i < 32; i++) {
                        if (!GPR[std::to_string(i)].is_null()) {
                            INFO("Checking GPR" + std::to_string(i) + ".");
                            REQUIRE((int32_t) registers.GPR[i] == GPR[std::to_string(i)].get<int32_t>());
                        }
                    }
                }

                // Check FPRs
                if (!FPR.is_null()) {
                    for (uint32_t i = 0; i < 32; i++) {
                        if (!FPR[std::to_string(i)].is_null()) {
                            INFO("Checking FPR" + std::to_string(i) + ".");
                            REQUIRE(registers.FPR[i] == FPR[std::to_string(i)].get<uint32_t>());
                        }
                    }
                }

                // Check CRs
                if (!CR.is_null()) {
                    if(CR.is_object()) {
                        for(uint32_t i = 0; i < 8; i++) {
                            auto CR_i = CR["CR" + std::to_string(i)];
                            if (CR_i.is_object()) {
                                condition_field_t &reg = registers.condition_reg[i];
                                // Fixed Point conditions
                                if(CR_i["LT"].is_boolean()) {
                                    INFO("Checking CR" + std::to_string(i) + " LT bit.")
                                    REQUIRE(reg.condition_fixed_point.LT == CR_i["LT"].get<bool>());
                                }
                                if(CR_i["GT"].is_boolean()) {
                                    INFO("Checking CR" + std::to_string(i) + " GT bit.")
                                    REQUIRE(reg.condition_fixed_point.GT == CR_i["GT"].get<bool>());
                                }
                                if(CR_i["EQ"].is_boolean()) {
                                    INFO("Checking CR" + std::to_string(i) + " EQ bit.")
                                    REQUIRE(reg.condition_fixed_point.EQ == CR_i["EQ"].get<bool>());
                                }
                                if(CR_i["SO"].is_boolean()) {
                                    INFO("Checking CR" + std::to_string(i) + " SO bit.")
                                    REQUIRE(reg.condition_fixed_point.SO == CR_i["SO"].get<bool>());
                                }
                                // Floating Point conditions
                                if(CR_i["FL"].is_boolean()) {
                                    INFO("Checking CR" + std::to_string(i) + " FL bit.")
                                    REQUIRE(reg.condition_floating_point_compare.FL == CR_i["FL"].get<bool>());
                                }
                                if(CR_i["FG"].is_boolean()) {
                                    INFO("Checking CR" + std::to_string(i) + " FG bit.")
                                    REQUIRE(reg.condition_floating_point_compare.FG == CR_i["FG"].get<bool>());
                                }
                                if(CR_i["FE"].is_boolean()) {
                                    INFO("Checking CR" + std::to_string(i) + " FE bit.")
                                    REQUIRE(reg.condition_floating_point_compare.FE == CR_i["FE"].get<bool>());
                                }
                                if(CR_i["FU"].is_boolean()) {
                                    INFO("Checking CR" + std::to_string(i) + " FU bit.")
                                    REQUIRE(reg.condition_floating_point_compare.FU == CR_i["FU"].get<bool>());
                                }
                                // Floating Point conditions CR1
                                if(i == 1) {
                                    if(CR_i["OX"].is_boolean()) {
                                        INFO("Checking CR" + std::to_string(i) + " OX bit.")
                                        REQUIRE(reg.condition_floating_point.OX == CR_i["OX"].get<bool>());
                                    }
                                    if(CR_i["VX"].is_boolean()) {
                                        INFO("Checking CR" + std::to_string(i) + " VX bit.")
                                        REQUIRE(reg.condition_floating_point.VX == CR_i["VX"].get<bool>());
                                    }
                                    if(CR_i["FEX"].is_boolean()) {
                                        INFO("Checking CR" + std::to_string(i) + " FEX bit.")
                                        REQUIRE(reg.condition_floating_point.FEX == CR_i["FEX"].get<bool>());
                                    }
                                    if(CR_i["FX"].is_boolean()) {
                                        INFO("Checking CR" + std::to_string(i) + " FX bit.")
                                        REQUIRE(reg.condition_floating_point.FX == CR_i["FX"].get<bool>());
                                    }
                                }
                            }
                        }
                    } else {
                        INFO("Checking CRs.");
                        REQUIRE(registers.condition_reg.getCR() == CR.get<uint32_t>());
                    }
                }

                // Check XER
                if (!XER.is_null()) {
                    if(XER.is_object()) {
                        if(XER["SO"].is_boolean()) {
                            INFO("Checking XER SO bit.");
                            REQUIRE(registers.fixed_exception_reg.exception_fields.SO == XER["SO"].get<bool>());
                        }
                        if(XER["OV"].is_boolean()) {
                            INFO("Checking XER OV bit.");
                            REQUIRE(registers.fixed_exception_reg.exception_fields.OV == XER["OV"].get<bool>());
                        }
                        if(XER["CA"].is_boolean()) {
                            INFO("Checking XER CA bit.");
                            REQUIRE(registers.fixed_exception_reg.exception_fields.CA == XER["CA"].get<bool>());
                        }
                        if(XER["String_Bytes"].is_number()) {
                            INFO("Checking XER string bytes field.");
                            REQUIRE(registers.fixed_exception_reg.exception_fields.string_bytes == XER["String_Bytes"].get<uint8_t>());
                        }
                    } else {
                        INFO("Checking XER.");
                        REQUIRE(registers.fixed_exception_reg.exception_bits == XER.get<uint32_t>());
                    }
                }

                // Check LR
                if(!LR.is_null()) {
                    INFO("Checking LR.");
                    REQUIRE(registers.link_register == LR.get<uint32_t>());
                }

                // Check trap handler execution
                auto trap = after["Trap"];
                if(trap.is_object()) {
                    auto trap_bool = trap["Occurred"];
                    if(trap_bool.is_boolean()) {
                        INFO("Checking trap occurrence.");
                        REQUIRE(trap_happened == trap_bool.get<bool>());
                        if(trap["Position"].is_number()) {
                            INFO("Checking trap occurrence position.");
                            REQUIRE(trap_instructions_pos == trap["Position"].get<uint32_t>());
                        }
                    }
                }

                // Compare data (single addressed data support)
                // Address is supplied on a per byte basis
                auto sa_data = after["Data"];
                if (!sa_data.is_null()) {
                    for (uint32_t i = 0; i < D_MEM_SIZE; i++) {
                        if (!sa_data[std::to_string(i*4)].is_null()) {
                            ap_uint<32> little = sa_data[std::to_string(i*4)].get<int32_t>();
                            ap_uint<32> big;
                            big(7, 0) = little(31, 24);
                            big(15, 8) = little(23, 16);
                            big(23, 16) = little(15, 8);
                            big(31, 24) = little(7, 0);
                            INFO("Checking data memory address " + std::to_string(i*4) + ".");
                            REQUIRE(d_mem[i] == big);
                        }
                    }
                }
            }
        }
    }
}
