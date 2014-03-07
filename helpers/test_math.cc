#include "prob_math.h"
#include "gtest/gtest.h"

#define MARGIN 0.001
namespace {
using namespace conquest::internal;

TEST(KillExactlyN, test1) {
  EXPECT_NEAR(0.4, kill_exactly_n(1, 0, 0.6), MARGIN);
}

TEST(KillExactlyN, test2) {
  EXPECT_NEAR(0.6, kill_exactly_n(1, 1, 0.6), MARGIN);
}

TEST(KillExactlyN, SumsToOne) {
  EXPECT_NEAR(1.0, kill_exactly_n(3, 0, 0.6) + kill_exactly_n(3, 1, 0.6) + kill_exactly_n(3, 2, 0.6) + kill_exactly_n(3, 3, 0.6), MARGIN);
}

TEST(KillExactlyN, SumsToOne2) {
  EXPECT_NEAR(1.0, kill_exactly_n(1, 0, 0.6) + kill_exactly_n(1, 1, 0.6), MARGIN);
}

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