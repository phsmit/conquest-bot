#ifndef CONQUEST_OLD_BOTS_H_
#define CONQUEST_OLD_BOTS_H_

#include "bot.h"

class NoMoveBot : public BaseBot {
public:
  RegionVector cmd_pick_starting_regions(long t, RegionVector regions);

  MoveVector cmd_go_attacktransfer(long t);

  PlacementVector cmd_go_place_armies(long t);
};

class StandYourGroundBot : public BaseBot {
public:
  RegionVector rv;

  StandYourGroundBot() {};

  void cmd_setupmap_regions(SettingVector regions);
  void cmd_updatemap(UpdateVector updates);

  RegionVector cmd_pick_starting_regions(long t, RegionVector regions);

  MoveVector cmd_go_attacktransfer(long t);

  PlacementVector cmd_go_place_armies(long t);
};


#endif  // CONQUEST_OLD_BOTS_H_