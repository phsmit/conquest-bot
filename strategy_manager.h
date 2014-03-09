#ifndef CONQUEST_STRATEGY_MANAGER_H_
#define CONQUEST_STRATEGY_MANAGER_H_

#include "strategy.h"

class StrategyManager {
public:
  std::vector<Strategy *> strategies;
  SavingBaseBot &bot;

  StrategyManager(SavingBaseBot &bot): bot(bot) {
    for (int super_region = 0; super_region < bot.super_region_ids.size(); ++super_region) {
      strategies.push_back(new FootholdStrategy(bot, super_region));
      strategies.push_back(new AquireContinentStrategy(bot, super_region));
      strategies.push_back(new DefendContinentStrategy(bot, super_region));
    }
    strategies.push_back(new KillAllEnemiesStrategy(bot));
    strategies.push_back(new BasicStrategy(bot));
  };

  ~StrategyManager() {
    for (std::vector<Strategy *>::iterator it = strategies.begin(); it != strategies.end(); ++it) {
      delete *it;
    }
  }

  void update_strategies() {
    for (std::vector<Strategy *>::iterator it = strategies.begin(); it != strategies.end(); ++it) {
      (*it)->update();
    }
  }

  PlacementVector place_armies() {
    PlacementVector pv;
    for (int s = 0; s < strategies.size(); ++s) {
      if (strategies[s]->active()) {
        PlacementVector ret = strategies[s]->place_armies(bot.place_armies);
        pv.insert(pv.end(), ret.begin(), ret.end());
        break;
      }
    }
    return pv;
  }

  MoveVector make_moves() {
    MoveVector mv;
    for (int s = 0; s < strategies.size(); ++s) {
      if (strategies[s]->active()) {
        MoveVector ret = strategies[s]->do_moves();
        mv.insert(mv.end(), ret.begin(), ret.end());
      }
    }
    return mv;
  }
};

#endif // CONQUEST_STRATEGY_MANAGER_H_