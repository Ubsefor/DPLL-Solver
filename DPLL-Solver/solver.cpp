  //
  //  solver.cpp
  //  DPLL-Solver
  //
  //  Created by Alex Makhov on 15.05.2023.
  //

#include "solver.hpp"

std::stack<Stack> dpllStack;

namespace fs = std::filesystem;

#ifdef DEBUG

static void debug_print_stack_changed() {
  std::cout << dpllStack.top().item << " changed: ";

  for (auto i : dpllStack.top().changed) {
    std::cout << i << " ";
  }

  std::cout << std::endl;
}

static void debug_print_stack_top() {

  switch (dpllStack.top().state) {
    case State::TRUE:
      std::cout << "TRUE" << std::endl;
      break;

    case State::FALSE:
      std::cout << "FALSE" << std::endl;
      break;

    case State::UNDEF:
      std::cout << "UNDEF" << std::endl;
      break;

    default:
      std::cout << "[CHECKPOINT] FALSE" << std::endl;
      break;
  }
}

static void debug_print_matrix(const Matrix &mat) {
  std::cout << std::endl;
  int i = 0;
  for (auto m : mat) {
    std::cout << i << " row: ";

    for (auto n : m)
      std::cout << n << "\t";

    i++;

    std::cout << std::endl;
  }
  std::cout << std::endl;
}

static void debug_print_states(std::vector<State> &states) {
  std::cout << std::endl;
  for (int i = 1; i < (int) states.size(); ++i)
    std::cout << "state: " << i << " " << states[i] << std::endl;
  std::cout << std::endl;
}

#endif /* DEBUG */

State get_state(const int &value, const std::vector<State> &states) {
  int val = abs(value);

  if (states[val] == State::UNDEF)
    return State::UNDEF;

  if (value < 0)
    return states[val] == State::TRUE ? State::FALSE : State::TRUE;

  return states[val];
}

static void populate_clauses(Matrix &clauses, std::ifstream &fi, std::string &line, int &lines_num) {
  while (lines_num != 0) {

    std::getline(fi, line);
    std::stringstream lineS(line);

    std::vector<int> clause;
    int var;
    while(lineS >> var) {
      if (var == 0)
        break;

      clause.push_back(var);
    }
    clauses.push_back(clause);
    lines_num--;
  }
}

Matrix process_file(std::string path, Matrix &clauses, std::vector<State> &states) {
  int vars_num;
  int lines_num;
  std::string tmp;

  std::ifstream fi = std::ifstream(path);
  std::string line;
  std::stringstream header = read_dimacs(fi, line);

    // dimacs file header
  header >> tmp >> tmp >> vars_num >> lines_num;

    // + 1 to states as we count from 1, unlike in header
  states = std::vector<State> (vars_num + 1, State::UNDEF);

    // set first checkpoint to return to
  states[0] = State::CHECKPOINT;

  populate_clauses(clauses, fi, line, lines_num);

  fi.close();
  return clauses;
}

int get_single_form(const std::vector<int> &clauses, const std::vector<State> &states) {
  int item_num = 0;
  int res = 0;

  for (int clause : clauses) {

    State c_state = get_state(clause, states);

    if (c_state == State::TRUE) {
      return 0;
    } else if (c_state == State::UNDEF) {
      ++item_num;
      res = clause;
    }

    if (item_num > 1)
      return 0;
  }

  return res;
}

void update_states(int &elem, std::vector<State> &states) {
  bool stackEmpty = dpllStack.empty();

  if (elem > 0)
    states[abs(elem)] = State::TRUE;

  else
    states[abs(elem)] = State::FALSE;

  if (!stackEmpty)
    dpllStack.top().changed.push_back(elem);
}

bool check_true(const Matrix &clauses, std::vector<State> &states) {

    // search for true clause in row
  for (const auto &clause : clauses) {

    bool flag = false;

    for (auto item : clause) {
      flag = get_state(item, states) == State::TRUE;
      if (flag) break;
    }

    if (!flag)
      return false;
  }

  return true;
}

void set_first_true(std::vector<State> &states) {
  for (int i = 1; i < (int) states.size(); ++i) {
    if (get_state(i, states) == State::UNDEF) {
      dpllStack.push({i, {}, State::TRUE});
      states[abs(i)] = State::TRUE;
      return;
    }
  }
}

void random_true(std::vector<State> &states) {

  std::vector<int> undefs;

  for (unsigned int i = 1; i < states.size(); ++i) {
    if (states[i] == State::UNDEF)
      undefs.push_back(i);
  }

  std::srand((double) std::time(nullptr));
  int val = undefs[std::rand() % undefs.size()];

  dpllStack.push({val, {}, State::TRUE});

  states[abs(val)] = State::TRUE;
}

bool check_revert(const Matrix &clauses, std::vector<State> &states) {
  for (const auto &clause : clauses) {

    bool flag = false;

    for (auto &state : clause)
      if (get_state(state, states) != State::FALSE) {
        flag = true;
        break;
      }

    if (!flag)
      return true;
  }
  return false;
}

  // revert to previous "checkpoint", if all states are FALSE
void revert(std::vector<State> &states) {
  while (true) {
    for (auto &v : dpllStack.top().changed)
      states[abs(v)] = State::UNDEF;

    dpllStack.top().changed.clear();
//    debug_print_stack_top();

    if (dpllStack.top().state == State::TRUE) {

      dpllStack.top().state = State::FALSE;

      states[abs(dpllStack.top().item)] = State::FALSE;
      break;

    } else {

      states[abs(dpllStack.top().item)] = State::UNDEF;
      dpllStack.pop();

      if (dpllStack.empty())
        break;
    }
  }
}

State try_solve(Matrix &clauses, std::vector<State> &states, double ttl) {
  clock_t run = clock();
  clock_t tl = clock();
  clock_t ttl_ticks = CLOCKS_PER_SEC * ttl;
  while (true) {

      // time to live deadline
    tl = clock();
    if ((tl - run) >= ttl_ticks)
      return State::UNDEF;

    for (auto& s : clauses) {
      int elem = get_single_form(s, states);
      if (elem != 0)
        update_states(elem, states);
    }

    if (check_revert(clauses, states)) {
      if (!dpllStack.empty()) {
//        debug_print_stack_top();
        revert(states);
//        debug_print_stack_changed();
      }
      if (dpllStack.empty())
        return State::FALSE;
    } else {
      if (check_true(clauses, states))
        return State::TRUE;

//      random_true(states);
      set_first_true(states);
//      debug_print_states(states);
    }
  }
  return State::UNDEF;
}

int dpll(std::vector<std::string> files, double ttl){

  for (int i = 0; i < (int) files.size(); ++i) {

      // empty any previous runs' stack
    while (!dpllStack.empty()) {
      dpllStack.pop();
    }

    std::cout << "Run test " << i + 1 <<  ", cnf file: " << files[i] << std::endl;

    Matrix clauses;
    std::vector<State> states;

    process_file(files[i], clauses, states);

    State verdict; // reusing State as it is convenient

    clock_t start = clock();

    verdict = try_solve(clauses, states, ttl);

    clock_t end = clock();

    switch (verdict) {
      case State::TRUE:
        std::cout << "SAT" << std::endl;
        break;

      case State::FALSE:
        std::cout << "UNSAT" << std::endl;
        break;

      default:
        std::cout << "UNKNOWN" << std::endl;
        break;
    }

    double time = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "Test " << i + 1 << " time: " << time << "s" << std::endl << std::endl;
  }
  return 0;
}
