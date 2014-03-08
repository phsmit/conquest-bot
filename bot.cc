#include "bot.h"

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

  for (ConnectionVector::iterator it = connections.begin(); it != connections.end(); ++it) {
    for (std::vector<int>::iterator it2 = it->to.begin(); it2 != it->to.end(); ++it2) {
      int p1 = region_map[it->from];
      int p2 = region_map[*it2];

      neighbours[p1][p2] = true;
      neighbours[p2][p1] = true;
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
