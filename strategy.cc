#include "strategy.h"

#include "prob_math.h"

#include <set>

namespace {
bool element_in_vector(int element, std::vector<int> v) {
  for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    if (element == *it) return true;
  }
  return false;
}

int sum_vector(std::vector<int> v) {
  int sum = 0;
  for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    sum += *it;
  }
  return sum;
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
  return -1.0 * 100000;
}

PlacementVector BasicStrategy::place_armies(int n) {
  //place armies on one region, the one with the most neighbour armies
  int max_neighbour_armies = 0;
  int max_region = 0;

  for (size_t r = 0; r < bot.region_n; ++r) {
    if (bot.owner[r] != ME) continue;
    int army_neighbours = bot.get_enemy_neighbour_armies(r);
    if (army_neighbours > max_neighbour_armies) {
      max_region = r;
      max_neighbour_armies = army_neighbours;
    }
  }
  PlacementVector pv;
  Placement p = {max_region, n};
  pv.push_back(p);

  // add the armies to occupancy
  bot.occupancy[max_region] += n;
  return pv;
}

MoveVector BasicStrategy::do_moves(std::vector<int> &armies) {
  MoveVector moves;

  for (size_t r = 0; r < bot.region_n; ++r) {
    if (bot.owner[r] != ME) continue;
    if (armies[r] == 0) continue;

    if (bot.has_enemy_neighbours(r)) {
      MoveVector attack_moves = generate_attacks(r, armies);
      moves.insert(moves.end(), attack_moves.begin(), attack_moves.end());
    } else {
      int closest_enemy_region = -1;
      int enemy_distance = bot.region_n;
      for (size_t possible_region = 0; possible_region < bot.region_n; ++possible_region) {
        if (bot.owner[possible_region] == ME) continue;
        if (bot.distances[r][possible_region] < enemy_distance) {
          closest_enemy_region = possible_region;
          enemy_distance = bot.distances[r][possible_region];
        }
      }

      int target_region = -1;
      for (size_t neigh_region = 0; neigh_region < bot.region_n; ++neigh_region) {
        if (!bot.neighbours[r][neigh_region]) continue;
        if (bot.distances[neigh_region][closest_enemy_region] < enemy_distance) {
          target_region = neigh_region;
          break;
        }
      }

      Move m = {r, target_region, armies[r]};
      armies[r] = 0;
      moves.push_back(m);
    }
  }
  return moves;

}

MoveVector BasicStrategy::generate_attacks(int r, std::vector<int> &armies) {
  double MIN_WIN_PROB = 0.8;
  std::set<int> targets;

  MoveVector moves;
  std::vector<int> regions_done;

  for (std::vector<int>::const_iterator it = bot.neighbour_ids[r].begin(); it != bot.neighbour_ids[r].end(); ++it) {
    if (bot.owner[*it] == ME) continue;
    targets.insert(*it);
  }

  while (targets.size() > 0) {
    int best_attack_region = -1;
    int best_num_enemies = 0;
    for (std::set<int>::iterator it = targets.begin(); it != targets.end(); ++it) {
      if (conquest::internal::get_win_prob(armies[r], bot.occupancy[*it]) > MIN_WIN_PROB) {
        if (bot.occupancy[*it] > best_num_enemies) {
          best_num_enemies = bot.occupancy[*it];
          best_attack_region = *it;
        }
      }
    }
    if (best_attack_region >= 0) {
      int necessary_armies = conquest::internal::attackers_needed(best_num_enemies, MIN_WIN_PROB);
      armies[r] -= necessary_armies;
      Move m = {r, best_attack_region, necessary_armies};
      moves.push_back(m);
      targets.erase(best_attack_region);
    } else {
      break;
    }
  }

  bool all_other_attacked = true;
  for (std::set<int>::iterator it = targets.begin(); it != targets.end(); ++it) {
    if (bot.owner[*it] == OTHER) all_other_attacked = false;
  }

  if (all_other_attacked && moves.size() > 0) {
    int i = 0;
    while (armies[r] > 0) {
      moves[i].amount++;
      armies[r]--;
      i = (i + 1) % moves.size();
    }
  }
  return moves;
}

