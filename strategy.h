#ifndef CONQUEST_STRATEGY_H_
#define CONQUEST_STRATEGY_H_

#include "bot.h"

class Strategy {
protected:
  SavingBaseBot &bot;

public:
  std::string name;
  Strategy(SavingBaseBot &bot): bot(bot) {
  }

  virtual bool active() {
    return false;
  }

  virtual void update() {
  };

  virtual int armies_needed() {
    return 0;
  }

  virtual double get_priority() const {
    return 0.0;
  }

  virtual PlacementVector place_armies(int n) {
    return PlacementVector();
  }

  virtual MoveVector do_moves(std::vector<int>& armies) {
    return MoveVector();
  }
};

class DefenseStrategy : public Strategy {
private:
  double DEFENSE_PROB;
  int expected_increase;
  std::vector<int> need;
  bool active_;

public:
  DefenseStrategy(SavingBaseBot &bot): Strategy(bot) {
    name = "Defense strategy";
    DEFENSE_PROB = 0.5;
    expected_increase = 5;
  }

  virtual bool active();

  virtual void update();

  virtual int armies_needed();

  virtual double get_priority() const;

  virtual PlacementVector place_armies(int n);
};

class FootholdStrategy : public Strategy {
public:
  FootholdStrategy(SavingBaseBot &bot, int super_region): Strategy(bot) {
    name = "INACTIVE Foothold " + bot.super_region_names[super_region];
  }
};

class AquireContinentStrategy : public Strategy {
private:
  double WIN_PROB;
  const int super_region;
//  int enemy_armies;
//  int regions_missing;
//  int my_surplus_armies;
  int need;

  bool active_;

  std::vector<int> army_need;

public:
  AquireContinentStrategy(SavingBaseBot &bot, int super_region): Strategy(bot), super_region(super_region) {
    WIN_PROB = 0.9;

    name = "Aquire " + bot.super_region_names[super_region];
  }

  virtual bool active() {return active_;}
  virtual void update();
  virtual int armies_needed();

  virtual double get_priority() const {return -1.0 * need;}

  virtual PlacementVector place_armies(int n);
  virtual MoveVector do_moves(std::vector<int>& armies);

  int get_local_neighbour_armies(int region);

};

class DefendContinentStrategy : public Strategy {
public:
  DefendContinentStrategy(SavingBaseBot &bot, int super_region): Strategy(bot) {
    name = "INACTIVE Defend " + bot.super_region_names[super_region];
  }
};

class KillAllEnemiesStrategy : public Strategy {
public:
  KillAllEnemiesStrategy(SavingBaseBot &bot): Strategy(bot) {
    name = "INACTIVE KILLALLENEMIES ";
  }
};

class BasicStrategy : public Strategy {
public:
  BasicStrategy(SavingBaseBot &bot): Strategy(bot) {
    name = "Basic strategy";
  }

  virtual bool active();

  virtual void update();

  virtual int armies_needed();

  virtual double get_priority() const;

  virtual PlacementVector place_armies(int n);

  virtual MoveVector do_moves(std::vector<int>& armies);

  MoveVector generate_attacks(int region, std::vector<int>& armies);
};


#endif //CONQUEST_STRATEGY_H_
