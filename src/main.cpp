#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <unordered_map>

struct Command {
  std::string name;
  std::vector<std::string> args;
};

Command parse_user_input(const std::string& input) {
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

bool execute_builtin(Command cmd) {

  if (cmd.name == "exit") 
    return false;
  
  else if (cmd.name == "echo") {
    if (!cmd.args.empty()) {
      std::cout << cmd.args[0] << '\n';
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

  return true;
}


void execute_external_command(Command cmd) {
  std::string executable_path = find_path_to_command(cmd.name);

  if (executable_path.empty()) {
    std::cout << cmd.name << ": command not found\n";
    return;
  }

  pid_t pid = fork();

  if (pid == 0) {
    // Child process
    std::string executable_path = find_path_to_command(cmd.name);

    std::vector<std::string> arg_sep_space;
    arg_sep_space.push_back(cmd.name);

    std::string arg = cmd.args[0];
    size_t j{0};

    for (size_t i{0}; i < arg.size(); ++i) {
      if (arg[i] == ' ') {
        if (i > j) {
          arg_sep_space.push_back(arg.substr(j, i - j));
        }

        j = i + 1;
      }
    }

    if (j < arg.size()) 
      arg_sep_space.push_back(arg.substr(j));
    
    std::vector<char *> argp;

    for (auto &s: arg_sep_space) {
      argp.push_back(const_cast<char *>(s.c_str()));
    }

    argp.push_back(nullptr); // it expect nullptr to stop

    execv(executable_path.c_str(), argp.data());

  } else if (pid > 0) {
    // Parent process
    int status;
    waitpid(pid, &status, 0);

  } else {
    std::cout << "Error: Failed to fork\n"; 
  }
}
int main() {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::cout << "$ ";
  std::string user_input;
  while (std::getline(std::cin, user_input)) {
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
