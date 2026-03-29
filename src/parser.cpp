#include "../include/common.hpp"
#include "../include/parser.hpp"
#include <string>
#include <sstream>
#include <iostream>

Command parse_user_input(const std::string& input) {
  Command cmd;

  enum State {NORMAL, IN_SINGLE, IN_DOUBLE};
  State state = NORMAL;

  std::vector<std::string> tokens;
  std::string current_token;

  for (size_t i = 0; i < input.size(); ++i) {
    char c = input[i];

    if (c == '\'' && state != IN_DOUBLE) {
      state = (state == IN_SINGLE) ? NORMAL : IN_SINGLE;
    }

    else if (c == '\"' && state != IN_SINGLE) {
      state = (state == IN_DOUBLE) ? NORMAL : IN_DOUBLE;
    }

    else if (c == ' ' && state == NORMAL) {
      if (!current_token.empty()) {
        tokens.push_back(current_token);
        current_token.clear();
      }
    }

    else if (c == '\\' && state != IN_SINGLE) {
      if (i + 1 < input.size()) {
        current_token += input[i + 1];
        ++i;
      }
    }

    else {
      current_token += c;
    }
  }

  if (!current_token.empty()) {
    tokens.push_back(current_token);
  }

  if (state != NORMAL) {
    std::cerr << "Erorr: unmatched quotes\n";
    return cmd;
  }

  if (!tokens.empty()) {
    cmd.name = tokens[0];
    cmd.args.assign(tokens.begin() + 1, tokens.end()); 
  }

  return cmd;
}