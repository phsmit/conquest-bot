#ifndef CONQUEST_BASIC_ATTACK_BOT_H_
#define CONQUEST_BASIC_ATTACK_BOT_H_

#include "bot.h"

#include <map>

enum Owner {ME = 0, OTHER = 1, NEUTRAL = 2};

class BasicAttackBot : public BaseBot {
public:
  std::vector<int> super_region_ids;
  std::vector<int> super_region_reward;

  std::vector<int> region_ids;
  std::vector<int> region_super_region;
  std::map<int, int> region_map;

  std::map<std::string, Owner> owner_map;
  std::vector<Owner> owner;
  std::vector<int> occupancy;

  std::vector<std::vector<bool> > neighbours;

  BasicAttackBot() {};

  virtual void cmd_setupmap_superregions(SettingVector regions);
  virtual void cmd_setupmap_regions(SettingVector regions);
  virtual void cmd_setupmap_neighbours(ConnectionVector connections);
  virtual void cmd_updatemap(UpdateVector updates);
  virtual void cmd_opponentmoves(MoveVector moves);
  virtual PlacementVector cmd_go_place_armies(long t);
  virtual MoveVector cmd_go_attacktransfer(long t);
  virtual RegionVector cmd_pick_starting_regions(long t, RegionVector regions);

  int get_enemy_neighbour_armies(int region);
  bool has_enemy_neighbours(int region);

};

#endif  // CONQUEST_BASIC_ATTACK_BOT_H_