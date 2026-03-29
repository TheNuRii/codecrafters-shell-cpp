#include "../include/common.hpp"
#include "../include/parser.hpp"
#include <string>
#include <sstream>

Command parse_user_input(const std::string& input) {
    Command cmd;
    enum State { NORMAL, IN_SINGLE, IN_DOUBLE };
    State state = NORMAL;

    std::istringstream iss(input);
    iss >> cmd.name;

    std::string rest = input.substr(cmd.name.size()); // rest input

  size_t start = rest.find_first_not_of(' ');   // trim leading spaces
    if (start != std::string::npos)
        rest = rest.substr(start);
    else
        return cmd;

    std::string current_token;

    for (size_t i = 0; i < rest.size(); ++i) {
        char c = rest[i];

        
        if (c == '\'' && state != IN_DOUBLE) {
            state = (state == IN_SINGLE) ? NORMAL : IN_SINGLE;
        }
        else if (c == '\"' && state != IN_SINGLE) {
            state = (state == IN_DOUBLE) ? NORMAL : IN_DOUBLE;
        }
        else if (c == ' ' && state == NORMAL) {
            if (!current_token.empty()) {
                cmd.args.push_back(current_token);
                current_token.clear();
            }
        }
        else if (c == '\\' && state != IN_SINGLE) {
            // escape next char
            if (i + 1 < rest.size()) {
                current_token += rest[i + 1];
                ++i;
            }
        } else {
            current_token += c;
        }
    }

    if (!current_token.empty())
        cmd.args.push_back(current_token);
      
    return cmd;
}