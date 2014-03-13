#include "io.h"

int main(int argc, const char *argv[]) {

  IOManager io = IOManager();

  CanonicalGameSetup game_setup = io.run_setup();
  GameData game_data(game_setup);
  StrategyManager manager(game_data);

  io.run_game_loop(manager);
  return 0;

}
