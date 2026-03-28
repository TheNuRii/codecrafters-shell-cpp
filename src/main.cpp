#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <vector>
#include <unordered_map>

struct Command {
  std::string name;
  std::vector<std::string> args;
};

Command split_user_input(const std::string& input) {
  Command cmd;

  std::istringstream iss(input);
  iss >> cmd.name;

  for (std::string arg; iss; ) {
    cmd.args.push_back(arg);
  }

  return cmd;
}

std::string find_path_to_command(const std::string& command) {
  const char* path_to_env = std::getenv("PATH");
  if (path_to_env == nullptr) {
    return "";
  }
  std::string path_env(path_to_env);
  std::stringstream ss_path(path_env);
  std::string path;
  
  while (std::getline(ss_path, path, ':')) {
    std::string full_path = path + "/" + command;
    if (access(full_path.c_str(), X_OK) == 0) {
        return full_path;
    }
  }

  return "";
}

bool is_builtin(std::string& cmd) {
  static const std::unordered_map<std::string, bool> builtins = {
    {"exit", true},
    {"echo", true},
    {"type", true},
  };

  return builtins.find(cmd) != builtins.end();
}

int main() {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::cout << "$ ";
  std::vector<std::string> builtins = {"exit", "echo", "type"};
  std::string user_input;

  while(std::getline(std::cin, user_input)){
    // prase user input by space and execute command
    /*
    std::vector<std::string> tokens;
    size_t pos = user_input.find(' ');
    std::string command = user_input.substr(0, pos);
    std::string command_args = (pos == std::string::npos) ? "" : user_input.substr(pos + 1); // just checking for empty string (no args)
    */
    std::vector<std::string> tokens = SplitUserInput(user_input);
    if (tokens.empty()) {
      std::cout << '\n' << "$ " ;
      continue;
    }
    std::string command = tokens[0];
  
    if (command == "exit") break;

    if (command == "type"){
      if (command_arg == "exit" || command_args == "echo" || command_args == "type") {
        std::cout << command_args << " is a shell builtin\n" << "$ ";
        continue;
      }

      const char* path_env_cstr = std::getenv("PATH");
      if (path_env_cstr == nullptr) {
        std::cerr << command_args << ": not found\n" << "$ ";        
        continue;
      }

      std::string path_env = std::getenv("PATH");
      std::stringstream ss_path(path_env);
      std::string path;
      //printf("PATH: %s\n", path_env.c_str());
      //printf("Command args: %s\n", command_args.c_str());
      bool found = false;

      while (std::getline(ss_path, path, ':')) {
        std::string full_path = path + "/" + command_args;
        if (access(full_path.c_str(), X_OK) == 0) {
            std::cout << command_args << " is " << full_path << "\n" << "$ ";
            found = true;
            break;
        }
      }
      
      if (!found) {
        std::cerr << command_args << ": not found\n" << "$ ";
      }
      continue;
    }

    if (command == "echo") {
      std::cout << command_args << '\n';
      std::cout << "$ ";
      continue;
    }
    std::cerr << command << ": command not found\n" << "$ ";
  }
  
  return 0;
}
