#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>
#include <unistd.h>
#include <cstdlib>
#include <cctype>

using namespace std;

vector<string> parse(const string& s) {
    vector<string> args;
    string cur;
    bool in_quote = false;

    for(char c : s) {
        if(c == '\'') {
            in_quote = !in_quote;
        }
        else if(isspace(c) && !in_quote) {
            if(!cur.empty()) {
                args.push_back(cur);
                cur.clear();
            }
        }
        else {
            cur.push_back(c);
        }
    }

    if(!cur.empty()) args.push_back(cur);
    return args;
}

string find_command(const string& cmd) {
    const char* path_env = getenv("PATH");
    if(!path_env) return "";

    stringstream ss(path_env);
    string path;

    while(getline(ss, path, ':')) {
        string full = path + "/" + cmd;
        if(access(full.c_str(), X_OK) == 0)
            return full;
    }

    return "";
}

int main() {
    cout << unitbuf;
    cerr << unitbuf;

    const unordered_set<string> builtin = {"cd","echo","exit","pwd","type"};
    string line;

    while(true) {
        cout << "$ ";
        getline(cin, line);

        auto args = parse(line);
        if(args.empty()) continue;

        const string& command = args[0];

        if(command == "exit") {
            break;
        }

        else if(command == "echo") {
            for(size_t i = 1; i < args.size(); i++) {
                if(i > 1) cout << " ";
                cout << args[i];
            }
            cout << "\n";
        }

        else if(command == "pwd") {
            char buf[1024];
            if(getcwd(buf, sizeof(buf)))
                cout << buf << "\n";
        }

        else if(command == "cd") {
            string dir = args.size() > 1 ? args[1] : "";

            if(dir == "~")
                dir = getenv("HOME");

            if(chdir(dir.c_str()) != 0)
                cout << "cd: " << dir << ": No such file or directory\n";
        }

        else if(command == "type") {
            if(args.size() < 2) continue;

            string target = args[1];

            if(builtin.count(target)) {
                cout << target << " is a shell builtin\n";
            } 
            else {
                string path = find_command(target);
                if(path.empty())
                    cout << target << ": not found\n";
                else
                    cout << target << " is " << path << "\n";
            }
        }

        else {
            string path = find_command(command);

            if(path.empty()) {
                cout << command << ": command not found\n";
            } 
            else {
                system(line.c_str());
            }
        }
    }
}