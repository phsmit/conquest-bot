#include "io.h"

#include <sstream>
#include <map>

namespace {
struct Connection2 {
  int from;
  std::vector<int> to;
};

typedef std::vector<Connection2> ConnectionVector2;
typedef std::pair<int, int> Setting2;
typedef std::vector<Setting2> SettingVector2;


SettingVector2 read_pair_list2(std::string data) {
  std::stringstream stream(data);

  SettingVector2 result;
  int k, v;
  while (stream >> k >> v) {
    result.push_back(Setting2(k, v));
  }
  return result;
}

ConnectionVector2 read_neighbour_list2(std::string data) {
  std::stringstream stream(data);

  ConnectionVector2 result;
  int k, n;
  std::string ns;
  while (stream >> k >> ns) {
    std::vector<int> neighbours;
    std::stringstream ss2(ns);
    while (ss2 >> n) {
      neighbours.push_back(n);
      if (ss2.peek() == ',') ss2.ignore();
    }
    Connection2 c = {k, neighbours};
    result.push_back(c);
  }
  return result;
}

std::vector<int> split(std::string data) {
  std::vector<int> parts;
  std::stringstream stream(data);
  int n;
  while (stream >> n) {
    parts.push_back(n);
  }
  return parts;
}

std::map<int, int> make_map(std::vector<int> v) {
  std::map<int, int> map;
  for (size_t i = 0; i < v.size(); ++i) {
    map[v[i]] = i;
  }
  return map;
}

bool read_setup(std::string line, std::string &command, std::string &sub_command, std::string &data) {
  std::stringstream stream(line);
  if (stream >> command >> sub_command) {
    stream.ignore();
    std::getline(stream, data);
    return true;
  }
  return false;
};

void read_until_command(std::istream &in, std::string test_command, std::string &data) {
  while (true) {
    std::string line;
    std::getline(in, line);

    if (!line.compare(0, test_command.size(), test_command)) {
      data = "";
      if (line.size() > test_command.size() + 1) data = line.substr(test_command.size() + 1);
      return;
    }
  }
}
}

CanonicalGameSetup IOManager2::run_setup() {
  // Read all basic setup commands. At the moment the pick_starting_regions command is received, return the current setup
  std::string line;

  std::string command;
  std::string sub_command;
  std::string data;
  while (true) {
    std::getline(std::cin, line);
    if (read_setup(line, command, sub_command, data)) {
      if (command == "settings") {
        if (sub_command == "your_bot") {
          game_setup.name_me = data;
        }
        else if (sub_command == "opponent_bot") {
          game_setup.name_opponent = data;
        }
      }
      else if (command == "setup_map") {
        if (sub_command == "super_regions") {
          SettingVector2 super_regions = read_pair_list2(data);
          super_id.clear();
          for (SettingVector2::iterator it = super_regions.begin(); it != super_regions.end(); ++it) {
            super_id.push_back(it->first);
            game_setup.super_award.push_back(it->second);
          }
        }
        else if (sub_command == "regions") {
          SettingVector2 regions = read_pair_list2(data);
          region_id.clear();
          std::map<int, int> map = make_map(super_id);
          for (SettingVector2::iterator it = regions.begin(); it != regions.end(); ++it) {
            region_id.push_back(it->first);
            game_setup.region_super.push_back(map[it->second]);
          }
        }
        else if (sub_command == "neighbors") {
          ConnectionVector2 connections = read_neighbour_list2(data);
          std::map<int, int> map = make_map(region_id);
          game_setup.neighbours = std::vector<std::vector<int> >(region_id.size());
          for (ConnectionVector2::iterator it = connections.begin(); it != connections.end(); ++it) {
            int r1 = map[it->from];
            for (std::vector<int>::iterator it2 = it->to.begin(); it2 != it->to.end(); ++it2) {
              int r2 = map[*it2];
              game_setup.neighbours[r1].push_back(r2);
              game_setup.neighbours[r2].push_back(r1);
            }
          }
        }
      }
      else if (command == "pick_starting_regions") {
        std::map<int, int> map = make_map(region_id);
        game_setup.init_options = std::vector<int>();
        std::vector<int> in_regions = split(data);
        for (std::vector<int>::iterator it = in_regions.begin(); it != in_regions.end(); ++it) {
          game_setup.init_options.push_back(map[*it]);
        }
        break;
      }
    }
  }

  return game_setup;
}

void IOManager2::run_game_loop(StrategyManager &manager) {
  write_picks(manager.pick_starting_regions());

  std::string data;
  int starting_armies;
  while (true) {
    read_until_command(std::cin, "settings starting_armies", data);
    starting_armies = atoi(data.c_str());

    read_until_command(std::cin, "update_map", data);
    manager.process_updates(parse_updates(data));

    read_until_command(std::cin, "opponent_moves", data);
    manager.process_opponent_moves(parse_moves(data));

    manager.start_new_round(starting_armies);

    write_placements(manager.place_armies());
    write_moves(manager.make_moves());

    if (manager.finished()) {
      break;
    }
  }
}

void IOManager2::write_picks(RegionVector regions) {
  for (std::vector<int>::iterator it = regions.begin(); it != regions.end(); ++it) {
    std::cout << region_id[*it] << " ";
  }
  std::cout << std::endl;
}

void IOManager2::write_moves(MoveVector moves) {
  if (moves.size() == 0) {
    std::cout << "No moves" << std::endl;
    return;
  }

  for (MoveVector::iterator it = moves.begin(); it != moves.end(); ++it) {
    std::cout << game_setup.name_me << " attack/transfer " << region_id[it->from] << " " << region_id[it->to] << " " << it->amount << ", ";
  }

  std::cout << std::endl;

}

void IOManager2::write_placements(PlacementVector placements) {
  bool output_written = false;

  for (PlacementVector::iterator it = placements.begin(); it != placements.end(); ++it) {
    if (it->amount > 0) {
      std::cout << game_setup.name_me << " place_armies " << region_id[it->to] << " " << it->amount << ", ";
      output_written = true;
    }
  }

  if (!output_written) std::cout << "No moves";
  std::cout << std::endl;
}

MoveVector IOManager2::parse_moves(std::string data) {
  //TODO: implement
  return std::vector<Move>();
}

UpdateVector2 IOManager2::parse_updates(std::string data) {
  std::map<int, int> map = make_map(region_id);
  std::map<std::string, Owner> owner_map;
  owner_map[game_setup.name_me] = ME;
  owner_map[game_setup.name_opponent] = OTHER;
  owner_map["neutral"] = NEUTRAL;

  UpdateVector2 result;

  std::stringstream stream(data);

  int region, amount;
  std::string player;
  while (stream >> region >> player >> amount) {
    Update2 u = {map[region], owner_map[player], amount};
    result.push_back(u);
  }

  return result;
}