void AquireContinentStrategy::update() {
  int indirect_enemies = 0;

  std::vector<int> unused_armies(bot.region_n, 0);
  army_need = std::vector<int>((size_t) bot.region_n, 0);

  active_ = false;
  bool enemies_present = false;

  for (int r = 0; r < bot.region_n; ++r) {
    if (bot.region_super[r] != super_region) continue;
    if (bot.owner[r] != ME) {
      enemies_present = true;
    } else {
      active_ = true;
      unused_armies[r] = bot.occupancy[r] - 1;
    }
  }

  if (!enemies_present) active_ = false;
  if (!active_) return;

  for (int r = 0; r < bot.region_n; ++r) {
    if (bot.region_super[r] != super_region) continue;
    if (bot.owner[r] == ME) continue;

    int armies_needed = conquest::internal::attackers_needed(bot.occupancy[r], WIN_PROB);
    int optimal_neighbour_id = -1;
    int optimal_neighbour_armies = 0;
    for (int nr = 0; nr < bot.region_n; ++nr) {
      if (!bot.neighbours[r][nr]) continue;
      if ((unused_armies[nr] < optimal_neighbour_armies && unused_armies[nr] >= armies_needed) ||
          (unused_armies[nr] > optimal_neighbour_armies && optimal_neighbour_armies < armies_needed)) {
        optimal_neighbour_armies = unused_armies[nr];
        optimal_neighbour_id = nr;
      }
    }
    if (optimal_neighbour_id >= 0) {
      int armies_used = std::min(unused_armies[optimal_neighbour_id], armies_needed);
      unused_armies[optimal_neighbour_id] -= armies_used;
      army_need[optimal_neighbour_id] += armies_needed - armies_used;
    } else {
      indirect_enemies += bot.occupancy[r];
    }
  }

  need = conquest::internal::attackers_needed(indirect_enemies, WIN_PROB) + sum_vector(army_need);
}

int AquireContinentStrategy::armies_needed() {
  return need;
}

PlacementVector AquireContinentStrategy::place_armies(int n) {
  PlacementVector pv;
  for (int r = 0; r < bot.region_n; ++r) {
    if (bot.region_super[r] != super_region) continue;
    if (army_need[r] > 0) {
      int armies_used = std::min(n, army_need[r]);
      army_need[r] -= armies_used;
      n -= armies_used;
      bot.occupancy[r] += armies_used;
      Placement p = {r, armies_used};
      pv.push_back(p);
    }
  }

  if (n == 0) return pv;

  for (size_t r = 0; r < bot.region_n; ++r) {
    if (bot.owner[r] != ME) continue;
    if (bot.region_super[r] != super_region) continue;

    Placement p = {r, n};
    pv.push_back(p);
    break;

  }
  return pv;
}

MoveVector AquireContinentStrategy::do_moves(std::vector<int> &armies) {
  MoveVector moves;

  for (size_t r = 0; r < bot.region_n; ++r) {
    if (bot.owner[r] != ME) continue;
    if (bot.region_super[r] != super_region) continue;
    if (armies[r] == 0) continue;

    if (bot.has_enemy_neighbours(r)) {
      MoveVector attack_moves = generate_attacks(r, armies);
      moves.insert(moves.end(), attack_moves.begin(), attack_moves.end());
    } else {
      int closest_enemy_region = -1;
      int enemy_distance = bot.region_n;
      for (size_t possible_region = 0; possible_region < bot.region_n; ++possible_region) {
        if (bot.owner[possible_region] == ME) continue;
        if (bot.distances[r][possible_region] < enemy_distance) { // bug!!! (not fixed yet because of refactoring)
          closest_enemy_region = possible_region;
          enemy_distance = bot.distances[r][possible_region];
        }
      }

      int target_region = -1;
      for (size_t neigh_region = 0; neigh_region < bot.region_n; ++neigh_region) {
        if (!bot.neighbours[r][neigh_region]) continue;
        if (bot.distances[neigh_region][closest_enemy_region] < enemy_distance) {
          target_region = neigh_region;
          break;
        }
      }

      Move m = {r, target_region, armies[r]};
      armies[r] = 0;
      moves.push_back(m);
    }
  }
  return moves;

}

