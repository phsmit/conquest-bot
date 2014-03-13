#ifndef CONQUEST_STRATEGY_MANAGER_H_
#define CONQUEST_STRATEGY_MANAGER_H_

#include "datatypes.h"
#include "strategy.h"
#include "gamedata.h"
#include <algorithm>
#include <iostream>

namespace {
int count_armies(PlacementVector &pv) {
  int armies = 0;
  for (PlacementVector::iterator it = pv.begin(); it != pv.end(); ++it) {
    armies += it->amount;
  }
  return armies;
}

bool compareStrategy(const Strategy *s1, const Strategy *s2) {
  return s1->get_priority() > s2->get_priority();
}
}
class StrategyManager {
public:
  std::vector<Strategy *> strategies;
  GameData &bot;
  bool initialized;

  int turn;

  int avail_armies;

  StrategyManager(GameData &bot): bot(bot) {
    initialized = false;
  };

  ~StrategyManager() {
    for (std::vector<Strategy *>::iterator it = strategies.begin(); it != strategies.end(); ++it) {
      delete *it;
    }
  }

  void init() {
    for (int super_region = 0; super_region < bot.super_n; ++super_region) {
      strategies.push_back(new FootholdStrategy(bot, super_region));
      strategies.push_back(new AquireContinentStrategy(bot, super_region));
      strategies.push_back(new DefendContinentStrategy(bot, super_region));
    }
    strategies.push_back(new KillAllEnemiesStrategy(bot));
    strategies.push_back(new BasicStrategy(bot));
    strategies.push_back(new DefenseStrategy(bot));
    initialized = true;
  }

  void update_strategies() {
    if (!initialized) init();
    for (std::vector<Strategy *>::iterator it = strategies.begin(); it != strategies.end(); ++it) {
      (*it)->update();
    }

    std::sort(strategies.begin(), strategies.end(), compareStrategy);
  }

  PlacementVector place_armies() {
    std::cerr << "Round " << turn << std::endl;
    update_strategies();

    PlacementVector pv;
    int armies_available = avail_armies;

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
    std::vector<int> army_surplus(bot.region_n, 0);
    for (int r = 0; r < bot.region_n; ++r) {
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

  bool finished() {
    return false;
  }

  void process_updates(UpdateVector2 updates) {
    std::vector<bool> updated(bot.region_n, false);
    for (UpdateVector2::iterator it = updates.begin(); it != updates.end(); ++it) {
      bot.owner[it->region] = it->player;
      bot.occupancy[it->region] = it->amount;
      updated[it->region] = true;
    }

    for (int r = 0; r < bot.region_n; ++r) {
      if (bot.owner[r] != ME) continue;
      if (!updated[r]) {
        bot.owner[r] = OTHER;
        bot.occupancy[r] = 2;
      }
    }
  }

  void process_opponent_moves(MoveVector moves) {
//TODO implement
  }

  void start_new_round(int armies) {
    turn++;
    avail_armies = armies;
    update_strategies();
  }

  std::vector<int> pick_starting_regions() {
    RegionVector rv;
    RegionVector regions = bot.init_regions;
    std::srand(regions[0] + 1); // plus 1 is to keep compatible with earlier versions
    std::random_shuffle(regions.begin(), regions.end());
    for (int i = 0; i < 6; ++i) {
      rv.push_back(regions[i]);
    }
    return rv;
  }
};

#endif // CONQUEST_STRATEGY_MANAGER_H_