#!/bin/bash

input_file="$1"
output_dir="$2"
input_directory="$3"
solver="$4"
time_t="$5"

while read -r file_path; do
    # 在这里处理每个文件，例如：
    relative_path=${file_path#$input_directory}
    new_file_path="${output_dir}${relative_path}"
    new_file_dir=$(dirname "$new_file_path")

    mkdir -p "$new_file_dir"

    # 在这里执行你需要的文件处理操作
    start=$[$(date +%s%N)/1000000]
    if [ "$solver"x = "z3"x ]; then
        ./solvers/z3/run.sh $file_path $time_t > "$new_file_path"
    fi
    if [ "$solver"x = "cvc5"x ]; then
        ./solvers/cvc5/run.sh $file_path $time_t > "$new_file_path"
    fi
    if [ "$solver"x = "yices2"x ]; then
        ./solvers/yices2/run.sh $file_path $time_t > "$new_file_path"
    fi
    if [ "$solver"x = "mathsat5"x ]; then
        ./solvers/mathsat/run.sh $file_path $time_t > "$new_file_path"
    fi
    if [ "$solver"x = "smtrat"x ]; then
        ./solvers/smtrat/run.sh $file_path $time_t > "$new_file_path"
    fi
    if [ "$solver"x = "z3_UD"x ]; then
        ./solvers/z3_UD/run.sh $file_path $time_t > "$new_file_path"
    fi
    if [ "$solver"x = "ismt"x ]; then
        ./solvers/ismt/run.sh $file_path $time_t > "$new_file_path"
    fi
    if [ "$solver"x = "ismt-yices2"x ]; then
        ./solvers/ismt-yices2/run.sh $file_path $time_t > "$new_file_path"
    fi
    end=$[$(date +%s%N)/1000000]
    take=$(( end - start ))
    echo $file_path : ${take} ms. >> "$new_file_path"
done < "$input_file"
