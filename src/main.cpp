#include <iostream>
#include <string>
#include <unordered_set>

std::string trim(const std::string& s) {
    size_t l = s.find_first_not_of(" \t\n\r");
    if (l == std::string::npos) return "";
    size_t r = s.find_last_not_of(" \t\n\r");
    return s.substr(l, r - l + 1);
}

int main() {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::unordered_set<std::string>builtin({"echo","exit","type"});


  while(true){
      std::cout << "$ ";
      std::string command;
      std::getline(std::cin, command);
      command = trim(command);
      if(command == "exit") break;
      else if(command.substr(0,5)=="echo ") std::cout<<command.substr(5)<<"\n";
      else if(command.substr(0,5)=="type "){
        if(builtin.count(command.substr(5))) std::cout<<command.substr(5)<<" is a shell builtin"<<"\n";
        else std::cout<<command.substr(5)<<": not found"<<"\n";
      }
      else std::cout << command << ": command not found\n";
  }
}