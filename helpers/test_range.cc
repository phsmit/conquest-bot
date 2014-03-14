#include "datatypes.h"
#include "util.h"

#include <iostream>

int main(int argc, char **argv) {
  reg_t b = 12;
  for (auto i : range(b)) {
    std::cout << sizeof(i) << std::endl;
    std::cout << i << std::endl;
  }
  return 0;
}