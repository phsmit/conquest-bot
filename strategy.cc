#include "strategy.h"

#include "prob_math.h"

#include <set>
#include "util.h"

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

  for (auto r : range(data.region_n)) {
    if (data.owner[r] != ME) continue;
    army_t army_neighbours = data.get_enemy_neighbour_armies(r);
    if (army_neighbours > max_neighbour_armies) {
      max_region = r;
      max_neighbour_armies = army_neighbours;
    }
  }
  PlacementVector pv;
  pv.push_back({max_region, n});

  // add the armies to occupancy
  data.occupancy[max_region] += n;
  return pv;
}

MoveVector BasicStrategy::do_moves(ArmyVector &armies) {
  MoveVector moves;

  for (auto r : range(data.region_n)) {
    if (data.owner[r] != ME) continue;
    if (armies[r] == 0) continue;

    if (data.has_enemy_neighbours(r)) {
      MoveVector attack_moves = generate_attacks(r, armies);
      moves.insert(moves.end(), attack_moves.begin(), attack_moves.end());
    } else {
      reg_t closest_enemy_region = UNKNOWN_REGION;
      int enemy_distance = data.region_n;
      for (auto possible_region : range(data.region_n)) {
        if (data.owner[possible_region] == ME) continue;
        if (data.distances[r][possible_region] < enemy_distance) {
          closest_enemy_region = possible_region;
          enemy_distance = data.distances[r][possible_region];
        }
      }

      if (closest_enemy_region == UNKNOWN_REGION) continue;

      reg_t target_region = UNKNOWN_REGION;
      for (auto neigh_region : range(data.region_n)) {
        if (!data.neighbours[r][neigh_region]) continue;
        if (data.distances[neigh_region][closest_enemy_region] < enemy_distance) {
          target_region = neigh_region;
          break;
        }
      }

      if (target_region == UNKNOWN_REGION) continue;

      moves.push_back({r, target_region, armies[r]});
      armies[r] = 0;
    }
  }
  return moves;

}

