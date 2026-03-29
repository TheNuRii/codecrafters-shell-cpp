#include "../include/common.hpp"
#include <string>
#include <iostream>
#include <sys/wait.h>
#include <sstream>

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

void execute_external_command(const Command &cmd) {
    std::string executable_path = find_path_to_command(cmd.name);

    if (executable_path.empty()) {
        std::cout << cmd.name << ": command not found\n";
        return;
    }

    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        std::vector<char*> argv;
        argv.push_back(const_cast<char*>(cmd.name.c_str())); // argv[0] = command name

        for (const auto &arg : cmd.args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr); // execv expects null-terminated array

        execv(executable_path.c_str(), argv.data());

        perror("execv");
        _exit(EXIT_FAILURE);
    } 
    else if (pid > 0) {
        // Parent process
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
        }
    } 
    else {
        std::cerr << "Error: Failed to fork\n";
    }
}