#ifndef CONQUEST_STRATEGY_BOT_H_
#define CONQUEST_STRATEGY_BOT_H_

#include "bot.h"
#include "strategy_manager.h"

class StrategyBot : public SavingBaseBot {
public:
  StrategyManager sm;

  StrategyBot(): sm(*this) {
  };

  virtual PlacementVector cmd_go_place_armies(long t);

  virtual MoveVector cmd_go_attacktransfer(long t);

  virtual RegionVector cmd_pick_starting_regions(long t, RegionVector regions);
};


#endif //CONQUEST_STRATEGY_BOT_H_
