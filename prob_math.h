#pragma once

#include "gen/nbinom-cdf-gen.h"

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

float get_win_prob(army_t attackers, army_t defenders) {

  float attack_success = nbinom_cdf(defenders, attackers - defenders, ATTACK);
  float defence_success = nbinom_cdf(attackers, defenders - attackers, DEFENCE);

  return attack_success * (1 - defence_success);
}

army_t attackers_needed(army_t defenders, double win_chance) {
  if (defenders == 0) return 0;
  army_t attackers = defenders;
  while (get_win_prob(attackers, defenders) < win_chance) ++attackers;
  return attackers;

}

army_t defenders_needed(army_t attackers, double defend_chance) {
  if (attackers <= 0) return 0;
  army_t defenders = 0;
  while (get_win_prob(attackers, defenders) > (1 - defend_chance)) ++defenders;

  return defenders;
}

}
}