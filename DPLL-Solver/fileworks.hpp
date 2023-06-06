//
//  fileworks.hpp
//  DPLL-Solver
//
//  Created by Alex Makhov on 15.05.2023.
//

#ifndef fileworks_hpp
#define fileworks_hpp

#include <iostream>
#include <filesystem>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

namespace fs = std::filesystem;

std::stringstream read_dimacs(std::ifstream &fi, std::string &line);
std::vector<std::string> collect_tests(std::string dir);

#endif /* fileworks_hpp */