MoveVector BasicStrategy::generate_attacks(reg_t r, ArmyVector &armies) {
  double MIN_WIN_PROB = 0.8;
  std::set<reg_t> targets;

  MoveVector moves;
  std::vector<reg_t> regions_done;

  for (auto neigbhour : data.neighbour_ids[r]) {
    if (data.owner[neigbhour] == ME) continue;
    targets.insert(neigbhour);
  }

  while (targets.size() > 0) {
    reg_t best_attack_region = UNKNOWN_REGION;
    army_t best_num_enemies = 0;
    for (auto target : targets) {
      if (conquest::internal::get_win_prob(armies[r], data.occupancy[target]) > MIN_WIN_PROB) {
        if (data.occupancy[target] > best_num_enemies) {
          best_num_enemies = data.occupancy[target];
          best_attack_region = target;
        }
      }
    }
    if (best_attack_region != UNKNOWN_REGION) {
      army_t necessary_armies = conquest::internal::attackers_needed(best_num_enemies, MIN_WIN_PROB);
      armies[r] -= necessary_armies;
      moves.push_back({r, best_attack_region, necessary_armies});
      targets.erase(best_attack_region);
    } else {
      break;
    }
  }

  bool all_other_attacked = true;
  for (auto target : targets) {
    if (data.owner[target] == OTHER) all_other_attacked = false;
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

  ArmyVector unused_armies(data.region_n, 0);
  army_need = ArmyVector(data.region_n, 0);

  active_ = false;
  bool enemies_present = false;

  for (auto r : range(data.region_n)) {
    if (data.region_super[r] != super_region) continue;
    if (data.owner[r] != ME) {
      enemies_present = true;
    } else {
      active_ = true;
      unused_armies[r] = data.occupancy[r] - 1;
    }
  }

  if (!enemies_present) active_ = false;
  if (!active_) return;

  for (auto r : range(data.region_n)) {
    if (data.region_super[r] != super_region) continue;
    if (data.owner[r] == ME) continue;

    army_t armies_needed = conquest::internal::attackers_needed(data.occupancy[r], WIN_PROB);
    reg_t optimal_neighbour_id = UNKNOWN_REGION;
    army_t optimal_neighbour_armies = 0;
    for (auto nr : range(data.region_n)) {
      if (!data.neighbours[r][nr]) continue;
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
      indirect_enemies += data.occupancy[r];
    }
  }

  need = conquest::internal::attackers_needed(indirect_enemies, WIN_PROB) + sum(army_need);
}

army_t AquireContinentStrategy::armies_needed() {
  return need;
}

PlacementVector AquireContinentStrategy::place_armies(army_t n) {
  PlacementVector pv;
  for (auto r : range(data.region_n)) {
    if (data.region_super[r] != super_region) continue;
    if (army_need[r] > 0) {
      army_t armies_used = std::min(n, army_need[r]);
      army_need[r] -= armies_used;
      n -= armies_used;
      data.occupancy[r] += armies_used;
      pv.push_back({r, armies_used});
    }
  }

  if (n == 0) return pv;

  for (auto r : range(data.region_n)) {
    if (data.owner[r] != ME) continue;
    if (data.region_super[r] != super_region) continue;

    pv.push_back({r, n});
    break;

  }
  return pv;
}

MoveVector AquireContinentStrategy::do_moves(ArmyVector &armies) {
  MoveVector moves;

  for (auto r : range(data.region_n)) {
    if (data.owner[r] != ME) continue;
    if (data.region_super[r] != super_region) continue;
    if (armies[r] == 0) continue;

    if (data.has_enemy_neighbours(r)) {
      MoveVector attack_moves = generate_attacks(r, armies);
      moves.insert(moves.end(), attack_moves.begin(), attack_moves.end());
    } else {
      reg_t closest_enemy_region = UNKNOWN_REGION;
      int enemy_distance = data.region_n;
      for (auto possible_region : range(data.region_n)) {
        if (data.owner[possible_region] == ME) continue;
        if (data.distances[r][possible_region] < enemy_distance) { // bug!!! (not fixed yet because of refactoring)
          closest_enemy_region = possible_region;
          enemy_distance = data.distances[r][possible_region];
        }
      }

      if (closest_enemy_region == UNKNOWN_REGION) continue;

      reg_t target_region = UNKNOWN_REGION;
      for (auto neigh_region : range(data.region_n)) {
        if (!data.neighbours[r][neigh_region]) continue;
        if (data.distances[neigh_region][closest_enemy_region] < enemy_distance) {
          target_region = neigh_region;
          break;
        }
      }

      if (target_region == UNKNOWN_REGION) continue;

      moves.push_back({r, target_region, armies[r]});
      armies[r] = 0;
    }
  }
  return moves;

}

MoveVector AquireContinentStrategy::generate_attacks(reg_t r, ArmyVector &armies) {
  double MIN_WIN_PROB = 0.8;
  std::set<reg_t> targets;

  MoveVector moves;
  RegionVector regions_done;

  for (auto neighbour : data.neighbour_ids[r]) {
    if (data.owner[neighbour] == ME) continue;
    if (data.region_super[neighbour] != super_region) continue;
    targets.insert(neighbour);
  }

  while (targets.size() > 0) {
    reg_t best_attack_region = UNKNOWN_REGION;
    army_t best_num_enemies = 0;
    for (auto target : targets) {
      if (conquest::internal::get_win_prob(armies[r], data.occupancy[target]) > MIN_WIN_PROB) {
        if (data.occupancy[target] > best_num_enemies) {
          best_num_enemies = data.occupancy[target];
          best_attack_region = target;
        }
      }
    }
    if (best_attack_region != UNKNOWN_REGION) {
      army_t necessary_armies = conquest::internal::attackers_needed(best_num_enemies, MIN_WIN_PROB);
      armies[r] -= necessary_armies;
      moves.push_back({r, best_attack_region, necessary_armies});
      targets.erase(best_attack_region);
    } else {
      break;
    }
  }

  bool all_other_attacked = true;
  for (auto target : targets) {
    if (data.owner[target] == OTHER) all_other_attacked = false;
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
  for (auto r : range(data.region_n)) {
    if (!data.neighbours[region][r]) continue;
    if (data.owner[r] == ME) continue;
    if (data.region_super[r] != super_region) continue;
    num_neighbours += data.occupancy[r];
  }
  return num_neighbours;
}

void DefenseStrategy::update() {
  active_ = false;
  need = ArmyVector(data.region_n, 0);
  for (auto r : range(data.region_n)) {
    if (data.owner[r] != ME) continue;
    for (auto nr : range(data.region_n)) {
      if (data.owner[nr] != OTHER) continue;
      if (!data.neighbours[r][nr]) continue;

      army_t attackers = data.occupancy[nr] + expected_increase - 1;
      army_t defenders = conquest::internal::defenders_needed(attackers, DEFENSE_PROB);
      if (data.occupancy[r] + need[r] < defenders) {
        need[r] = defenders - data.occupancy[r];
        active_ = true;
      }
    }
  }
}

bool DefenseStrategy::active() {
  return active_;
}

army_t DefenseStrategy::armies_needed() {
  return sum(need);
}

double DefenseStrategy::get_priority() const {
  return -1.0 * sum(need);
}

PlacementVector DefenseStrategy::place_armies(army_t n) {
  PlacementVector pv;
  for (auto r : range(data.region_n)) {
    if (need[r] > 0) {
      army_t armies_used = std::min(n, need[r]);
      n -= armies_used;
      data.occupancy[r] += armies_used;
      pv.push_back({r, armies_used});
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
