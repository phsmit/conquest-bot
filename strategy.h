#ifndef CONQUEST_STRATEGY_H_
#define CONQUEST_STRATEGY_H_

#include "bot.h"

class Strategy {
protected:
  SavingBaseBot &bot;
public:
  Strategy(SavingBaseBot &bot): bot(bot) {
  }

  virtual bool active() {
    return false;
  }
  virtual void update() {};

  virtual double get_cost() {
    return 0.0;
  }
  virtual double get_priority() {
    return 0.0;
  }

  virtual PlacementVector place_armies(int n) {
    return PlacementVector();
  }

  virtual MoveVector do_moves() {
    return MoveVector();
  }
};

class FootholdStrategy : public Strategy {
public:
  FootholdStrategy(SavingBaseBot & bot, int super_region): Strategy(bot) {}
};

class AquireContinentStrategy : public Strategy {
public:
  AquireContinentStrategy(SavingBaseBot & bot, int super_region): Strategy(bot) {}
};

class DefendContinentStrategy : public Strategy {
public:
  DefendContinentStrategy(SavingBaseBot & bot, int super_region): Strategy(bot) {}
};

class KillAllEnemiesStrategy : public Strategy {
public:
  KillAllEnemiesStrategy(SavingBaseBot & bot): Strategy(bot) {}
};


#endif //CONQUEST_STRATEGY_H_