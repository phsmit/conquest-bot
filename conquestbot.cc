#include "io.h"
#include "rand.h"

int main(int argc, const char *argv[]) {

  str seed = "";
  if (argc > 1) {
    seed = str(argv[1]);
  }

  randg_t rand_engine = make_rand_engine(seed);

  IOManager io = IOManager();

  CanonicalGameSetup game_setup = io.run_setup();
  GameData game_data(game_setup);
  StrategyManager manager(game_data, rand_engine);

  io.run_game_loop(manager);
  return 0;

}