MoveVector AquireContinentStrategy::generate_attacks(int r, std::vector<int> &armies) {
  double MIN_WIN_PROB = 0.8;
  std::set<int> targets;

  MoveVector moves;
  std::vector<int> regions_done;

  for (std::vector<int>::const_iterator it = bot.neighbour_ids[r].begin(); it != bot.neighbour_ids[r].end(); ++it) {
    if (bot.owner[*it] == ME) continue;
    if (bot.region_super[*it] != super_region) continue;
    targets.insert(*it);
  }

  while (targets.size() > 0) {
    int best_attack_region = -1;
    int best_num_enemies = 0;
    for (std::set<int>::iterator it = targets.begin(); it != targets.end(); ++it) {
      if (conquest::internal::get_win_prob(armies[r], bot.occupancy[*it]) > MIN_WIN_PROB) {
        if (bot.occupancy[*it] > best_num_enemies) {
          best_num_enemies = bot.occupancy[*it];
          best_attack_region = *it;
        }
      }
    }
    if (best_attack_region >= 0) {
      int necessary_armies = conquest::internal::attackers_needed(best_num_enemies, MIN_WIN_PROB);
      armies[r] -= necessary_armies;
      Move m = {r, best_attack_region, necessary_armies};
      moves.push_back(m);
      targets.erase(best_attack_region);
    } else {
      break;
    }
  }

  bool all_other_attacked = true;
  for (std::set<int>::iterator it = targets.begin(); it != targets.end(); ++it) {
    if (bot.owner[*it] == OTHER) all_other_attacked = false;
  }

  if (all_other_attacked && moves.size() > 0) {
    int i = 0;
    while (armies[r] > 0) {
      moves[i].amount++;
      armies[r]--;
      i = (i + 1) % moves.size();
    }
  }
  return moves;
}

int AquireContinentStrategy::get_local_neighbour_armies(int region) {
  int num_neighbours = 0;
  for (size_t r = 0; r < bot.region_n; ++r) {
    if (!bot.neighbours[region][r]) continue;
    if (bot.owner[r] == ME) continue;
    if (bot.region_super[r] != super_region) continue;
    num_neighbours += bot.occupancy[r];
  }
  return num_neighbours;
}

void DefenseStrategy::update() {
  active_ = false;
  need = std::vector<int>((size_t) bot.region_n, 0);
  for (size_t r = 0; r < bot.region_n; ++r) {
    if (bot.owner[r] != ME) continue;
    for (int nr = 0; nr < bot.region_n; ++nr) {
      if (bot.owner[nr] != OTHER) continue;
      if (!bot.neighbours[r][nr]) continue;

      int attackers = bot.occupancy[nr] + expected_increase - 1;
      int defenders = conquest::internal::defenders_needed(attackers, DEFENSE_PROB);
      if (bot.occupancy[r] + need[r] < defenders) {
        need[r] = defenders - bot.occupancy[r];
        active_ = true;
      }
    }
  }
}

bool DefenseStrategy::active() {
  return active_;
}

int DefenseStrategy::armies_needed() {
  return sum_vector(need);
}

double DefenseStrategy::get_priority() const {
  return -1 * sum_vector(need);
}

PlacementVector DefenseStrategy::place_armies(int n) {
  PlacementVector pv;
  for (size_t r = 0; r < bot.region_n; ++r) {
    if (need[r] > 0) {
      int armies_used = std::min(n, need[r]);
      n -= armies_used;
      bot.occupancy[r] += armies_used;
      Placement p = {r, armies_used};
      pv.push_back(p);
    }
  }

  return pv;
}
