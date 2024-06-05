#!/bin/bash

# Usage: ./run.sh -solver <solver> -smt2 <smt_file> -t <time> [-output <0 or 1>]

# 使用cvc5-ocac：./run.sh -solver c -smt2 <smt_file> -t <time>
# 使用PairLS并输出解：./run.sh -solver p -smt2 <smt_file> -t <time> -output 1
# 使用PairLS并不输出解：./run.sh -solver p -smt2 <smt_file> -t <time> -output 0
# 使用z3pp-ls：./run.sh -solver z -smt2 <smt_file> -t <time>

./solvers/maomt/bin/maomt $@