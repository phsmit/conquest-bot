#pragma once

#include "gamedata.h"
#include "gen/names-gen.h"

// IDEAS for strategies
//class FootholdStrategy
//class KillAllEnemiesStrategy

class Strategy {
protected:
  GameData &data;


  bool active;
  army_t total_need;
  army_t current_need;
  double move_priority;

public:
  str name;

  Strategy(GameData &data) : data(data) { }
  virtual ~Strategy() {}

  void start_round() {
//    cur_state = states.back();
//    cur_plan = plans.back();
  }

  virtual bool is_active() const {
    return active;
  }

  virtual army_t total_army_need() const {
    return total_need;
  }

  virtual army_t current_army_need() const {
    return current_need;
  }

  virtual double get_move_priority() const {
    return -1.0 * total_need;
  }

  virtual void update() = 0;
  virtual army_t place_armies2(army_t n) = 0;
  virtual unsigned do_moves2() = 0;
};

class SuperRegionStrategy : public Strategy {
protected:
  reg_t super_region;
public:
  SuperRegionStrategy(GameData &data, reg_t super_region) : Strategy(data), super_region(super_region) {}
};

class BasicStrategy : public Strategy {
public:
  BasicStrategy(GameData &data): Strategy(data) {
    name = "Basic strategy";
    active = true;
    total_need = 10000;
    current_need = 10000;
  }

  virtual void update() {};
  virtual army_t place_armies2(army_t n);
  virtual unsigned do_moves2();

  MoveVector generate_attacks(reg_t region, ArmyVector &armies);
};

class AquireContinentStrategy : public SuperRegionStrategy {
private:
  static constexpr double WIN_PROB = 0.9;
  ArmyVector army_need;

public:
  AquireContinentStrategy(GameData &data, reg_t super_region): SuperRegionStrategy(data, super_region) {
    name = "Aquire " + SUPER_REGION_NAMES[super_region];
  }

  virtual void update();
  virtual army_t place_armies2(army_t n);
  virtual unsigned do_moves2();

  army_t get_local_neighbour_armies(reg_t region);
  MoveVector generate_attacks(reg_t region, ArmyVector &armies);
};

class DefendContinentStrategy : public SuperRegionStrategy {
private:
  static constexpr double DEFENSE_PROB = 0.7;
  ArmyVector num_defenders;

public:
  DefendContinentStrategy(GameData &data, reg_t super_region): SuperRegionStrategy(data, super_region) {
    name = "Defend " + SUPER_REGION_NAMES[super_region];
  }

  virtual void update();
  virtual army_t place_armies2(army_t n);
  virtual unsigned do_moves2();
};

class DefenseStrategy : public Strategy {
private:
  static constexpr double DEFENSE_PROB = 0.5;
  army_t expected_increase;
  ArmyVector need;

public:
  DefenseStrategy(GameData &data): Strategy(data) {
    name = "Defense strategy";
    expected_increase = 5;
  }

  virtual void update();
  virtual army_t place_armies2(army_t n);
  virtual unsigned do_moves2();
};
