#ifndef CONQUEST_BOT_H_
#define CONQUEST_BOT_H_

#include <map>
#include <string>
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

struct Update {
  int region;
  std::string player;
  int amount;

  Update(int region, std::string player, int amount): region(region), player(player), amount(amount) {
  };
};

typedef std::vector<int> RegionVector;
typedef std::vector<Connection> ConnectionVector;
typedef std::vector<Placement> PlacementVector;
typedef std::vector<Move> MoveVector;
typedef std::vector<Update> UpdateVector;
typedef std::pair<int, int> Setting;
typedef std::vector<Setting> SettingVector;

enum Owner {
  ME = 0, OTHER = 1, NEUTRAL = 2
};

class BaseBot {

public:
  std::string name_opponent;
  std::string name_me;
  int turn;
  int place_armies;

  BaseBot(): turn(0), place_armies(0) {
  }

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

  virtual void cmd_setupmap_superregions(SettingVector regions) {
  };

  virtual void cmd_setupmap_regions(SettingVector regions) {
  };

  virtual void cmd_setupmap_neighbours(ConnectionVector connections) {
  };

  virtual void cmd_updatemap(UpdateVector updates) {
  };

  virtual void cmd_opponentmoves(MoveVector moves) {
  };

  virtual PlacementVector cmd_go_place_armies(long t) = 0;

  virtual MoveVector cmd_go_attacktransfer(long t) = 0;

  virtual RegionVector cmd_pick_starting_regions(long t, RegionVector regions) = 0;

};

class SavingBaseBot : public BaseBot {
public:
  std::vector<int> super_region_ids;
  std::vector<int> super_region_reward;

  std::vector<int> region_ids;
  std::vector<int> region_super_region;
  std::map<int, int> region_map;

  std::map<std::string, Owner> owner_map;
  std::vector<Owner> owner;
  std::vector<int> occupancy;

  std::vector<std::vector<bool> > neighbours;
  std::vector<std::vector<int> > distances;

  std::vector<std::string> super_region_names;

  SavingBaseBot() {
    super_region_names.push_back("North America");
    super_region_names.push_back("South America");
    super_region_names.push_back("Europe");
    super_region_names.push_back("Africa");
    super_region_names.push_back("Asia");
    super_region_names.push_back("Australia");
  }

  virtual void cmd_setupmap_superregions(SettingVector regions);

  virtual void cmd_setupmap_regions(SettingVector regions);

  virtual void cmd_setupmap_neighbours(ConnectionVector connections);

  virtual void cmd_updatemap(UpdateVector updates);

  int get_enemy_neighbour_armies(int region);

  bool has_enemy_neighbours(int region);

};


#endif  // CONQUEST_BOT_H_