#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <sstream>
#include <unistd.h>
#include <cstdlib>
#include <sys/wait.h>
#include <fcntl.h>

#include "parser.h"

using namespace std;

string find_command(const string& cmd)
{
    const char* path_env = getenv("PATH");
    if(!path_env) return "";

    stringstream ss(path_env);
    string dir;

    while(getline(ss,dir,':'))
    {
        string full = dir + "/" + cmd;

        if(access(full.c_str(),X_OK)==0)
            return full;
    }

    return "";
}

int main()
{
    cout<<unitbuf;
    cerr<<unitbuf;

    unordered_set<string> builtin={"cd","echo","exit","pwd","type"};

    string line;

    while(true)
    {
        cout<<"$ ";

        getline(cin,line);

        auto args=parse_command(line);

        if(args.empty())
            continue;

        string outfile;
        string errfile;

        vector<string> cmd_args;

        for(size_t i=0;i<args.size();i++)
        {
            if(args[i]=="?>" || args[i]=="1>")
            {
                outfile=args[i+1];
                i++;
            }
            else if(args[i]==">")
            {
                outfile=args[i+1];
                i++;
            }
            else if(args[i]=="2>")
            {
                errfile=args[i+1];
                i++;
            }
            else
            {
                cmd_args.push_back(args[i]);
            }
        }

        if(cmd_args.empty())
            continue;

        string cmd=cmd_args[0];

        if(cmd=="exit")
            break;

        if(cmd=="cd")
        {
            string dir = cmd_args.size()>1 ? cmd_args[1] : "";

            if(dir=="~")
                dir=getenv("HOME");

            if(chdir(dir.c_str())!=0)
                cout<<"cd: "<<dir<<": No such file or directory\n";

            continue;
        }

        if(cmd=="pwd")
        {
            char buf[1024];
            getcwd(buf,sizeof(buf));

            if(outfile.empty())
                cout<<buf<<"\n";
            else
            {
                int fd=open(outfile.c_str(),
                            O_WRONLY|O_CREAT|O_TRUNC,
                            0644);

                dprintf(fd,"%s\n",buf);
                close(fd);
            }

            continue;
        }

        if(cmd=="echo")
        {
            if(outfile.empty())
            {
                for(size_t i=1;i<cmd_args.size();i++)
                {
                    if(i>1) cout<<" ";
                    cout<<cmd_args[i];
                }

                cout<<"\n";
            }
            else
            {
                int fd=open(outfile.c_str(),
                            O_WRONLY|O_CREAT|O_TRUNC,
                            0644);

                for(size_t i=1;i<cmd_args.size();i++)
                {
                    if(i>1) dprintf(fd," ");
                    dprintf(fd,"%s",cmd_args[i].c_str());
                }

                dprintf(fd,"\n");

                close(fd);
            }

            continue;
        }

        if(cmd=="type")
        {
            if(cmd_args.size()<2)
                continue;

            string target=cmd_args[1];

            if(builtin.count(target))
                cout<<target<<" is a shell builtin\n";

            else
            {
                string path=find_command(target);

                if(path.empty())
                    cout<<target<<": not found\n";
                else
                    cout<<target<<" is "<<path<<"\n";
            }

            continue;
        }

        string path=find_command(cmd);

        if(path.empty())
        {
            cout<<cmd<<": command not found\n";
            continue;
        }

        pid_t pid=fork();

        if(pid==0)
        {
            if(!outfile.empty())
            {
                int fd=open(outfile.c_str(),
                            O_WRONLY|O_CREAT|O_TRUNC,
                            0644);

                dup2(fd,STDOUT_FILENO);
                close(fd);
            }

            if(!errfile.empty())
            {
                int fd=open(errfile.c_str(),
                            O_WRONLY|O_CREAT|O_TRUNC,
                            0644);

                dup2(fd,STDERR_FILENO);
                close(fd);
            }

            vector<char*> argv;

            for(auto &s:cmd_args)
                argv.push_back(const_cast<char*>(s.c_str()));

            argv.push_back(nullptr);

            execv(path.c_str(),argv.data());

            exit(1);
        }
        else
        {
            int status;
            waitpid(pid,&status,0);
        }
    }

    return 0;
}