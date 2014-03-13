#!/usr/bin/env python

precision = 5
PRECISION = 5
N_RANGE = 300
X_RANGE = 30

import math
import itertools
import scipy.stats

print "#pragma GCC diagnostic ignored \"-Wmissing-braces\""
print "#define NBINOM_X_RANGE {}".format(X_RANGE)
print "#define NBINOM_N_RANGE {}".format(N_RANGE)



print "static const float NBINOM_CDF_06[NBINOM_N_RANGE][NBINOM_X_RANGE] = {"

values = []

for d in range(N_RANGE):
    for a in range(X_RANGE):
        values.append(scipy.stats.nbinom.cdf(a,d,0.6))

values = [v if not math.isnan(v) else -1.0 for v in values]
values = [v if v > 0.0001 else 0.0 for v in values]

print ",".join("{:.{precision}}f".format(v, precision=precision) for v in
               values)

print "};"


print "static const float NBINOM_CDF_07[NBINOM_N_RANGE][NBINOM_X_RANGE] = {"

values = []

for d in range(N_RANGE):
    for a in range(X_RANGE):
        values.append(scipy.stats.nbinom.cdf(a,d,0.7))

values = [v if not math.isnan(v) else -1.0 for v in values]
values = [v if v > 0.0001 else 0.0 for v in values]


print ",".join("{:.{precision}}f".format(v, precision=precision) for v in
               values)

print "};"


