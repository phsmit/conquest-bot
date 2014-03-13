#!/bin/bash

python scripts/write_winchance_table.py > nbinom-cdf-gen.h
python scripts/write_names.py > names-gen.h

tar -cvzf conquest.tar.gz *.cc *.h