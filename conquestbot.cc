#include "io.h"
#include "strategy_bot.h"


int main(int argc, const char *argv[]) {

  StrategyBot bot;

  IOManager io;
  io.run(bot);

  return 0;

}
