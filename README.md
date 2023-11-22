# OMT-test-2024

文件夹结构：
* ./solvers: 求解器
* ./results: 求解结果
* ./charts:  结果表格
* ./scripts: 处理脚本

运行命令：
```
./total.sh $solver $time
$solver: cvc5-omt|z3pp-ls|PairLS
$time(s): 1000/1200
```
三个求解器的命令如下，结果保存在charts文件夹下。
```
./total.sh cvc5-omt 1200
./total.sh z3pp-ls 1200
./total.sh PairLS 300
```