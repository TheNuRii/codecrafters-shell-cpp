#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::cout << "$ ";
  std::string user_input;

  while(std::getline(std::cin, user_input)){
    // prase user input by space and execute command
    size_t pos = user_input.find(' ');
    std::string command = user_input.substr(0, pos);
    std::string command_args = user_input.substr(pos + 1);

    if (command == "exit") break;
    else if (command == "echo") {
      std::cout << command_args << '\n';
      std::cout << "$ ";
      continue;
    }
    std::cerr << command << ": command not found\n" << "$ ";
  }  
}
