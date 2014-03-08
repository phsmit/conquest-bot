#ifndef CONQUEST_STRATEGY_MANAGER_H_
#define CONQUEST_STRATEGY_MANAGER_H_

#include "strategy.h"

class StrategyManager {
public:
  std::vector<Strategy*> strategies;
  StrategyManager(SavingBaseBot &bot) {
    for (int super_region = 0; super_region < bot.super_region_ids.size(); ++super_region) {
      strategies.push_back(new FootholdStrategy(bot, super_region));
      strategies.push_back(new AquireContinentStrategy(bot, super_region));
      strategies.push_back(new DefendContinentStrategy(bot, super_region));
    }
    strategies.push_back(new KillAllEnemiesStrategy(bot));
  };

  ~StrategyManager() {
    for (std::vector<Strategy*>::iterator it = strategies.begin(); it != strategies.end(); ++it) {
      delete *it;
    }
  }

  void update_strategies() {
    for (std::vector<Strategy*>::iterator it = strategies.begin(); it != strategies.end(); ++it) {
      (*it)->update();
    }
  }
};

#endif // CONQUEST_STRATEGY_MANAGER_H_