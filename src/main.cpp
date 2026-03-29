#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <unordered_map>
#include <filesystem>

struct Command {
  std::string name;
  std::vector<std::string> args;
};

Command parse_user_input(const std::string input) {
  Command cmd;

  std::istringstream iss(input);
  iss >> cmd.name;

  for (std::string arg; iss >> arg; ) {
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
    {"pwd", true},
  };

  return builtins.find(cmd) != builtins.end();
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

  return true;
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

        // If execv returns, there was an error
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

int main() {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  
  while (true) {
    std::cout << "$ ";
    std::string user_input;
    std::getline(std::cin, user_input);

    Command cmd = parse_user_input(user_input);
    
    if (cmd.name.empty()) continue;

    if (is_builtin(cmd.name)) {
      if (!execute_builtin(cmd))
        break;
    } else {
      execute_external_command(cmd);
    }
  }
  return 0;
}
