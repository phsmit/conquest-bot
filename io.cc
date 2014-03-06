#include "io.h"

#include <iostream>
#include <sstream>

SettingVector read_pair_list(std::stringstream& in) {
  SettingVector result;
//  int k, v;
//  while(in >> k >> v) {
//    result.push_back(Setting{k, v});
//  }
  return result;
}

ConnectionVector read_neighbour_list(std::stringstream& in) {
  ConnectionVector result;

  return result;
}

UpdateVector read_updates(std::stringstream& in) {
  UpdateVector result;

  return result;
}

MoveVector read_move_vector(std::stringstream& in) {
  MoveVector result;
  return result;
}

RegionVector read_region_vector(std::stringstream& in) {
  RegionVector result;
  return result;
}


void IOManager::run(BaseBot& bot) {
  while (true) {
    std::string line;
    getline(std::cin, line);
    std::stringstream str(line);

    std::string command;
    str >> command;

    if (command == "quit") break;
    else if (command == "setup_map") {
      std::string sub_command;
      str >> sub_command;
      if (sub_command == "super_regions") {
        bot.cmd_setupmap_superregions(read_pair_list(str));
      } else if (sub_command == "regions") {
        bot.cmd_setupmap_regions(read_pair_list(str));
      } else if (sub_command == "neighbours") {
        bot.cmd_setupmap_neighbours(read_neighbour_list(str));
      }

    } else if (command == "settings") {
      std::string sub_command;
      str >> sub_command;
      if (sub_command == "your_bot") {
        std::string name;
        str >> name;
        bot.cmd_settings_yourbot(name);
      } else if (sub_command == "opponent_bot") {
        std::string name;
        str >> name;
        bot.cmd_settings_opponent(name);
      } else if (sub_command == "starting_armies") {
        int starting_armies;
        str >> starting_armies;
        bot.cmd_settings_starting_armies(starting_armies);
      }
    } else if (command == "update_map") {
      bot.cmd_updatemap(read_updates(str));
    } else if (command == "opponent_moves") {
      bot.cmd_opponentmoves(read_move_vector(str));
    } else if (command == "pick_starting_regions") {
      long t;
      str >> t;
      RegionVector rv = bot.cmd_pick_starting_regions(t, read_region_vector(str));
      std::cout << "No moves" << std::endl;
    } else if (command == "go") {
      std::string sub_command;
      str >> sub_command;
      long t;
      str >> t;
      if (sub_command == "place_armies") {
        PlacementVector pv = bot.cmd_go_place_armies(t);
        std::cout << "No moves" << std::endl;
      } else if (sub_command == "attack/transfer") {
        MoveVector mv = bot.cmd_go_attacktransfer(t);
        std::cout << "No moves" << std::endl;
      }

    }
  }
}