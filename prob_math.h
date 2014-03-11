#ifndef CONQUEST_PROB_MATH_H_
#define CONQUEST_PROB_MATH_H_

#include "nbinom-cdf-gen.h"

namespace conquest {
namespace internal {

enum TABLE {
  ATTACK, DEFENCE
};

float nbinom_cdf(int n, int x, TABLE t) {
  if (n < 1) return 1.0;
  if (x < 0) return 0.0;

  if (x >= NBINOM_X_RANGE) return 1.0;
  if (n >= NBINOM_N_RANGE) return 0.0;

  switch (t) {
    case ATTACK:
      return NBINOM_CDF_06[n][x];
    case DEFENCE:
      return NBINOM_CDF_07[n][x];
  }
  return 0.0;
}

float get_win_prob(int attackers, int defenders) {

  float attack_success = nbinom_cdf(defenders, attackers - defenders, ATTACK);
  float defence_success = nbinom_cdf(attackers, defenders - attackers, DEFENCE);

  return attack_success * (1 - defence_success);
}

int attackers_needed(int defenders, double win_chance) {
  if (defenders == 0) return 0;
  int attackers = defenders;
  while(get_win_prob(attackers, defenders) < win_chance) ++attackers;
  return attackers;

}

}
}

#endif // CONQUEST_PROB_MATH_H_
