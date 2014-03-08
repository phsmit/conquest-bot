#ifndef CONQUEST_STRATEGY_H_
#define CONQUEST_STRATEGY_H_

#include "bot.h"

class Strategy {
protected:
  BaseBot &bot;
public:
  Strategy(BaseBot &bot): bot(bot) {
  }

  void update_needs();

  int get_need();

  PlacementVector place_armies(int n);

  MoveVector do_moves();
};

#endif //CONQUEST_STRATEGY_H_