#!/bin/bash

# 定义日志文件路径
log_file=$1

# 获取当前时间
current_time() {
    date "+%Y-%m-%d %H:%M:%S"
}

# 循环执行
while true; do
    # 获取总CPU占用和内存占用情况并追加到日志文件
    top -b -n 1 | grep "Cpu(s)" >> "$log_file"
    top -b -n 1 | grep "KiB Mem" >> "$log_file"

    # 打印时间戳到日志文件
    echo "Timestamp: $(current_time)" >> "$log_file"

    # 间隔5分钟
    sleep 300
done &

# 获取脚本的进程ID
script_pid=$!

# 等待一天
sleep 24h

# 杀死脚本进程
kill "$script_pid"