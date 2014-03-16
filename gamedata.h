#pragma once

#include "datatypes.h"

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

  std::vector<Owner> owner;
  ArmyVector occupancy;
  std::vector<bool> visible;

  std::vector<bool> super_totally_owned;
  std::vector<bool> super_absent;

  GameData(CanonicalGameSetup setup);

  army_t get_enemy_neighbour_armies(reg_t region) const;

  bool has_enemy_neighbours(reg_t region) const;

  army_t count_neighbour_armies(reg_t region, Owner owner) const;
};