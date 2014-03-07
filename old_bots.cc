#include "old_bots.h"

RegionVector NoMoveBot::cmd_pick_starting_regions(long t, RegionVector regions) {
  RegionVector rv;
  for (int i = 0; i < 6; ++i) {
    rv.push_back(regions[i]);
  }
  return rv;
}

MoveVector NoMoveBot::cmd_go_attacktransfer(long t) {
  return MoveVector();
}

PlacementVector NoMoveBot::cmd_go_place_armies(long t) {
  return PlacementVector();
}


RegionVector StandYourGroundBot::cmd_pick_starting_regions(long t, RegionVector regions) {
  RegionVector rv;
  for (int i = 0; i < 6; ++i) {
    rv.push_back(regions[i]);
  }
  return rv;
}

MoveVector StandYourGroundBot::cmd_go_attacktransfer(long t) {
  return MoveVector();
}

PlacementVector StandYourGroundBot::cmd_go_place_armies(long t) {
  Placement p = {rv[0], place_armies};
  PlacementVector pv;
  pv.push_back(p);
  return pv;
}

void StandYourGroundBot::cmd_setupmap_regions(SettingVector regions) {
//  for(SettingVector::iterator it = regions.begin(); it != regions.end(); ++it) {
//    rv.push_back(it->first);
//  }
}

void StandYourGroundBot::cmd_updatemap(UpdateVector updates) {
  for (UpdateVector::iterator it = updates.begin(); it != updates.end(); ++it) {
    for (RegionVector::iterator it2 = rv.begin(); it2 != rv.end(); ++it2) {
      if (*it2 == it->region) {
        rv.erase(it2);
      }
    }
    if (it->player == name_me) {
      rv.push_back(it->region);
    }
  }

}
