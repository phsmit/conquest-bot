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
    result.push_back({k, v});
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
    result.push_back({k, neighbours});
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

bool read_until_command(std::istream &in, str test_command, str &data) {
  str line;
  while (std::getline(in, line)) {
    if (!line.compare(0, test_command.size(), test_command)) {
      data = "";
      if (line.size() > test_command.size() + 1) data = line.substr(test_command.size() + 1);
      return true;
    }
  }
  return false;
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
    if (!read_until_command(std::cin, "settings starting_armies", data)) break;
    starting_armies = atoi(data.c_str());

    if (!read_until_command(std::cin, "update_map", data)) break;
    manager.process_updates(parse_updates(data));

    if (!read_until_command(std::cin, "opponent_moves", data)) break;
    manager.process_opponent_moves(parse_moves(data));

    manager.start_new_round(starting_armies);

    if (!read_until_command(std::cin, "go place_armies", data)) break;
    write_placements(manager.place_armies());

    if (!read_until_command(std::cin, "go attack/transfer", data)) break;
    write_moves(manager.make_moves());

    if (manager.finished()) {
      break;
    }
  }
}

void IOManager::write_picks(RegionVector regions) {
  for (auto region : regions) {
    std::cout << region_id[region] << " ";
  }
  std::cout << std::endl;
}

void IOManager::write_moves(MoveVector moves) {
  if (moves.size() == 0) {
    std::cout << "No moves" << std::endl;
    return;
  }

  for (auto move : moves) {
    std::cout << game_setup.name_me << " attack/transfer " << region_id[move.from] << " " << region_id[move.to] << " " << move.amount << ", ";
  }

  std::cout << std::endl;

}

void IOManager::write_placements(PlacementVector placements) {
  bool output_written = false;

  for (auto placement : placements) {
    if (placement.amount > 0) {
      std::cout << game_setup.name_me << " place_armies " << region_id[placement.to] << " " << placement.amount << ", ";
      output_written = true;
    }
  }

  if (!output_written) std::cout << "No moves";
  std::cout << std::endl;
}

MoveVector IOManager::parse_moves(str data) {
  std::map<reg_t, reg_t> map = make_map(region_id);

  MoveVector moves;
  std::stringstream stream(data);

  str player;
  str action;
  reg_t from;
  reg_t to;
  army_t amount;

  while (stream >> player >> action) {
    if (action == "place_armies") {
      stream >> to >> amount;
      from = to;
    } else {
      stream >> from >> to >> amount;
    }

    moves.push_back({map[from], map[to], amount});
  }


  return moves;
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
    result.push_back({map[region], owner_map[player], amount});
  }

  return result;
}
