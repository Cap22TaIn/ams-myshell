#include "parser.h"
#include <cstdlib>
#include <cctype>

enum class State {
    NORMAL,
    SINGLE_QUOTE,
    DOUBLE_QUOTE
};

static std::string expand_variable(const std::string& line, size_t& i) {
    std::string var;

    i++; // skip $

    while(i < line.size() && (isalnum(line[i]) || line[i]=='_')) {
        var.push_back(line[i]);
        i++;
    }

    i--; // step back because loop will increment

    const char* val = getenv(var.c_str());
    return val ? std::string(val) : "";
}

std::vector<std::string> parse_command(const std::string& line) {

    std::vector<std::string> args;
    std::string cur;
    State state = State::NORMAL;

    for(size_t i=0;i<line.size();i++) {
        char c = line[i];

        switch(state) {

        case State::NORMAL:

            if(isspace(c)) {
                if(!cur.empty()) {
                    args.push_back(cur);
                    cur.clear();
                }
            }

            else if(c=='\'') {
                state = State::SINGLE_QUOTE;
            }

            else if(c=='"') {
                state = State::DOUBLE_QUOTE;
            }

            else if(c=='\\') {
                if(i+1 < line.size())
                    cur.push_back(line[++i]);
            }

            else if(c=='$') {
                cur += expand_variable(line,i);
            }

            else {
                cur.push_back(c);
            }

            break;

        case State::SINGLE_QUOTE:

            if(c=='\'') {
                state = State::NORMAL;
            }
            else {
                cur.push_back(c);
            }

            break;

        case State::DOUBLE_QUOTE:

            if(c=='"') {
                state = State::NORMAL;
            }

            else if(c=='$') {
                cur += expand_variable(line,i);
            }

            else if(c=='\\') {
                if(i+1 < line.size())
                    cur.push_back(line[++i]);
            }

            else {
                cur.push_back(c);
            }

            break;
        }
    }

    if(!cur.empty())
        args.push_back(cur);

    return args;
}