#include "strategy_bot.h"

#include <cstdlib>
#include <algorithm>

PlacementVector StrategyBot::cmd_go_place_armies(long t) {
  sm.update_strategies();
  return sm.place_armies();
}

MoveVector StrategyBot::cmd_go_attacktransfer(long t) {
  return sm.make_moves();
}

RegionVector StrategyBot::cmd_pick_starting_regions(long t, RegionVector regions) {
  RegionVector rv;
  std::srand(regions[0]);
  std::random_shuffle(regions.begin(), regions.end());
  for (int i = 0; i < 6; ++i) {
    rv.push_back(regions[i]);
  }
  return rv;
}

