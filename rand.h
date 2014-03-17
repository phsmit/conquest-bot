#pragma once

#include <iostream>
#include <random>

typedef uint16_t seed_t;
typedef std::mt19937 randg_t;

inline randg_t make_rand_engine(str str_seed) {
  seed_t seed;
  if (str_seed.size() == 0) {
    seed = (seed_t) time(NULL);
  } else {
    seed = (seed_t) std::stoi(str_seed, 0, 16);
  }

  std::cerr << "Seed: " << std::hex << seed << std::dec << std::endl;

  return randg_t(seed);
}