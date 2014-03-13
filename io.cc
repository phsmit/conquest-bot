#include "io.h"

#include <sstream>
#include <map>

namespace {
struct Connection {
  reg_t from;
  RegionVector to;
};

typedef std::vector<Connection> ConnectionVector;
typedef std::pair<reg_t, reg_t> Setting;
typedef std::vector<Setting> SettingVector;


SettingVector read_pair_list(str data) {
  std::stringstream stream(data);

  SettingVector result;
  reg_t k, v;
  while (stream >> k >> v) {
    result.push_back(Setting(k, v));
  }
  return result;
}

ConnectionVector read_neighbour_list(str data) {
  std::stringstream stream(data);

  ConnectionVector result;
  reg_t k, n;
  str ns;
  while (stream >> k >> ns) {
    RegionVector neighbours;
    std::stringstream ss2(ns);
    while (ss2 >> n) {
      neighbours.push_back(n);
      if (ss2.peek() == ',') ss2.ignore();
    }
    Connection c = {k, neighbours};
    result.push_back(c);
  }
  return result;
}

RegionVector split(str data) {
  RegionVector parts;
  std::stringstream stream(data);
  reg_t n;
  while (stream >> n) {
    parts.push_back(n);
  }
  return parts;
}

std::map<reg_t, reg_t> make_map(RegionVector v) {
  std::map<reg_t, reg_t> map;
  for (reg_t i = 0; i < v.size(); ++i) {
    map[v[i]] = i;
  }
  return map;
}

bool read_setup(str line, str &command, str &sub_command, str &data) {
  std::stringstream stream(line);
  if (stream >> command >> sub_command) {
    stream.ignore();
    std::getline(stream, data);
    return true;
  }
  return false;
};

void read_until_command(std::istream &in, str test_command, str &data) {
  while (true) {
    str line;
    std::getline(in, line);

    if (!line.compare(0, test_command.size(), test_command)) {
      data = "";
      if (line.size() > test_command.size() + 1) data = line.substr(test_command.size() + 1);
      return;
    }
  }
}
}

CanonicalGameSetup IOManager::run_setup() {
  // Read all basic setup commands. At the moment the pick_starting_regions command is received, return the current setup
  str line;

  str command;
  str sub_command;
  str data;
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
          SettingVector super_regions = read_pair_list(data);
          super_id.clear();
          for (SettingVector::iterator it = super_regions.begin(); it != super_regions.end(); ++it) {
            super_id.push_back(it->first);
            game_setup.super_award.push_back(it->second);
          }
        }
        else if (sub_command == "regions") {
          SettingVector regions = read_pair_list(data);
          region_id.clear();
          std::map<reg_t, reg_t> map = make_map(super_id);
          for (SettingVector::iterator it = regions.begin(); it != regions.end(); ++it) {
            region_id.push_back(it->first);
            game_setup.region_super.push_back(map[it->second]);
          }
        }
        else if (sub_command == "neighbors") {
          ConnectionVector connections = read_neighbour_list(data);
          std::map<reg_t, reg_t> map = make_map(region_id);
          game_setup.neighbours = std::vector<RegionVector>(region_id.size());
          for (ConnectionVector::iterator it = connections.begin(); it != connections.end(); ++it) {
            reg_t r1 = map[it->from];
            for (RegionVector::iterator it2 = it->to.begin(); it2 != it->to.end(); ++it2) {
              reg_t r2 = map[*it2];
              game_setup.neighbours[r1].push_back(r2);
              game_setup.neighbours[r2].push_back(r1);
            }
          }
        }
      }
      else if (command == "pick_starting_regions") {
        std::map<reg_t, reg_t> map = make_map(region_id);
        game_setup.init_options = std::vector<reg_t>();
        RegionVector in_regions = split(data);
        for (RegionVector::iterator it = in_regions.begin(); it != in_regions.end(); ++it) {
          game_setup.init_options.push_back(map[*it]);
        }
        break;
      }
    }
  }

  return game_setup;
}

void IOManager::run_game_loop(StrategyManager &manager) {
  write_picks(manager.pick_starting_regions());

  str data;
  army_t starting_armies;
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

void IOManager::write_picks(RegionVector regions) {
  for (RegionVector::iterator it = regions.begin(); it != regions.end(); ++it) {
    std::cout << region_id[*it] << " ";
  }
  std::cout << std::endl;
}

void IOManager::write_moves(MoveVector moves) {
  if (moves.size() == 0) {
    std::cout << "No moves" << std::endl;
    return;
  }

  for (MoveVector::iterator it = moves.begin(); it != moves.end(); ++it) {
    std::cout << game_setup.name_me << " attack/transfer " << region_id[it->from] << " " << region_id[it->to] << " " << it->amount << ", ";
  }

  std::cout << std::endl;

}

void IOManager::write_placements(PlacementVector placements) {
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

MoveVector IOManager::parse_moves(str data) {
  //TODO: implement
  return std::vector<Move>();
}

UpdateVector IOManager::parse_updates(str data) {
  std::map<reg_t, reg_t> map = make_map(region_id);
  std::map<std::string, Owner> owner_map;
  owner_map[game_setup.name_me] = ME;
  owner_map[game_setup.name_opponent] = OTHER;
  owner_map["neutral"] = NEUTRAL;

  UpdateVector result;

  std::stringstream stream(data);

  reg_t region;
  army_t amount;
  str player;
  while (stream >> region >> player >> amount) {
    Update u = {map[region], owner_map[player], amount};
    result.push_back(u);
  }

  return result;
}
