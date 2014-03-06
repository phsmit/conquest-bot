#ifndef CONQUEST_BOT_H_
#define CONQUEST_BOT_H_

#include <string>
#include <vector>

struct Connection {
  short from;
  std::vector<short> to;
};

struct Placement {
  short to;
  short amount;
};

struct Move {
  short from;
  short to;
  short amount;
};

struct Update {
  short region;
  std::string player;
  short amount;
};

typedef std::vector<short> RegionVector;
typedef std::vector<Connection> ConnectionVector;
typedef std::vector<Placement> PlacementVector;
typedef std::vector<Move> MoveVector;
typedef std::vector<Update> UpdateVector;
typedef std::pair<int, int> Setting;
typedef std::vector<Setting> SettingVector;


class BaseBot {

public:
  std::string name_opponent;
  std::string name_me;
  int turn;
  int place_armies;

  BaseBot() : turn(0), place_armies(0) {}

  void cmd_settings_opponent(std::string name) {
    name_opponent = name;
  }

  void cmd_settings_yourbot(std::string name) {
    name_me = name;
  }

  void cmd_settings_starting_armies(int num_armies) {
    turn++;
    place_armies = num_armies;
  }

  virtual void cmd_setupmap_superregions(SettingVector regions) {};

  virtual void cmd_setupmap_regions(SettingVector regions) {  };

  virtual void cmd_setupmap_neighbours(ConnectionVector connections) {  };

  virtual void cmd_updatemap(UpdateVector updates) {  };

  virtual void cmd_opponentmoves(MoveVector moves) {  };

  virtual PlacementVector cmd_go_place_armies(long t) = 0;

  virtual MoveVector cmd_go_attacktransfer(long t) = 0;

  virtual RegionVector cmd_pick_starting_regions(long t, RegionVector regions) = 0;

};


class NoMoveBot : public BaseBot {
public:
  RegionVector cmd_pick_starting_regions(long t, RegionVector regions);

  MoveVector cmd_go_attacktransfer(long t);

  PlacementVector cmd_go_place_armies(long t);
};


#endif  // CONQUEST_BOT_H_