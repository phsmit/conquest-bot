#ifndef CONQUEST_DATATYPES_H_
#define CONQUEST_DATATYPES_H_

#include <string>
#include <utility>
#include <vector>

struct Connection {
  int from;
  std::vector<int> to;

  Connection(int from, std::vector<int> to): from(from), to(to) {
  };
};

struct Placement {
  int to;
  int amount;
};

struct Move {
  int from;
  int to;
  int amount;
};


enum Owner {
  ME = 0, OTHER = 1, NEUTRAL = 2
};

struct Update2 {
  int region;
  Owner player;
  int amount;
};

typedef std::vector<int> RegionVector;
typedef std::vector<Placement> PlacementVector;
typedef std::vector<Move> MoveVector;
typedef std::vector<Update2> UpdateVector2;


struct CanonicalGameSetup {
  std::string name_me;
  std::string name_opponent;
  std::vector<int> super_award;
  std::vector<int> region_super;
  std::vector<std::vector<int> > neighbours;
  std::vector<int> init_options;
};

#endif //CONQUEST_DATATYPES_H_