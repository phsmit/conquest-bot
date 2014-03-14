#ifndef CONQUEST_STRATEGY_MANAGER_H_
#define CONQUEST_STRATEGY_MANAGER_H_

#include "datatypes.h"
#include "gamedata.h"
#include "strategy.h"
#include "util.h"
#include <algorithm>
#include <iostream>

namespace {
army_t count_armies(PlacementVector &pv) {
  army_t armies = 0;
  for (auto p : pv) {
    armies += p.amount;
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
  GameData &data;
  bool initialized;

  int turn;

  army_t avail_armies;

  StrategyManager(GameData &data): data(data), turn(0) {
    initialized = false;
  };

  ~StrategyManager() {
    for (auto strategy : strategies) {
      delete strategy;
    }
  }

  void init() {
    for (auto super_region : make_range(data.super_n)) {
      strategies.push_back(new FootholdStrategy(data, super_region));
      strategies.push_back(new AquireContinentStrategy(data, super_region));
      strategies.push_back(new DefendContinentStrategy(data, super_region));
    }
    strategies.push_back(new KillAllEnemiesStrategy(data));
    strategies.push_back(new BasicStrategy(data));
    strategies.push_back(new DefenseStrategy(data));
    initialized = true;
  }

  void update_strategies() {
    if (!initialized) init();
    for (auto strategy : strategies) {
      strategy->update();
    }

    std::sort(strategies.begin(), strategies.end(), compareStrategy);
  }

  PlacementVector place_armies() {
    std::cerr << "Round " << turn << std::endl;
//    update_strategies();

    PlacementVector pv;
    army_t armies_available = avail_armies;

    //for (size_t s = 0; s < strategies.size(); ++s) {
    for (auto strategy : strategies) {
      if (strategy->active()) {
        army_t need = strategy->armies_needed();
        std::cerr << strategy->name << " active. Army need: " << need << std::endl;

        PlacementVector ret = strategy->place_armies(std::min(need, armies_available));
        armies_available -= count_armies(ret);
        pv.insert(pv.end(), ret.begin(), ret.end());
      }
    }
    return pv;
  }

  MoveVector make_moves() {
    std::vector<army_t> army_surplus(data.region_n, 0);
    for (auto r : make_range(data.region_n)) {
      if (data.owner[r] != ME) continue;
      army_surplus[r] = data.occupancy[r] - 1;
    }

    MoveVector mv;
    for (auto strategy : strategies) {
      if (strategy->active()) {
        MoveVector ret = strategy->do_moves(army_surplus);
        mv.insert(mv.end(), ret.begin(), ret.end());
      }
    }
    return mv;
  }

  bool finished() {
    return false;
  }

  void process_updates(UpdateVector updates) {
    data.visible = std::vector<bool>(data.region_n, false);
    for (UpdateVector::iterator it = updates.begin(); it != updates.end(); ++it) {
      data.owner[it->region] = it->player;
      data.occupancy[it->region] = it->amount;
      data.visible[it->region] = true;
    }

    for (reg_t r = 0; r < data.region_n; ++r) {
      if (data.owner[r] != ME) continue;
      if (!data.visible[r]) {
        data.owner[r] = OTHER;
        data.occupancy[r] = 2;
      }
    }
  }

  void process_opponent_moves(MoveVector moves) {
//TODO implement
  }

  void start_new_round(army_t armies) {
    turn++;
    avail_armies = armies;
    update_strategies();
  }

  RegionVector pick_starting_regions() {
    RegionVector rv;
    RegionVector regions = data.init_regions;
    std::srand(regions[0] + 1); // plus 1 is to keep compatible with earlier versions
    std::random_shuffle(regions.begin(), regions.end());
    for (int i = 0; i < 6; ++i) {
      rv.push_back(regions[i]);
    }
    return rv;
  }
};

#endif // CONQUEST_STRATEGY_MANAGER_H_