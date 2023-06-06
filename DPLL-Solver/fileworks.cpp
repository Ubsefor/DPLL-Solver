//
//  fileworks.cpp
//  DPLL-Solver
//
//  Created by Alex Makhov on 15.05.2023.
//

#include "fileworks.hpp"

static bool ends_with(const std::string& str, const std::string& suffix)
{
  return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

static bool starts_with(const std::string& str, const std::string& prefix)
{
  return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

bool check_file(std::string path){
  if (fs::exists(path) &&
      fs::is_regular_file(path) &&
      ends_with(path, ".cnf") &&
      !starts_with(path.substr(path.find_last_of("/\\") + 1), "."))
    return true;
  return false;
};

std::vector<std::string> collect_tests(std::string dir){
  std::vector<std::string> files;

  for (const auto& topDir : fs::directory_iterator(dir)){
    if (!fs::is_directory(topDir)) {
      if (fs::is_regular_file(topDir.path()) && check_file(topDir.path()))
        files.push_back(topDir.path());
      continue;
    };
    for (const auto& entry : fs::directory_iterator(topDir)){
      if (check_file(entry.path()))
        files.push_back(entry.path());
    }
  }
  return files;
};

std::ifstream get_file(std::string path){
  return std::ifstream (path);
}

std::stringstream read_dimacs(std::ifstream &fi, std::string &line) {
  while (std::getline(fi, line) && line[0] != 'p');

  return std::stringstream(line);
}
