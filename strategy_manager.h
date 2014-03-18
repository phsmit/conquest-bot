#pragma once

#include "datatypes.h"
#include "gamedata.h"
#include "rand.h"
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

bool compareStrategyTotalNeed(const Strategy *s1, const Strategy *s2) {
  return s1->total_army_need() < s2->total_army_need();
}

bool compareStrategyMovePrio(const Strategy *s1, const Strategy *s2) {
  return s1->get_move_priority() > s2->get_move_priority();
}

}
class StrategyManager {
public:
  GameData &data;
  randg_t rand_engine;

  std::vector<Strategy *> strategies;

  bool initialized;

  int turn;

  army_t avail_armies;

  StrategyManager(GameData &data, randg_t rand_engine): data(data), rand_engine(rand_engine), turn(0) {
    initialized = false;
  };

  ~StrategyManager() {
    for (auto strategy : strategies) {
      delete strategy;
    }
  }

  void init() {
    data.init();
    for (auto super_region : range(data.super_n)) {
//      strategies.push_back(new FootholdStrategy(data, super_region));
      strategies.push_back(new AquireContinentStrategy(data, super_region));
//      strategies.push_back(new DefendContinentStrategy(data, super_region));
    }
//    strategies.push_back(new KillAllEnemiesStrategy(data));
    strategies.push_back(new BasicStrategy(data));
    strategies.push_back(new DefenseStrategy(data));
    initialized = true;
  }

  PlacementVector place_armies() {
    data.plans[data.round].placements.clear();

    army_t armies_available = data.states[data.round].place_armies;

    std::sort(strategies.begin(), strategies.end(), compareStrategyTotalNeed);

    for (auto strategy : strategies) {
      if (strategy->is_active()) {
        army_t need = strategy->current_army_need();
        std::cerr << strategy->name << " Total: " << strategy->total_army_need() << ", Current: " << need;

        army_t armies_placed = strategy->place_armies2(std::min(need, armies_available));
        std::cerr << ", Actual: " << armies_placed << std::endl;
        armies_available -= armies_placed;
      }
    }

    return data.plans[data.round].placements;
  }

  MoveVector make_moves() {
    data.cur_plan().occupancy = ArmyVector(data.cur_state().occupancy);
    for (auto region : range(data.region_n)) {
      if (data.cur_state().owner[region] == ME && data.cur_plan().occupancy[region] > 1) {
        data.cur_plan().available_armies[region] = data.cur_plan().occupancy[region] - 1;
      }
    }

    std::sort(strategies.begin(), strategies.end(), compareStrategyMovePrio); // TODO change to move priority

    for (auto strategy : strategies) {
      if (strategy->is_active()) {
        unsigned moves_done = strategy->do_moves2();
        std::cerr << strategy->name << " Num Moves: " << moves_done << std::endl;
      }
    }
    return data.plans[data.round].moves;
  }

  bool finished() {
    return false;
  }

  RegionVector pick_starting_regions() {
    RegionVector rv;
    RegionVector regions = data.init_regions;
    std::shuffle(regions.begin(), regions.end(), rand_engine);
    for (int i = 0; i < 6; ++i) {
      rv.push_back(regions[i]);
    }
    return rv;
  }

  void start_round(UpdateVector updates, MoveVector opponent_moves, army_t place_armies) {
    if (!initialized) init();
    turn++;

    std::cerr << "Round " << turn << std::endl;

    data.start_round(updates, opponent_moves, place_armies);

    // Update all strategies to new round
    for (auto strategy : strategies) {
      strategy->start_round();
      strategy->update();
    }
  }


};
