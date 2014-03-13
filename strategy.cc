#include "strategy.h"

#include "prob_math.h"

#include <set>

namespace {

army_t sum_vector(ArmyVector v) {
  army_t sum = 0;
  for (ArmyVector::iterator it = v.begin(); it != v.end(); ++it) {
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

army_t BasicStrategy::armies_needed() {
  return 10000;
}

double BasicStrategy::get_priority() const {
  return -1.0 * 100000;
}

PlacementVector BasicStrategy::place_armies(army_t n) {
  //place armies on one region, the one with the most neighbour armies
  army_t max_neighbour_armies = 0;
  reg_t max_region = 0;

  for (reg_t r = 0; r < bot.region_n; ++r) {
    if (bot.owner[r] != ME) continue;
    army_t army_neighbours = bot.get_enemy_neighbour_armies(r);
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

MoveVector BasicStrategy::do_moves(ArmyVector &armies) {
  MoveVector moves;

  for (reg_t r = 0; r < bot.region_n; ++r) {
    if (bot.owner[r] != ME) continue;
    if (armies[r] == 0) continue;

    if (bot.has_enemy_neighbours(r)) {
      MoveVector attack_moves = generate_attacks(r, armies);
      moves.insert(moves.end(), attack_moves.begin(), attack_moves.end());
    } else {
      reg_t closest_enemy_region = UNKNOWN_REGION;
      int enemy_distance = bot.region_n;
      for (reg_t possible_region = 0; possible_region < bot.region_n; ++possible_region) {
        if (bot.owner[possible_region] == ME) continue;
        if (bot.distances[r][possible_region] < enemy_distance) {
          closest_enemy_region = possible_region;
          enemy_distance = bot.distances[r][possible_region];
        }
      }

      if (closest_enemy_region == UNKNOWN_REGION) continue;

      reg_t target_region = UNKNOWN_REGION;
      for (reg_t neigh_region = 0; neigh_region < bot.region_n; ++neigh_region) {
        if (!bot.neighbours[r][neigh_region]) continue;
        if (bot.distances[neigh_region][closest_enemy_region] < enemy_distance) {
          target_region = neigh_region;
          break;
        }
      }

      if (target_region == UNKNOWN_REGION) continue;

      Move m = {r, target_region, armies[r]};
      armies[r] = 0;
      moves.push_back(m);
    }
  }
  return moves;

}

MoveVector BasicStrategy::generate_attacks(reg_t r, ArmyVector &armies) {
  double MIN_WIN_PROB = 0.8;
  std::set<reg_t> targets;

  MoveVector moves;
  std::vector<reg_t> regions_done;

  for (RegionVector::const_iterator it = bot.neighbour_ids[r].begin(); it != bot.neighbour_ids[r].end(); ++it) {
    if (bot.owner[*it] == ME) continue;
    targets.insert(*it);
  }

  while (targets.size() > 0) {
    reg_t best_attack_region = UNKNOWN_REGION;
    army_t best_num_enemies = 0;
    for (std::set<reg_t>::iterator it = targets.begin(); it != targets.end(); ++it) {
      if (conquest::internal::get_win_prob(armies[r], bot.occupancy[*it]) > MIN_WIN_PROB) {
        if (bot.occupancy[*it] > best_num_enemies) {
          best_num_enemies = bot.occupancy[*it];
          best_attack_region = *it;
        }
      }
    }
    if (best_attack_region != UNKNOWN_REGION) {
      army_t necessary_armies = conquest::internal::attackers_needed(best_num_enemies, MIN_WIN_PROB);
      armies[r] -= necessary_armies;
      Move m = {r, best_attack_region, necessary_armies};
      moves.push_back(m);
      targets.erase(best_attack_region);
    } else {
      break;
    }
  }

  bool all_other_attacked = true;
  for (std::set<reg_t>::iterator it = targets.begin(); it != targets.end(); ++it) {
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
  army_t indirect_enemies = 0;

  ArmyVector unused_armies(bot.region_n, 0);
  army_need = ArmyVector(bot.region_n, 0);

  active_ = false;
  bool enemies_present = false;

  for (reg_t r = 0; r < bot.region_n; ++r) {
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

  for (reg_t r = 0; r < bot.region_n; ++r) {
    if (bot.region_super[r] != super_region) continue;
    if (bot.owner[r] == ME) continue;

    army_t armies_needed = conquest::internal::attackers_needed(bot.occupancy[r], WIN_PROB);
    reg_t optimal_neighbour_id = UNKNOWN_REGION;
    army_t optimal_neighbour_armies = 0;
    for (reg_t nr = 0; nr < bot.region_n; ++nr) {
      if (!bot.neighbours[r][nr]) continue;
      if ((unused_armies[nr] < optimal_neighbour_armies && unused_armies[nr] >= armies_needed) ||
          (unused_armies[nr] > optimal_neighbour_armies && optimal_neighbour_armies < armies_needed)) {
        optimal_neighbour_armies = unused_armies[nr];
        optimal_neighbour_id = nr;
      }
    }
    if (optimal_neighbour_id != UNKNOWN_REGION) {
      army_t armies_used = std::min(unused_armies[optimal_neighbour_id], armies_needed);
      unused_armies[optimal_neighbour_id] -= armies_used;
      army_need[optimal_neighbour_id] += armies_needed - armies_used;
    } else {
      indirect_enemies += bot.occupancy[r];
    }
  }

  need = conquest::internal::attackers_needed(indirect_enemies, WIN_PROB) + sum_vector(army_need);
}

army_t AquireContinentStrategy::armies_needed() {
  return need;
}

PlacementVector AquireContinentStrategy::place_armies(army_t n) {
  PlacementVector pv;
  for (reg_t r = 0; r < bot.region_n; ++r) {
    if (bot.region_super[r] != super_region) continue;
    if (army_need[r] > 0) {
      army_t armies_used = std::min(n, army_need[r]);
      army_need[r] -= armies_used;
      n -= armies_used;
      bot.occupancy[r] += armies_used;
      Placement p = {r, armies_used};
      pv.push_back(p);
    }
  }

  if (n == 0) return pv;

  for (reg_t r = 0; r < bot.region_n; ++r) {
    if (bot.owner[r] != ME) continue;
    if (bot.region_super[r] != super_region) continue;

    Placement p = {r, n};
    pv.push_back(p);
    break;

  }
  return pv;
}

MoveVector AquireContinentStrategy::do_moves(ArmyVector &armies) {
  MoveVector moves;

  for (reg_t r = 0; r < bot.region_n; ++r) {
    if (bot.owner[r] != ME) continue;
    if (bot.region_super[r] != super_region) continue;
    if (armies[r] == 0) continue;

    if (bot.has_enemy_neighbours(r)) {
      MoveVector attack_moves = generate_attacks(r, armies);
      moves.insert(moves.end(), attack_moves.begin(), attack_moves.end());
    } else {
      reg_t closest_enemy_region = UNKNOWN_REGION;
      int enemy_distance = bot.region_n;
      for (reg_t possible_region = 0; possible_region < bot.region_n; ++possible_region) {
        if (bot.owner[possible_region] == ME) continue;
        if (bot.distances[r][possible_region] < enemy_distance) { // bug!!! (not fixed yet because of refactoring)
          closest_enemy_region = possible_region;
          enemy_distance = bot.distances[r][possible_region];
        }
      }

      if (closest_enemy_region == UNKNOWN_REGION) continue;

      reg_t target_region = UNKNOWN_REGION;
      for (reg_t neigh_region = 0; neigh_region < bot.region_n; ++neigh_region) {
        if (!bot.neighbours[r][neigh_region]) continue;
        if (bot.distances[neigh_region][closest_enemy_region] < enemy_distance) {
          target_region = neigh_region;
          break;
        }
      }

      if (target_region == UNKNOWN_REGION) continue;

      Move m = {r, target_region, armies[r]};
      armies[r] = 0;
      moves.push_back(m);
    }
  }
  return moves;

}

MoveVector AquireContinentStrategy::generate_attacks(reg_t r, ArmyVector &armies) {
  double MIN_WIN_PROB = 0.8;
  std::set<reg_t> targets;

  MoveVector moves;
  RegionVector regions_done;

  for (RegionVector::const_iterator it = bot.neighbour_ids[r].begin(); it != bot.neighbour_ids[r].end(); ++it) {
    if (bot.owner[*it] == ME) continue;
    if (bot.region_super[*it] != super_region) continue;
    targets.insert(*it);
  }

  while (targets.size() > 0) {
    reg_t best_attack_region = UNKNOWN_REGION;
    army_t best_num_enemies = 0;
    for (std::set<reg_t>::iterator it = targets.begin(); it != targets.end(); ++it) {
      if (conquest::internal::get_win_prob(armies[r], bot.occupancy[*it]) > MIN_WIN_PROB) {
        if (bot.occupancy[*it] > best_num_enemies) {
          best_num_enemies = bot.occupancy[*it];
          best_attack_region = *it;
        }
      }
    }
    if (best_attack_region != UNKNOWN_REGION) {
      army_t necessary_armies = conquest::internal::attackers_needed(best_num_enemies, MIN_WIN_PROB);
      armies[r] -= necessary_armies;
      Move m = {r, best_attack_region, necessary_armies};
      moves.push_back(m);
      targets.erase(best_attack_region);
    } else {
      break;
    }
  }

  bool all_other_attacked = true;
  for (std::set<reg_t>::iterator it = targets.begin(); it != targets.end(); ++it) {
    if (bot.owner[*it] == OTHER) all_other_attacked = false;
  }

  if (all_other_attacked && moves.size() > 0) {
    unsigned i = 0;
    while (armies[r] > 0) {
      moves[i].amount++;
      armies[r]--;
      i = (i + 1) % moves.size();
    }
  }
  return moves;
}

army_t AquireContinentStrategy::get_local_neighbour_armies(reg_t region) {
  army_t num_neighbours = 0;
  for (reg_t r = 0; r < bot.region_n; ++r) {
    if (!bot.neighbours[region][r]) continue;
    if (bot.owner[r] == ME) continue;
    if (bot.region_super[r] != super_region) continue;
    num_neighbours += bot.occupancy[r];
  }
  return num_neighbours;
}

void DefenseStrategy::update() {
  active_ = false;
  need = ArmyVector(bot.region_n, 0);
  for (reg_t r = 0; r < bot.region_n; ++r) {
    if (bot.owner[r] != ME) continue;
    for (reg_t nr = 0; nr < bot.region_n; ++nr) {
      if (bot.owner[nr] != OTHER) continue;
      if (!bot.neighbours[r][nr]) continue;

      army_t attackers = bot.occupancy[nr] + expected_increase - 1;
      army_t defenders = conquest::internal::defenders_needed(attackers, DEFENSE_PROB);
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

army_t DefenseStrategy::armies_needed() {
  return sum_vector(need);
}

double DefenseStrategy::get_priority() const {
  return -1 * sum_vector(need);
}

PlacementVector DefenseStrategy::place_armies(army_t n) {
  PlacementVector pv;
  for (reg_t r = 0; r < bot.region_n; ++r) {
    if (need[r] > 0) {
      army_t armies_used = std::min(n, need[r]);
      n -= armies_used;
      bot.occupancy[r] += armies_used;
      Placement p = {r, armies_used};
      pv.push_back(p);
    }
  }

  return pv;
}

void DefendContinentStrategy::update() {
  active_ = false;
  need = 0;
}

army_t DefendContinentStrategy::armies_needed() {
  return Strategy::armies_needed();
}

PlacementVector DefendContinentStrategy::place_armies(army_t n) {
  return Strategy::place_armies(n);
}

MoveVector DefendContinentStrategy::do_moves(ArmyVector &armies) {
  return Strategy::do_moves(armies);
}
