//
//  solver.hpp
//  DPLL-Solver
//
//  Created by Alex Makhov on 15.05.2023.
//

#ifndef solver_hpp
#define solver_hpp

#pragma once

#include "fileworks.hpp"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include <stack>
#include <ctime>

enum State {
  FALSE = 0,
  TRUE = 1,
  UNDEF = 2,
  CHECKPOINT = 3
};

struct Stack {
  int item;
  std::vector<int> changed;
  State state;
};

typedef std::vector<std::vector<int>> Matrix;

int dpll(std::vector<std::string> files, double ttl);

#endif /* solver_hpp */
