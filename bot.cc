#include "bot.h"

namespace {
bool has_negative_values(std::vector<int> v) {
  for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    if (*it < 0) return true;
  }
  return false;
}
}

void SavingBaseBot::cmd_setupmap_superregions(SettingVector regions) {
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

void SavingBaseBot::cmd_setupmap_regions(SettingVector regions) {
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

void SavingBaseBot::cmd_setupmap_neighbours(ConnectionVector connections) {
  std::vector<bool> neighbours_row(region_ids.size(), false);
  neighbours = std::vector<std::vector<bool> >(region_ids.size(), neighbours_row);
  neighbour_ids = std::vector<std::vector<int> >(region_ids.size());

  for (ConnectionVector::iterator it = connections.begin(); it != connections.end(); ++it) {
    for (std::vector<int>::iterator it2 = it->to.begin(); it2 != it->to.end(); ++it2) {
      int p1 = region_map[it->from];
      int p2 = region_map[*it2];

      neighbours[p1][p2] = true;
      neighbours[p2][p1] = true;

      neighbour_ids[p1].push_back(p2);
      neighbour_ids[p2].push_back(p1);
    }
  }

  std::vector<int> distance_row(region_ids.size(), -1);
  distances = std::vector<std::vector<int> >(region_ids.size(), distance_row);

  for (int region = 0; region < region_ids.size(); ++region) {
    distances[region][region] = 0;
    while (has_negative_values(distances[region])) {
      for (int test_region = 0; test_region < region_ids.size(); ++test_region) {
        if (distances[region][test_region] >= 0) continue;
        for (int neigh_region = 0; neigh_region < region_ids.size(); ++neigh_region) {
          if (distances[region][neigh_region] >= 0) {
            distances[region][test_region] = distances[region][neigh_region] + 1;
            distances[test_region][region] = distances[region][test_region];
          }
        }
      }
    }
  }


}

void SavingBaseBot::cmd_updatemap(UpdateVector updates) {
  for (UpdateVector::iterator it = updates.begin(); it != updates.end(); ++it) {
    int real_region = region_map[it->region];
    owner[real_region] = owner_map[it->player];
    occupancy[real_region] = it->amount;
  }
}


int SavingBaseBot::get_enemy_neighbour_armies(int region) {
  int num_neighbours = 0;
  for (size_t r = 0; r < region_ids.size(); ++r) {
    if (!neighbours[region][r]) continue;
    if (owner[r] == ME) continue;
    num_neighbours += occupancy[r];
  }
  return num_neighbours;
}

bool SavingBaseBot::has_enemy_neighbours(int region) {
  for (size_t r = 0; r < region_ids.size(); ++r) {
    if (!neighbours[region][r]) continue;
    if (owner[r] != ME) return true;
  }
  return false;
}