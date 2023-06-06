  //
  //  main.cpp
  //  DPLL-Solver
  //
  //  Created by Alex Makhov on 15.05.2023.
  //

#include <iostream>
#include "fileworks.hpp"
#include "solver.hpp"
#include <pthread.h>
#include <string>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

  // ah, yes, a time limit for the solver
double ttl = 10;

std::string parse_testpath(std::vector<std::string> args, int argc){
  switch (argc){
    case 3:
      std::cout << "Setting TTL to " << args[2] << "s" << std::endl;
      ttl = (double) std::atof(args[2].c_str());
    case 2:
      std::cout << "Looking for tests to run in " << args[1] << std::endl;
      return args[1];
      break;
      
    default:
      std::cout << "Usage: ./executable <path to dir with tests> <time limit on 1 test>" << std::endl;
      std::cout << "Looking for ./tests with time limit = 10 seconds" << std::endl;
      return fs::current_path().string() + "/tests";
      break;
  }
}

int main(int argc, const char * argv[]) {
  
  std::vector<std::string> args(argv, argv+argc);
  for (int i = 0; i < argc; i++){
    args.push_back(argv[i]);
  }
  
  std::string path = parse_testpath(args, argc);
  
  if (!fs::is_directory(path)){
    std::cout << "No such directory: " << path << std::endl;
    return 1;
  }
  
  std::vector<std::string> files = collect_tests(path);
  
  dpll(files, ttl);
  
  return 0;
}
