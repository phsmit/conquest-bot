#include "prob_math.h"
#include "gtest/gtest.h"

#include <boost/math/special_functions/gamma.hpp>

#define MARGIN 0.001
namespace {
using namespace conquest::internal;


TEST(WinProb, lessattackers) {
  EXPECT_NEAR(0.0, get_win_prob(1, 2), MARGIN);
}

TEST(WinProb, muchmoreattackers) {
  EXPECT_NEAR(1.0, get_win_prob(10000, 1), MARGIN);
}
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}