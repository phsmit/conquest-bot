#ifndef CONQUEST_GAMEDATA_H_
#define CONQUEST_GAMEDATA_H_

#include "datatypes.h"

class GameData {
public:
  const std::vector<int> super_award;
  const std::vector<int> region_super;

  const std::vector<std::vector<bool> > neighbours;
  const std::vector<std::vector<int> > neighbour_ids;
  const std::vector<std::vector<int> > distances;

  const int super_n;
  const int region_n;

  const std::vector<int> init_regions;

  std::vector<std::string> super_region_names;

  std::vector<Owner> owner;
  std::vector<int> occupancy;

  GameData(CanonicalGameSetup setup);

  int get_enemy_neighbour_armies(int region) const;

  bool has_enemy_neighbours(int region) const;

  int count_neighbour_armies(int region, Owner owner) const;
};

#endif // CONQUEST_GAMEDATA_H_