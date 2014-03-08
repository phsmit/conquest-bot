#include "io.h"
#include "basic_attack_bot.h"
#include "strategy_manager.h"



int main(int argc, const char *argv[]) {

  BasicAttackBot bot;

  StrategyManager sm(bot);

  IOManager io;
  io.run(bot);

  return 0;

}
