#include "strategy.h"

#include "prob_math.h"

namespace {
bool element_in_vector(int element, std::vector<int> v) {
  for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    if (element == *it) return true;
  }
  return false;
}
}

bool BasicStrategy::active() {
  return true;
}

void BasicStrategy::update() {
}

int BasicStrategy::armies_needed() {
  return 10000;
}

double BasicStrategy::get_priority() const {
  return 0.0;
}

PlacementVector BasicStrategy::place_armies(int n) {
  //place armies on one region, the one with the most neighbour armies
  int max_neighbour_armies = 0;
  int max_region = 0;

  for (size_t r = 0; r < bot.region_ids.size(); ++r) {
    if (bot.owner[r] != ME) continue;
    int army_neighbours = bot.get_enemy_neighbour_armies(r);
    if (army_neighbours > max_neighbour_armies) {
      max_region = r;
      max_neighbour_armies = army_neighbours;
    }
  }
  Placement p = {bot.region_ids[max_region], n};
  PlacementVector pv;
  if(n > 0) pv.push_back(p);

  // add the armies to occupancy
  bot.occupancy[max_region] += n;
  return pv;
}

MoveVector BasicStrategy::do_moves(std::vector<int>& armies) {
  MoveVector moves;

  for (size_t r = 0; r < bot.region_ids.size(); ++r) {
    if (bot.owner[r] != ME) continue;
    if (armies[r] == 0) continue;

    if (bot.has_enemy_neighbours(r)) {
      MoveVector attack_moves = generate_attacks(r, armies);
      moves.insert(moves.end(), attack_moves.begin(), attack_moves.end());
    } else {
      int closest_enemy_region = -1;
      int enemy_distance = bot.region_ids.size();
      for (size_t possible_region = 0; possible_region < bot.region_ids.size(); ++possible_region) {
        if (bot.owner[r] == ME) continue;
        if (bot.distances[r][possible_region] < closest_enemy_region) {
          closest_enemy_region = possible_region;
          enemy_distance = bot.distances[r][possible_region];
        }
      }

      int target_region = -1;
      for (size_t neigh_region = 0; neigh_region < bot.region_ids.size(); ++neigh_region) {
        if (!bot.neighbours[r][neigh_region]) continue;
        if (bot.distances[neigh_region][closest_enemy_region] < enemy_distance) {
          target_region = neigh_region;
          break;
        }
      }

      Move m = {bot.region_ids[r], bot.region_ids[target_region], armies[r]};
      armies[r] = 0;
      moves.push_back(m);
    }
  }
  return moves;

}

MoveVector BasicStrategy::generate_attacks(int r, std::vector<int>& armies) {
  int armies_available = armies[r];
  MoveVector moves;
  std::vector<int> excluded_regions;
  while (true) {
    int best_attack_region = -1;
    int best_num_enemies = 0;
    for (size_t enemy_region = 0; enemy_region < bot.region_ids.size(); ++enemy_region) {
      if (!bot.neighbours[r][enemy_region]) continue;
      if (bot.owner[enemy_region] == ME) continue;
      if (element_in_vector(enemy_region, excluded_regions)) continue;
      if (conquest::internal::get_win_prob(armies[r], bot.occupancy[enemy_region]) > 0.8) {
        if (bot.occupancy[enemy_region] > best_num_enemies) {
          best_num_enemies = bot.occupancy[enemy_region];
          best_attack_region = enemy_region;
        }
      }
    }

    if (best_attack_region >= 0) {
      int necessary_armies = armies[r];
      while (conquest::internal::get_win_prob(necessary_armies, bot.occupancy[best_attack_region]) > 0.8) necessary_armies--;
      necessary_armies++;
      Move m = {bot.region_ids[r], bot.region_ids[best_attack_region], necessary_armies};
      armies[r] -= necessary_armies;
      moves.push_back(m);
      excluded_regions.push_back(best_attack_region);
    } else {
      break;
    }
  }
  return moves;
}

void AquireContinentStrategy::update() {
  int regions_missing = 0;
  need = 0;
  active_ = false;

  for (int r = 0; r < bot.region_ids.size(); ++r) {
    if (bot.region_super_region[r] != super_region) continue;
    if (bot.owner[r] == ME) {
      active_ = true;
      continue;
    };

    regions_missing += 1;

    int max_neighbours = 0;

    for (int nr = 0; nr < bot.region_ids.size(); ++nr) {
      if (!bot.neighbours[r][nr]) continue;
      if (bot.owner[nr] != ME) continue;
      if (bot.occupancy[nr] > max_neighbours) {
        max_neighbours = bot.occupancy[nr];
      }
    }

    int local_need = 0;
    while (conquest::internal::get_win_prob(max_neighbours - 1 + local_need, bot.occupancy[r]) < WIN_PROB) {
      ++local_need;
    }

    need += local_need;

//
//    if (bot.owner[r] == ME) {
//      my_surplus_armies += bot.occupancy[r] - 1;
//      active_ = true;
//    } else {
//      enemy_armies += bot.occupancy[r];
//      regions_missing += 1;
//    }
//
//    while (conquest::internal::get_win_prob(my_surplus_armies + need - regions_missing, enemy_armies) < WIN_PROB) {
//      ++need;
//    }
  }

  if (regions_missing == 0) {
    active_ = false;
  }
}

int AquireContinentStrategy::armies_needed() {
  return need;
}

PlacementVector AquireContinentStrategy::place_armies(int n) {
  int max_neighbour_armies = 0;
  int max_region = 0;

  for (size_t r = 0; r < bot.region_ids.size(); ++r) {
    if (bot.owner[r] != ME) continue;
    if (bot.region_super_region[r] != super_region) continue;

    int army_neighbours = get_local_neighbour_armies(r);
    if (army_neighbours > max_neighbour_armies) {
      max_region = r;
      max_neighbour_armies = army_neighbours;
    }
  }
  Placement p = {bot.region_ids[max_region], n};
  PlacementVector pv;
  if(n > 0) pv.push_back(p);

  // add the armies to occupancy
  bot.occupancy[max_region] += n;
  return pv;
}

MoveVector AquireContinentStrategy::do_moves(std::vector<int>& armies) {
  return MoveVector();
}

int AquireContinentStrategy::get_local_neighbour_armies(int region) {
  int num_neighbours = 0;
  for (size_t r = 0; r < bot.region_ids.size(); ++r) {
    if (!bot.neighbours[region][r]) continue;
    if (bot.owner[r] == ME) continue;
    if (bot.region_super_region[r] != super_region) continue;
    num_neighbours += bot.occupancy[r];
  }
  return num_neighbours;
}
