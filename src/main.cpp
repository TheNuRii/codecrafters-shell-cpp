#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include "../include/common.hpp"
#include "../include/parser.hpp"
#include "../include/command.hpp"
#include "../include/external.hpp"

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
