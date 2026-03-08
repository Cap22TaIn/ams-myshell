#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <unistd.h>
#include <cstdlib>

int main() {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  const std::unordered_set<std::string> builtin = {"cd","echo","exit","pwd","type"};

  std::string line;

  while (true) {
    std::cout << "$ ";
    std::getline(std::cin, line);

    std::stringstream ss(line);
    std::string command;
    ss >> command;
    if(command == "cd"){
       std::string dir;
       ss >> dir;
       if(dir == "~") chdir(getenv("HOME"));
       else if(access(dir.c_str(), F_OK) == 0) chdir(dir.c_str());
       else std::cout << "cd: " << dir << ": No such file or directory" << "\n";
    }
    else if (command == "echo") {
      std::string word;
      bool first = true;

      while (ss >> word) {
        if (!first) std::cout << " ";
        std::cout << word;
        first = false;
      }
      std::cout << "\n";
    }

    else if (command == "exit") {
      break;
    }

    else if(command=="pwd"){
      char buffer[1024];
      if (getcwd(buffer, sizeof(buffer)) != NULL) {
          std::cout << buffer << std::endl;
      }
    }

    else if (command == "type") {

      std::string check_command;
      ss >> check_command;

      if (builtin.count(check_command)) {
        std::cout << check_command << " is a shell builtin\n";
        continue;
      }

      bool found = false;

      const char* path_env = std::getenv("PATH");
      if (path_env) {

        std::stringstream ss_path(path_env);
        std::string path;

        while (std::getline(ss_path, path, ':')) {

          std::string full_path = path + "/" + check_command;

          if (access(full_path.c_str(), X_OK) == 0) {
            std::cout << check_command << " is " << full_path << "\n";
            found = true;
            break;
          }
        }
      }

      if (!found) {
        std::cout << check_command << ": not found\n";
      }
    }

    else {

      bool found = false;

      const char* path_env = std::getenv("PATH");

      if (path_env) {
        std::stringstream ss_path(path_env);
        std::string path;

        while (std::getline(ss_path, path, ':')) {

          std::string full_path = path + "/" + command;

          if (access(full_path.c_str(), X_OK) == 0) {
            std::system(line.c_str()); 
            found = true;
            break;
          }
        }
      }

      if (!found) {
        std::cout << command << ": command not found\n";
      }
    }
  }
}