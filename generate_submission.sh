#!/bin/bash

mkdir -p gen
python scripts/write_winchance_table.py > gen/nbinom-cdf-gen.h
python scripts/write_names.py > gen/names-gen.h

tar -cvzf conquest.tar.gz *.cc *.h gen/*.h