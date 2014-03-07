#include "bot.h"

#include "io.h"
#include "basic_attack_bot.h"

int main(int argc, const char *argv[]) {

  BasicAttackBot bot;

  IOManager io;
  io.run(bot);

  return 0;

}


