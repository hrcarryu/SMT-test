#!/bin/bash
# ./parallel.sh 200 ismt-yices2 1200
# ./parallel.sh 200 ismt 1200
# ./parallel.sh 200 z3 1200
# ./parallel.sh 100 Model-Validation

# 定义输入目录和输出目录
input_directory="/pub/data/Ailura/QF_S"
output_directory="/home/hanrui/SMT-test/results"
path_directory="/home/hanrui/SMT-test/QF_S_path"

# input_directory="/pub/data/Ailura/QF_NIA_scr"
# output_directory="/pub/data/Ailura/QF_NIA_res"
# path_directory="QF_NIA_path_all"

# input_directory="/pub/data/Ailura/QF_NIA_check"
# output_directory="/pub/data/Ailura/QF_NIA_res_1"
# path_directory="QF_NIA_path"

# input_directory="/home/hanrui/SMT-test/test"
# output_directory="/home/hanrui/SMT-test/test_res"
# path_directory="test_path"

# 输入参数
num_groups="$1"
solver="$2"
time_t="$3"
res_dir="$4"

# 创建输出目录
mkdir -p "$output_directory"
mkdir -p "$path_directory"
rm -rf "$path_directory"/*

# 获取文件夹下所有文件的路径，并将它们分为num_groups组
find "$input_directory" -type f > all_files.txt
split -l $((($(wc -l < all_files.txt) + num_groups - 1) / num_groups)) all_files.txt "${path_directory}/group_"
# 删除临时文件
rm all_files.txt

mkdir -p "$output_directory/$solver/$res_dir"
rm -rf "$output_directory/$solver/$res_dir"/*
# 遍历所有组文件，并使用run.sh脚本处理每一组
for group_file in "${path_directory}/group_"*; do
    group_name=$(basename "$group_file")
    ./run.sh "$group_file" "$input_directory" "$solver" "$time_t" > "${output_directory}/${solver}/${res_dir}/${group_name}.log" 2>&1 &
done
