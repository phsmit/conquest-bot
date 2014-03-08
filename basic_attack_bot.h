#ifndef CONQUEST_BASIC_ATTACK_BOT_H_
#define CONQUEST_BASIC_ATTACK_BOT_H_

#include "bot.h"

class BasicAttackBot : public SavingBaseBot {
public:
  BasicAttackBot() {};

  virtual PlacementVector cmd_go_place_armies(long t);
  virtual MoveVector cmd_go_attacktransfer(long t);
  virtual RegionVector cmd_pick_starting_regions(long t, RegionVector regions);

  int get_enemy_neighbour_armies(int region);
  bool has_enemy_neighbours(int region);

  MoveVector generate_attacks(int region);
};

#endif  // CONQUEST_BASIC_ATTACK_BOT_H_