#include "prob_math.h"

#include <iostream>
#include <iomanip>

int main(int argc, const char *argv[]) {
  int dim = 40;
  using namespace std;
  cout << "    | ";
  for (int i = 1; i < dim; ++i) {
    cout << "   " << std::setw(2) << i << " | ";
  }
  cout << endl;

  cout << "----|-";

  for (int i = 1; i < dim; ++i) {
    cout << "------|-";
  }
  cout << endl;
  std::cout << std::fixed;



  for (int j = 1; j < dim; ++j) {
    cout << std::setw(3) << j << " | ";

    for (int i = 1; i < dim; ++i) {
      cout << setprecision(3) << conquest::internal::get_win_prob(j, i) << " | ";
    }
    cout << endl;
  }

  return 0;

}