#pragma once

#include "datatypes.h"
#include "util.h"

class State {
public:
  std::vector<Owner> owner;
  ArmyVector occupancy;
  std::vector<bool> visible;
  MoveVector opponent_moves;

  army_t place_armies;

  State() : place_armies(0) {}

  State(const State & s) : owner(s.owner),
                           occupancy(s.occupancy),
                           visible(s.visible),
                           opponent_moves(s.opponent_moves),
                           place_armies(s.place_armies) {}
};

class Plan {
public:
  std::vector<Owner> owner;
  ArmyVector occupancy;
  MoveVector moves;
  PlacementVector placements;

  ArmyVector available_armies;

  Plan(State s) : owner(s.owner), occupancy(s.occupancy), available_armies(owner.size(), 0) {
    for (auto region : range((reg_t)owner.size())) {
      if (owner[region] == ME && occupancy[region] > 1) {
        available_armies[region] = occupancy[region] - 1;
      }
    }
  }
};

class GameData {
public:
  const ArmyVector super_award;
  const RegionVector region_super;
  const std::vector<RegionVector> regions_by_super;

  const std::vector<std::vector<bool> > neighbours;
  const std::vector<RegionVector> neighbour_ids;
  const std::vector<std::vector<int> > distances;

  const reg_t super_n;
  const reg_t region_n;

  const RegionVector init_regions;

  std::vector<State> states;
  std::vector<Plan> plans;

  unsigned round;

  GameData(CanonicalGameSetup setup);

  army_t get_enemy_neighbour_armies(reg_t region) const;

  bool has_enemy_neighbours(reg_t region) const;

  army_t count_neighbour_armies(reg_t region, Owner owner) const;

  State & cur_state() {
    return states[round];
  }

  Plan & cur_plan() {
    return plans[round];
  }

  void init() {
    states.clear();
    plans.clear();
    round = 0;
    State s1;
    s1.owner = std::vector<Owner>(region_n, NEUTRAL);
    s1.occupancy = ArmyVector(region_n, 2);
    s1.visible = std::vector<bool>(region_n, false);
    s1.place_armies = 0;
    states.push_back(s1);
    plans.push_back(Plan(s1));
  }

  void start_round(UpdateVector updates, MoveVector opponent_moves, army_t place_armies) {
    State new_state(states[round]);

    std::fill(new_state.visible.begin(), new_state.visible.end(), false);

    new_state.place_armies = place_armies;
    new_state.opponent_moves = opponent_moves;

    // Process updates
    for (auto update : updates) {
      new_state.owner[update.region] = update.player;
      new_state.occupancy[update.region] = update.amount;
      new_state.visible[update.region] = true;
    }

    // If we didn't get an update of one of our own regions, we lost it to OTHER.
    for (auto region : range(region_n)) {
      if (!new_state.visible[region] && new_state.owner[region] == ME) {
        new_state.owner[region] = OTHER;
        // The occupancy is an estimate, TODO could be better using opponent_moves?
        new_state.occupancy[region] = 2;
      }
    }

    round++;
    states.push_back(new_state);
    plans.push_back(Plan(new_state));
  }
};