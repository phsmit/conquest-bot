#include "bot.h"

#include "io.h"

int main(int argc, const char *argv[]) {

  NoMoveBot nmb;

  IOManager io;
  io.run(nmb);

  return 0;

}


RegionVector NoMoveBot::cmd_pick_starting_regions(long t, RegionVector regions) {
  return RegionVector();
}

MoveVector NoMoveBot::cmd_go_attacktransfer(long t) {
  return MoveVector();
}

PlacementVector NoMoveBot::cmd_go_place_armies(long t) {
  return PlacementVector();
}

