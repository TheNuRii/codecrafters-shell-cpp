#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>

enum class Command {
  EXIT,
  ECHO,
  TYPE,
  UNKNOWN
};

int main() {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::cout << "$ ";
  std::string user_input;

  while(std::getline(std::cin, user_input)){
    // prase user input by space and execute command
    size_t pos = user_input.find(' ');
    std::string command = user_input.substr(0, pos);
    std::string command_args = (pos == std::string::npos) ? "" : user_input.substr(pos + 1); // just checking for empty string (no args)

    if (command == "exit") break;
    else if (command == "type"){
      if (command_args == "exit" || command_args == "echo" || command_args == "type") {
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
