#include "basic_attack_bot.h"

#include <algorithm>
#include "prob_math.h"

namespace {
int get_num_neighbour_enemies(int region) {
  return 0;
}


}

void BasicAttackBot::cmd_setupmap_superregions(SettingVector regions) {
  // Is this the best place to do this?
  owner_map["neutral"] = NEUTRAL;
  owner_map[name_me] = ME;
  owner_map[name_opponent] = OTHER;

  super_region_ids.clear();
  super_region_reward.clear();

  for (SettingVector::iterator it = regions.begin(); it != regions.end(); ++it) {
    super_region_ids.push_back(it->first);
    super_region_reward.push_back(it->second);
  }
}

void BasicAttackBot::cmd_setupmap_regions(SettingVector regions) {
  region_ids.clear();
  region_super_region.clear();
  region_map.clear();

  std::map<int, int> super_region_map;
  for (size_t i = 0; i != super_region_ids.size(); ++i) {
    super_region_map[super_region_ids[i]] = i;
  }

  for (size_t i = 0; i != regions.size(); ++i) {
    region_ids.push_back(regions[i].first);
    region_super_region.push_back(super_region_map[regions[i].second]);
    region_map[regions[i].first] = i;

    owner.push_back(NEUTRAL);
    occupancy.push_back(2);
  }
}

void BasicAttackBot::cmd_setupmap_neighbours(ConnectionVector connections) {
  std::vector<bool> neighbours_row(region_ids.size(), false);
  neighbours = std::vector<std::vector<bool> >(region_ids.size(), neighbours_row);

  for (ConnectionVector::iterator it = connections.begin(); it != connections.end(); ++it) {
    for (std::vector<int>::iterator it2 = it->to.begin(); it2 != it->to.end(); ++it2) {
      int p1 = region_map[it->from];
      int p2 = region_map[*it2];

      neighbours[p1][p2] = true;
      neighbours[p2][p1] = true;
    }
  }
}

void BasicAttackBot::cmd_updatemap(UpdateVector updates) {
  for (UpdateVector::iterator it = updates.begin(); it != updates.end(); ++it) {
    owner[it->region] = owner_map[it->player];
    occupancy[it->region] = it->amount;
  }
}

void BasicAttackBot::cmd_opponentmoves(MoveVector moves) {

}

PlacementVector BasicAttackBot::cmd_go_place_armies(long t) {
  //place armies on one region, the one with the most neighbour armies
  int max_neighbour_armies = 0;
  int max_region = 0;

  for (size_t r = 0; r < region_ids.size(); ++r) {
    if (owner[r] != ME) continue;
    int army_neighbours = get_enemy_neighbour_armies(r);
    if (army_neighbours > max_neighbour_armies) {
      max_region = r;
      max_neighbour_armies = army_neighbours;
    }
  }
  Placement p = {max_region, place_armies};
  PlacementVector pv;
  pv.push_back(p);

  // add the armies to occupancy
  occupancy[max_region] += place_armies;
  return pv;
}

MoveVector BasicAttackBot::cmd_go_attacktransfer(long t) {
  // Every region that has no unconquered neighbours: move puppets in random directions
  // Every region that has unconquered neighours, do the biggest attack that has 95% win change

  MoveVector moves;

  for (size_t r = 0; r < region_ids.size(); ++r) {
    if (owner[r] != ME) continue;
    if (occupancy[r] < 2) continue;

    if (has_enemy_neighbours(r)) {
      int best_attack_region = -1;
      int best_num_enemies = 0;
      for (size_t enemy_region = 0; enemy_region < region_ids.size(); ++enemy_region) {
        if (!neighbours[r][enemy_region]) continue;
        if (owner[enemy_region] == ME) continue;
        if (conquest::internal::get_win_prob(occupancy[r] - 1, occupancy[enemy_region]) > 0.8) {
          if (occupancy[enemy_region] > best_num_enemies) {
            best_num_enemies = occupancy[enemy_region];
            best_attack_region = enemy_region;
          }
        }
      }

      if (best_attack_region >= 0) {
        Move m = {r, best_attack_region, occupancy[r] - 1};
        moves.push_back(m);
      }
    } else {
      std::vector<int> options;
      std::vector<int> good_options;
      for (size_t neigh_region = 0; neigh_region < region_ids.size(); ++neigh_region) {
        if (!neighbours[r][neigh_region]) break;
        options.push_back(neigh_region);
        if (has_enemy_neighbours(neigh_region)) {
          good_options.push_back(neigh_region);
        }
      }

      std::random_shuffle(good_options.begin(), good_options.end());
      std::random_shuffle(options.begin(), options.end());

      if (good_options.size() > 0) {
        Move m = {r, good_options[0], occupancy[r] - 1};
        moves.push_back(m);
      } else {
        Move m = {r, options[0], occupancy[r] - 1};
        moves.push_back(m);
      }

    }
  }
  return moves;
}

RegionVector BasicAttackBot::cmd_pick_starting_regions(long t, RegionVector regions) {
  RegionVector rv;
  for (int i = 0; i < 6; ++i) {
    rv.push_back(regions[i]);
  }
  return rv;

}

int BasicAttackBot::get_enemy_neighbour_armies(int region) {
  int num_neighbours = 0;
  for (size_t r = 0; r < region_ids.size(); ++r) {
    if (!neighbours[region][r]) continue;
    if (owner[r] == ME) continue;
    num_neighbours += occupancy[r];
  }
  return num_neighbours;
}

bool BasicAttackBot::has_enemy_neighbours(int region) {
  for (size_t r = 0; r < region_ids.size(); ++r) {
    if (!neighbours[region][r]) continue;
    if (owner[r] != ME) return true;
  }
  return false;
}
