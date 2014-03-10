
#include <vector>
#include <algorithm>
#include <iostream>

class Strategy {
public:
  int priority;
  int get_priority() const {
    return priority;
  }
};

namespace {
bool compareStrategy(const Strategy * s1, const Strategy * s2) {
  return s1->get_priority() > s2->get_priority();
}
}



int main(int argc, char **argv) {
  std::vector<Strategy *> strategies;
  strategies.push_back(new Strategy);
  strategies.push_back(new Strategy);
  strategies.push_back(new Strategy);

  strategies[1]->priority = 1;

  std::sort(strategies.begin(), strategies.end(), compareStrategy);

  for (int i = 0; i < strategies.size(); ++i) {
    std::cout << strategies[i]->get_priority() << std::endl;
  }

}