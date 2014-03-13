#ifndef CONQUEST_STRATEGY_H_
#define CONQUEST_STRATEGY_H_

#include "gamedata.h"

class Strategy {
protected:
  GameData &bot;

public:
  str name;

  Strategy(GameData &bot): bot(bot) {
  }

  virtual bool active() {
    return false;
  }

  virtual void update() {
  };

  virtual army_t armies_needed() {
    return 0;
  }

  virtual double get_priority() const {
    return 0.0;
  }

  virtual PlacementVector place_armies(army_t n) {
    return PlacementVector();
  }

  virtual MoveVector do_moves(ArmyVector &armies) {
    return MoveVector();
  }
};

class DefenseStrategy : public Strategy {
private:
  double DEFENSE_PROB;
  army_t expected_increase;
  ArmyVector need;
  bool active_;

public:
  DefenseStrategy(GameData &bot): Strategy(bot) {
    name = "Defense strategy";
    DEFENSE_PROB = 0.5;
    expected_increase = 5;
  }

  virtual bool active();

  virtual void update();

  virtual army_t armies_needed();

  virtual double get_priority() const;

  virtual PlacementVector place_armies(army_t n);
};

class FootholdStrategy : public Strategy {
public:
  FootholdStrategy(GameData &bot, reg_t super_region): Strategy(bot) {
    name = "INACTIVE Foothold " + bot.super_region_names[super_region];
  }
};

class AquireContinentStrategy : public Strategy {
private:
  double WIN_PROB;
  const reg_t super_region;
  army_t need;

  bool active_;

  ArmyVector army_need;

public:
  AquireContinentStrategy(GameData &bot, reg_t super_region): Strategy(bot), super_region(super_region) {
    WIN_PROB = 0.9;

    name = "Aquire " + bot.super_region_names[super_region];
  }

  virtual bool active() {
    return active_;
  }

  virtual void update();

  virtual army_t armies_needed();

  virtual double get_priority() const {
    return -1.0 * need;
  }

  virtual PlacementVector place_armies(army_t n);

  virtual MoveVector do_moves(ArmyVector &armies);

  army_t get_local_neighbour_armies(reg_t region);

  MoveVector generate_attacks(reg_t region, ArmyVector &armies);

};

class DefendContinentStrategy : public Strategy {
private:
  army_t need;
  bool active_;

public:

  DefendContinentStrategy(GameData &bot, reg_t super_region): Strategy(bot) {
    name = "Defend " + bot.super_region_names[super_region];
  }

  virtual bool active() {
    return active_;
  }

  virtual void update();

  virtual army_t armies_needed();

  virtual double get_priority() const {
    return -1.0 * need;
  }

  virtual PlacementVector place_armies(army_t n);

  virtual MoveVector do_moves(ArmyVector &armies);

};

class KillAllEnemiesStrategy : public Strategy {
public:
  KillAllEnemiesStrategy(GameData &bot): Strategy(bot) {
    name = "INACTIVE KILLALLENEMIES ";
  }
};

class BasicStrategy : public Strategy {
public:
  BasicStrategy(GameData &bot): Strategy(bot) {
    name = "Basic strategy";
  }

  virtual bool active();

  virtual void update();

  virtual army_t armies_needed();

  virtual double get_priority() const;

  virtual PlacementVector place_armies(army_t n);

  virtual MoveVector do_moves(ArmyVector &armies);

  MoveVector generate_attacks(reg_t region, ArmyVector &armies);
};


#endif //CONQUEST_STRATEGY_H_
