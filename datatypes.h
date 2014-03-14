#pragma once

#include <cstdint>

#include <limits>
#include <string>
#include <utility>
#include <vector>

typedef uint_fast16_t reg_t;
typedef uint_fast16_t army_t;
typedef uint_fast16_t distance_t;

const distance_t UNREACHABLE = std::numeric_limits<distance_t>::max();
const reg_t UNKNOWN_REGION = std::numeric_limits<reg_t>::max();

typedef std::string str;

enum Owner {
  ME = 0, OTHER = 1, NEUTRAL = 2
};

struct Move {
  reg_t from;
  reg_t to;
  army_t amount;
};

struct Placement {
  reg_t to;
  army_t amount;
};

struct Update {
  reg_t region;
  Owner player;
  army_t amount;
};

typedef std::vector<army_t> ArmyVector;
typedef std::vector<reg_t> RegionVector;
typedef std::vector<Placement> PlacementVector;
typedef std::vector<Move> MoveVector;
typedef std::vector<Update> UpdateVector;


struct CanonicalGameSetup {
  str name_me;
  str name_opponent;
  ArmyVector super_award;
  RegionVector region_super;
  std::vector<RegionVector> neighbours;
  RegionVector init_options;
};
