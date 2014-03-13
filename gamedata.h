#ifndef CONQUEST_GAMEDATA_H_
#define CONQUEST_GAMEDATA_H_

#include "datatypes.h"

class GameData {
public:
  const ArmyVector super_award;
  const RegionVector region_super;

  const std::vector<std::vector<bool> > neighbours;
  const std::vector<RegionVector> neighbour_ids;
  const std::vector<std::vector<int> > distances;

  const reg_t super_n;
  const reg_t region_n;

  const RegionVector init_regions;

  std::vector<Owner> owner;
  ArmyVector occupancy;
  std::vector<bool> visible;

  GameData(CanonicalGameSetup setup);

  army_t get_enemy_neighbour_armies(reg_t region) const;

  bool has_enemy_neighbours(reg_t region) const;

  army_t count_neighbour_armies(reg_t region, Owner owner) const;
};

#endif // CONQUEST_GAMEDATA_H_