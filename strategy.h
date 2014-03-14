#pragma once

#include "gamedata.h"
#include "gen/names-gen.h"


// IDEAS for strategies
//class FootholdStrategy
//class KillAllEnemiesStrategy

class Strategy {
protected:
  GameData &data;
  bool active_;
  army_t armies_need;

public:
  str name;

  Strategy(GameData &data): data(data) {
  }
  virtual ~Strategy() {}

  virtual bool active() const {
    return active_;
  }

  virtual army_t armies_needed() const {
    return armies_need;
  }

  virtual double get_priority() const {
    return -1.0 * armies_need;
  }

  virtual void update() = 0;
  virtual PlacementVector place_armies(army_t n) = 0;
  virtual MoveVector do_moves(ArmyVector &armies) = 0;
};

class BasicStrategy : public Strategy {
public:
  BasicStrategy(GameData &data): Strategy(data) {
    name = "Basic strategy";
    active_ = true;
    armies_need = 10000;
  }

  virtual void update() {};
  virtual PlacementVector place_armies(army_t n);
  virtual MoveVector do_moves(ArmyVector &armies);

  MoveVector generate_attacks(reg_t region, ArmyVector &armies);
};

class AquireContinentStrategy : public Strategy {
private:
  static constexpr double WIN_PROB = 0.9;
  const reg_t super_region;
  ArmyVector army_need;

public:
  AquireContinentStrategy(GameData &data, reg_t super_region): Strategy(data), super_region(super_region) {
    name = "Aquire " + SUPER_REGION_NAMES[super_region];
  }

  virtual void update();
  virtual PlacementVector place_armies(army_t n);
  virtual MoveVector do_moves(ArmyVector &armies);

  army_t get_local_neighbour_armies(reg_t region);
  MoveVector generate_attacks(reg_t region, ArmyVector &armies);
};

class DefendContinentStrategy : public Strategy {
private:
  static constexpr double DEFENSE_PROB = 0.7;
  const reg_t super_region;
  ArmyVector num_defenders;

public:
  DefendContinentStrategy(GameData &data, reg_t super_region): Strategy(data), super_region(super_region) {
    name = "Defend " + SUPER_REGION_NAMES[super_region];
  }

  virtual void update();
  virtual PlacementVector place_armies(army_t n);
  virtual MoveVector do_moves(ArmyVector &armies);
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
  virtual PlacementVector place_armies(army_t n);
  virtual MoveVector do_moves(ArmyVector &armies);
};
