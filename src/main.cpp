#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <sstream>
#include <unistd.h>
#include <cstdlib>
#include <sys/wait.h>

#include "parser.h"

using namespace std;

string find_command(const string& cmd) {
    const char* path_env = getenv("PATH");
    if(!path_env) return "";

    stringstream ss(path_env);
    string dir;

    while(getline(ss, dir, ':')) {
        string full = dir + "/" + cmd;
        if(access(full.c_str(), X_OK) == 0)
            return full;
    }

    return "";
}

int main() {
    cout << unitbuf;
    cerr << unitbuf;

    unordered_set<string> builtin = {"cd","echo","exit","pwd","type"};
    string line;

    while(true) {

        cout << "$ ";
        getline(cin, line);

        auto args = parse_command(line);

        if(args.empty())
            continue;

        string cmd = args[0];

        // EXIT
        if(cmd == "exit") {
            break;
        }

        // ECHO
        else if(cmd == "echo") {

            for(size_t i=1;i<args.size();i++){
                if(i>1) cout<<" ";
                cout<<args[i];
            }

            cout<<"\n";
        }

        // PWD
        else if(cmd == "pwd") {

            char buf[1024];

            if(getcwd(buf,sizeof(buf)))
                cout<<buf<<"\n";
        }

        // CD
        else if(cmd == "cd") {

            string dir = args.size()>1 ? args[1] : "";

            if(dir=="~")
                dir = getenv("HOME");

            if(chdir(dir.c_str()) != 0)
                cout<<"cd: "<<dir<<": No such file or directory\n";
        }

        // TYPE
        else if(cmd == "type") {

            if(args.size()<2)
                continue;

            string target = args[1];

            if(builtin.count(target))
                cout<<target<<" is a shell builtin\n";

            else{

                string path = find_command(target);

                if(path.empty())
                    cout<<target<<": not found\n";

                else
                    cout<<target<<" is "<<path<<"\n";
            }
        }

        // EXTERNAL COMMAND
        else {

            string path = find_command(cmd);

            if(path.empty()){
                cout<<cmd<<": command not found\n";
                continue;
            }

            pid_t pid = fork();

            if(pid == 0) {

                vector<char*> argv;

                for(auto& s : args)
                    argv.push_back(const_cast<char*>(s.c_str()));

                argv.push_back(nullptr);

                execv(path.c_str(), argv.data());

                exit(1);
            }

            else {

                int status;
                waitpid(pid, &status, 0);
            }
        }
    }
}