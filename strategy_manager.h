#ifndef CONQUEST_STRATEGY_MANAGER_H_
#define CONQUEST_STRATEGY_MANAGER_H_

#include "strategy.h"
#include <algorithm>
#include <iostream>

namespace {
int count_armies(PlacementVector& pv) {
  int armies = 0;
  for (PlacementVector::iterator it = pv.begin(); it != pv.end(); ++it) {
    armies += it->amount;
  }
  return armies;
}

bool compareStrategy(const Strategy * s1, const Strategy * s2) {
  return s1->get_priority() > s2->get_priority();
}
}
class StrategyManager {
public:
  std::vector<Strategy *> strategies;
  SavingBaseBot &bot;
  bool initialized;

  StrategyManager(SavingBaseBot &bot): bot(bot) {
    initialized  = false;
  };

  ~StrategyManager() {
    for (std::vector<Strategy *>::iterator it = strategies.begin(); it != strategies.end(); ++it) {
      delete *it;
    }
  }

  void init() {
    for (int super_region = 0; super_region < bot.super_region_ids.size(); ++super_region) {
      strategies.push_back(new FootholdStrategy(bot, super_region));
      strategies.push_back(new AquireContinentStrategy(bot, super_region));
      strategies.push_back(new DefendContinentStrategy(bot, super_region));
    }
    strategies.push_back(new KillAllEnemiesStrategy(bot));
    strategies.push_back(new BasicStrategy(bot));
    initialized = true;
  }

  void update_strategies() {
    if(!initialized) init();
    for (std::vector<Strategy *>::iterator it = strategies.begin(); it != strategies.end(); ++it) {
      (*it)->update();
    }

    std::sort(strategies.begin(), strategies.end(), compareStrategy);
  }

  PlacementVector place_armies() {
    std::cerr << "Round " << bot.turn << std::endl;
    update_strategies();

    PlacementVector pv;
    int armies_available = bot.place_armies;

    for (int s = 0; s < strategies.size(); ++s) {
      if (strategies[s]->active()) {
        int need = strategies[s]->armies_needed();
        std::cerr << strategies[s]->name << " active. Army need: " << need << std::endl;

        PlacementVector ret = strategies[s]->place_armies(std::min(need, armies_available));
        armies_available -= count_armies(ret);
        pv.insert(pv.end(), ret.begin(), ret.end());
      }
    }
    return pv;
  }

  MoveVector make_moves() {
    std::vector<int> army_surplus(bot.region_ids.size(), 0);
    for (int r = 0; r < bot.region_ids.size(); ++r) {
      if (bot.owner[r] != ME) continue;
      army_surplus[r] = bot.occupancy[r] - 1;
    }

    MoveVector mv;
    for (int s = 0; s < strategies.size(); ++s) {
      if (strategies[s]->active()) {
        MoveVector ret = strategies[s]->do_moves(army_surplus);
        mv.insert(mv.end(), ret.begin(), ret.end());
      }
    }
    return mv;
  }
};

#endif // CONQUEST_STRATEGY_MANAGER_H_