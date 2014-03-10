#ifndef CONQUEST_PROB_MATH_H_
#define CONQUEST_PROB_MATH_H_

namespace conquest {
namespace internal {

#include <assert.h>
#include <math.h>

unsigned long long factorial(int x) {
  if (x <= 1) return 1;

  return factorial(x - 1) * x;
}

unsigned long long bin_coefficient(int n, int k) {
  return factorial(n + k) / (factorial(n) * factorial(k));
}

double log_bin_coefficient(int n, int k) {
  double x = n;
  double y = k;
  double result = (x * log(x)) - (y * log(y)) - ((x - y) * log(x - y));
  return result;
}

double kill_exactly_n(int trials, int n, double prob) {
  assert (trials >= n);
  if (trials > 10) {
    return exp(log(pow(prob, n)) + log(pow((1.0 - prob), trials - n))
        + log_bin_coefficient(trials - n, n));
  }
  return pow(prob, n) * pow((1.0 - prob), trials - n) * double(bin_coefficient(trials - n, n));
}

double win_chance(int attackers, int defenders, double attack_prob) {
  if (attackers < defenders) return 0.0;
  if (attackers - defenders > 10) return 1.0;

  double win_chance = 0.0;
  for (int n = defenders; n <= attackers; ++n) {
    win_chance += kill_exactly_n(attackers, n, attack_prob);
  }
  return win_chance;
}

double get_win_prob(int attackers, int defenders) {
  float ATTACK_CHANCE = 0.6;
  float DEFEND_CHANCE = 0.7;

  double attack_success = win_chance(attackers, defenders, ATTACK_CHANCE);
  double defence_succes = win_chance(defenders, attackers, DEFEND_CHANCE);

  return attack_success * (1 - defence_succes);
}

int attackers_needed(int defenders, double win_chance) {
  int attackers = defenders;
  while(get_win_prob(attackers, defenders) < win_chance) ++attackers;
  return attackers;
}

}
}

#endif // CONQUEST_PROB_MATH_H_
