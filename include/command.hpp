#pragma once
#include <string>
#include <vector>
#include "common.hpp"

bool is_builtin(std::string& cmd);

std::string find_home_dir();

bool execute_builtin(Command cmd);

std::string find_path_to_command(const std::string& command);