#include <string>
#include "../include/common.hpp"
#include <filesystem>
#include <unordered_map>
#include <iostream>
#include "../include/command.hpp"
#include "../include/external.hpp"


bool is_builtin(std::string& cmd) {
  static const std::unordered_map<std::string, bool> builtins = {
    {"exit", true},
    {"echo", true},
    {"type", true},
    {"pwd", true},
    {"cd", true},
  };

  return builtins.find(cmd) != builtins.end();
}

std::string find_home_dir() {
  const char* path_to_home_dir = std::getenv("HOME");

  if (path_to_home_dir == nullptr) {
    std::cout << "Your operating system is broken :)";
  }

  return std::string(path_to_home_dir);
}

bool execute_builtin(Command cmd) {

  if (cmd.name == "exit") 
    return false;
  
  else if (cmd.name == "echo") {
    if (!cmd.args.empty()) {
      for (const auto& arg : cmd.args) {
        std::cout << arg << ' ';
      }
      std::cout << '\n';
    }
  }

  else if (cmd.name == "type") {
    std::string arg = cmd.args[0];

    if (is_builtin(arg)) {
      std::cout << arg << " is a shell builtin" << '\n';
    } else {
      std::string executable_path = find_path_to_command(arg);

      if (!executable_path.empty()) {
      std::cout << arg << " is " << executable_path << '\n';
      } else {
        std::cout << arg << ": not found\n"; 
      }
    }
  }

  else if (cmd.name == "pwd") {
    std::string working_directory = std::filesystem::current_path();
    std::cout << working_directory << "\n";
  }

  else if (cmd.name == "cd") {
    if (cmd.args.empty()) {
      std::cout << '\n';
    }else {
      std::string new_path = cmd.args[0];
      
      if (new_path == "~") {
        new_path = find_home_dir();
      } 

      try {
        std::filesystem::current_path(new_path);
      } catch (const std::filesystem::filesystem_error& e) {
        std::cout << "cd: " << new_path << ": No such file or directory\n";
      }
    }
  }

  return true;
}