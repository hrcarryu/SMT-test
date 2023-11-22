#!/bin/bash
# ./total.sh z3pp-ls 1200

solver=$1
time_t=$2

if [ "$solver"x = "cvc5-omt"x ]; then
    ./scripts/parallel.sh 100 cvc5-omt $time_t OMT_NRA
    ./scripts/parallel.sh 100 z3 $time_t OMT_NRA
fi

if [ "$solver"x = "z3pp-ls"x ]; then
    ./scripts/parallel.sh 100 z3pp-ls $time_t OMT_LIA
    ./scripts/parallel.sh 100 z3 $time_t OMT_LIA
fi

if [ "$solver"x = "PairLS"x ]; then
    ./scripts/parallel.sh 100 PairLS $time_t MaxSMT_LS
    ./scripts/parallel.sh 100 z3 $time_t MaxSMT_z3
fi

if [ "$solver"x = "test"x ]; then
    ./scripts/parallel.sh 100 z3pp-ls $time_t test
    ./scripts/parallel.sh 100 z3 $time_t test
fi