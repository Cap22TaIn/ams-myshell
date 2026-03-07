#include <iostream>
#include <string>

std::string trim(const std::string& s) {
    size_t l = s.find_first_not_of(" \t\n\r");
    if (l == std::string::npos) return "";
    size_t r = s.find_last_not_of(" \t\n\r");
    return s.substr(l, r - l + 1);
}

int main() {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while(true){
      std::cout << "$ ";
      std::string command;
      std::getline(std::cin, command);
      trim(command);
      if(command == "exit") break;
      else if(command.substr(0,5)=="echo ") std::cout<<command.substr(5)<<"\n";
      else std::cout << command << ": command not found\n";
  }
}