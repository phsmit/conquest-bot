#pragma once

#include "strategy_manager.h"

class IOManager {
private:
  RegionVector region_id;
  RegionVector super_id;
  CanonicalGameSetup game_setup;

  void write_picks(RegionVector regions);

  void write_moves(MoveVector moves);

  void write_placements(PlacementVector placements);

  MoveVector parse_moves(str data);

  UpdateVector parse_updates(str data);


public:
  CanonicalGameSetup run_setup();

  void run_game_loop(StrategyManager &manager);
};
