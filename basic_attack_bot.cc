#include "basic_attack_bot.h"

#include <cstdlib>
#include <algorithm>
#include "prob_math.h"

namespace {
bool element_in_vector(int element, std::vector<int> v) {
  for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    if (element == *it) return true;
  }
  return false;
}
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
  Placement p = {region_ids[max_region], place_armies};
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
      MoveVector attack_moves = generate_attacks(r);
      moves.insert(moves.end(), attack_moves.begin(), attack_moves.end());
    } else {
      int closest_enemy_region = -1;
      int enemy_distance = region_ids.size();
      for (size_t possible_region = 0; possible_region < region_ids.size(); ++possible_region) {
        if (owner[r] == ME) continue;
        if (distances[r][possible_region] < closest_enemy_region) {
          closest_enemy_region = possible_region;
          enemy_distance = distances[r][possible_region];
        }
      }

      int target_region = -1;
      for (size_t neigh_region = 0; neigh_region < region_ids.size(); ++neigh_region) {
        if (!neighbours[r][neigh_region]) continue;
        if (distances[neigh_region][closest_enemy_region] < enemy_distance) {
          target_region = neigh_region;
          break;
        }
      }

      Move m = {region_ids[r], region_ids[target_region], occupancy[r]-1};
      moves.push_back(m);
    }
  }
  return moves;
}

RegionVector BasicAttackBot::cmd_pick_starting_regions(long t, RegionVector regions) {
  RegionVector rv;
  std::srand(regions[0]);
  std::random_shuffle(regions.begin(), regions.end());
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

MoveVector BasicAttackBot::generate_attacks(int r) {
  int armies_available = occupancy[r] - 1;
  MoveVector moves;
  std::vector<int> excluded_regions;
  while (true) {
    int best_attack_region = -1;
    int best_num_enemies = 0;
    for (size_t enemy_region = 0; enemy_region < region_ids.size(); ++enemy_region) {
      if (!neighbours[r][enemy_region]) continue;
      if (owner[enemy_region] == ME) continue;
      if (element_in_vector(enemy_region, excluded_regions)) continue;
      if (conquest::internal::get_win_prob(armies_available, occupancy[enemy_region]) > 0.8) {
        if (occupancy[enemy_region] > best_num_enemies) {
          best_num_enemies = occupancy[enemy_region];
          best_attack_region = enemy_region;
        }
      }
    }

    if (best_attack_region >= 0) {
      int necessary_armies = armies_available;
      while (conquest::internal::get_win_prob(necessary_armies, occupancy[best_attack_region]) > 0.8) necessary_armies--;
      necessary_armies++;
      Move m = {region_ids[r], region_ids[best_attack_region], necessary_armies};
      armies_available -= necessary_armies;
      moves.push_back(m);
      excluded_regions.push_back(best_attack_region);
    } else {
      break;
    }
  }
  return moves;
}
