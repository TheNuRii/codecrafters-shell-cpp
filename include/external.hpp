#pragma once
#include <string>
#include <vector>
#include "common.hpp"

std::string find_path_to_command(const std::string& command);

void execute_external_command(const Command &cmd);
