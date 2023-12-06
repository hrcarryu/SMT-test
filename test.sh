./scripts/parallel.sh 100 z3pp-ls 1200 OMT_LIA
nohup ./scripts/log.sh results/z3pp-ls-1200.log &

./scripts/parallel.sh 100 PairLS 300 MaxSMT_LS
nohup ./scripts/log.sh results/PairLS-300.log &

./scripts/parallel.sh 100 z3 1200 OMT_LIA
nohup ./scripts/log.sh results/z3-1-1200.log &

./scripts/parallel.sh 100 z3 300 MaxSMT_z3
nohup ./scripts/log.sh results/z3-maxsmt-300.log &

./scripts/parallel.sh 100 z3 1200 OMT_NRA
nohup ./scripts/log.sh results/z3-nra-1200.log &