#ifndef CONQUEST_IO_H_
#define CONQUEST_IO_H_

#include "strategy_manager.h"

class IOManager2 {
private:
  std::vector<int> region_id;
  std::vector<int> super_id;
  CanonicalGameSetup game_setup;

  void write_picks(std::vector<int> regions);

  void write_moves(std::vector<Move> moves);

  void write_placements(PlacementVector placements);

  MoveVector parse_moves(std::string data);

  UpdateVector2 parse_updates(std::string data);


public:
  CanonicalGameSetup run_setup();

  void run_game_loop(StrategyManager &manager);
};

#endif  // CONQUEST_IO_H_