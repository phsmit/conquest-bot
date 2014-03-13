#!/bin/bash

python write_winchance_table.py > nbinom-cdf-gen.h

tar -cvzf conquest.tar.gz *.cc *.h