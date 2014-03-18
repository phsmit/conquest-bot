#include "strategy.h"

#include "prob_math.h"

#include <set>
#include "util.h"


army_t BasicStrategy::place_armies2(army_t n) {
  //place armies on one region, the one with the most neighbour armies
  army_t max_neighbour_armies = 0;
  reg_t max_region = 0;

  for (auto r : range(data.region_n)) {
    if (data.cur_state().owner[r] != ME) continue;
    army_t army_neighbours = data.get_enemy_neighbour_armies(r);
    if (army_neighbours > max_neighbour_armies) {
      max_region = r;
      max_neighbour_armies = army_neighbours;
    }
  }
  data.cur_plan().placements.push_back({max_region, n});

  // add the armies to occupancy
  data.cur_state().occupancy[max_region] += n;
  return n;
}

unsigned BasicStrategy::do_moves2() {
  unsigned num_moves;

  for (auto r : range(data.region_n)) {
    if (data.cur_state().owner[r] != ME) continue;
    if (data.cur_plan().available_armies[r] == 0) continue;

    if (data.has_enemy_neighbours(r)) {
      MoveVector attack_moves = generate_attacks(r, data.cur_plan().available_armies);
      num_moves += attack_moves.size();

      data.cur_plan().moves.insert(data.cur_plan().moves.end(), attack_moves.begin(), attack_moves.end());
    } else {
      reg_t closest_enemy_region = UNKNOWN_REGION;
      int enemy_distance = data.region_n;
      for (auto possible_region : range(data.region_n)) {
        if (data.cur_state().owner[possible_region] == ME) continue;
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

      data.cur_plan().moves.push_back({r, target_region, data.cur_plan().available_armies[r]});
      num_moves++;
      data.cur_plan().available_armies[r] = 0;
    }
  }
  return num_moves;

}

MoveVector BasicStrategy::generate_attacks(reg_t r, ArmyVector &armies) {
  double MIN_WIN_PROB = 0.8;
  std::set<reg_t> targets;

  MoveVector moves;
  std::vector<reg_t> regions_done;

  for (auto neigbhour : data.neighbour_ids[r]) {
    if (data.cur_state().owner[neigbhour] == ME) continue;
    targets.insert(neigbhour);
  }

  while (targets.size() > 0) {
    reg_t best_attack_region = UNKNOWN_REGION;
    army_t best_num_enemies = 0;
    for (auto target : targets) {
      if (conquest::internal::get_win_prob(armies[r], data.cur_state().occupancy[target]) > MIN_WIN_PROB) {
        if (data.cur_state().occupancy[target] > best_num_enemies) {
          best_num_enemies = data.cur_state().occupancy[target];
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
    if (data.cur_state().owner[target] == OTHER) all_other_attacked = false;
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

  active = false;
  bool enemies_present = false;

  for (auto r : range(data.region_n)) {
    if (data.region_super[r] != super_region) continue;
    if (data.cur_state().owner[r] != ME) {
      enemies_present = true;
    } else {
      active = true;
      unused_armies[r] = data.cur_state().occupancy[r] - 1;
    }
  }

  if (!enemies_present) active = false;
  if (!active) return;

  for (auto r : range(data.region_n)) {
    if (data.region_super[r] != super_region) continue;
    if (data.cur_state().owner[r] == ME) continue;

    army_t armies_needed = conquest::internal::attackers_needed(data.cur_state().occupancy[r], WIN_PROB);
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
      indirect_enemies += data.cur_state().occupancy[r];
    }
  }

  current_need = sum(army_need);
  total_need = conquest::internal::attackers_needed(indirect_enemies, WIN_PROB) + current_need;
}

army_t AquireContinentStrategy::place_armies2(army_t n) {
  army_t num_placed = 0;
  for (auto r : range(data.region_n)) {
    if (data.region_super[r] != super_region) continue;
    if (army_need[r] > 0) {
      army_t armies_used = std::min(n, army_need[r]);
      army_need[r] -= armies_used;
      n -= armies_used;
      data.cur_state().occupancy[r] += armies_used;
      data.cur_plan().placements.push_back({r, armies_used});
      num_placed += armies_used;
    }
  }

  if (n == 0) return num_placed;

  for (auto r : range(data.region_n)) {
    if (data.cur_state().owner[r] != ME) continue;
    if (data.region_super[r] != super_region) continue;

    data.cur_plan().placements.push_back({r, n});
    num_placed += n;
    break;
  }
  return num_placed;
}

unsigned AquireContinentStrategy::do_moves2() {
  unsigned num_moves = 0;

  for (auto r : range(data.region_n)) {
    if (data.cur_state().owner[r] != ME) continue;
    if (data.region_super[r] != super_region) continue;
    if (data.cur_plan().available_armies[r] == 0) continue;

    if (data.has_enemy_neighbours(r)) {
      MoveVector attack_moves = generate_attacks(r, data.cur_plan().available_armies);
      data.cur_plan().moves.insert(data.cur_plan().moves.end(), attack_moves.begin(), attack_moves.end());
    } else {
      reg_t closest_enemy_region = UNKNOWN_REGION;
      int enemy_distance = data.region_n;
      for (auto possible_region : range(data.region_n)) {
        if (data.cur_state().owner[possible_region] == ME) continue;
        if (data.distances[r][possible_region] < enemy_distance) { // TODO bug!!! (not fixed yet because of refactoring)
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

      data.cur_plan().moves.push_back({r, target_region, data.cur_plan().available_armies[r]});
      data.cur_plan().available_armies[r] = 0;
    }
  }
  return num_moves;
}

MoveVector AquireContinentStrategy::generate_attacks(reg_t r, ArmyVector &armies) {
  double MIN_WIN_PROB = 0.8;
  std::set<reg_t> targets;

  MoveVector moves;
  RegionVector regions_done;

  for (auto neighbour : data.neighbour_ids[r]) {
    if (data.cur_state().owner[neighbour] == ME) continue;
    if (data.region_super[neighbour] != super_region) continue;
    targets.insert(neighbour);
  }

  while (targets.size() > 0) {
    reg_t best_attack_region = UNKNOWN_REGION;
    army_t best_num_enemies = 0;
    for (auto target : targets) {
      if (conquest::internal::get_win_prob(armies[r], data.cur_state().occupancy[target]) > MIN_WIN_PROB) {
        if (data.cur_state().occupancy[target] > best_num_enemies) {
          best_num_enemies = data.cur_state().occupancy[target];
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
    if (data.cur_state().owner[target] == OTHER) all_other_attacked = false;
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
    if (data.cur_state().owner[r] == ME) continue;
    if (data.region_super[r] != super_region) continue;
    num_neighbours += data.cur_state().occupancy[r];
  }
  return num_neighbours;
}

void DefenseStrategy::update() {
  active = false;
  need = ArmyVector(data.region_n, 0);
  for (auto r : range(data.region_n)) {
    if (data.cur_state().owner[r] != ME) continue;
    for (auto nr : range(data.region_n)) {
      if (data.cur_state().owner[nr] != OTHER) continue;
      if (!data.neighbours[r][nr]) continue;

      army_t attackers = data.cur_state().occupancy[nr] + expected_increase - 1;
      army_t defenders = conquest::internal::defenders_needed(attackers, DEFENSE_PROB);
      if (data.cur_state().occupancy[r] + need[r] < defenders) {
        need[r] = defenders - data.cur_state().occupancy[r];
        active = true;
      }
    }
  }
  total_need = current_need = sum(need);
}

void DefendContinentStrategy::update() {
  active = true;
  num_defenders = ArmyVector(data.region_n, 0);
  total_need = 0;

  for (auto region : data.regions_by_super[super_region]) {
    if (data.cur_state().owner[region] != ME) {
      active = false;
      return;
    }

    army_t max_attackers = 0;
    for (auto neighbour : data.neighbour_ids[region]) {
      if (data.cur_state().owner[neighbour] == OTHER) {
        max_attackers = std::max(max_attackers, data.cur_state().occupancy[neighbour] + 5 - 1);
      }
    }
    army_t defenders_needed = conquest::internal::defenders_needed(max_attackers, DEFENSE_PROB);
    //TODO take into account to move first units from neighbouring countries
    num_defenders[region] = defenders_needed;
    if (defenders_needed > data.cur_state().occupancy[region]) {
      total_need += defenders_needed - data.cur_state().occupancy[region];
    }
  }
  current_need = total_need;
}

army_t DefendContinentStrategy::place_armies2(army_t n) {
  army_t num_placed = 0;
  for (auto region : data.regions_by_super[super_region]) {
    if (num_defenders[region] > data.cur_state().occupancy[region]) {
      army_t place_armies = std::min(n, num_defenders[region] - data.cur_state().occupancy[region]);
      n -= place_armies;
      data.cur_plan().placements.push_back({region, place_armies});
      num_placed += place_armies;
    }
  }
  return num_placed;
}

unsigned DefendContinentStrategy::do_moves2() {
  return 0;
}

army_t DefenseStrategy::place_armies2(army_t n) {
  army_t num_placed = 0;
  for (auto r : range(data.region_n)) {
    if (need[r] > 0) {
      army_t armies_used = std::min(n, need[r]);
      n -= armies_used;
      data.cur_state().occupancy[r] += armies_used;
      data.cur_plan().placements.push_back({r, armies_used});
      num_placed += armies_used;
    }
  }

  return num_placed;
}

unsigned DefenseStrategy::do_moves2() {
  return 0;
}