import re
import random
import os
import sys

def has_optimization_target(content):
    return '(minimize' in content or '(maximize' in content

def modify_file(file_path):
    # 读取文件内容
    with open(file_path, 'r') as file:
        content = file.read()

    if has_optimization_target(content):
        print(f"{file_path}: 文件中已存在优化目标，不做修改。")
        return

    # 正则表达式匹配声明语句
    declare_pattern = re.compile(r'declare-fun (\S+) \(\) Int')
    matches = declare_pattern.findall(content)
    if not matches:
        declare_pattern = re.compile(r'\(declare-const (\w+) Int\)')
        matches = declare_pattern.findall(content)
        if not matches:
            print(f"{file_path}: 未找到声明语句。")
            os.remove(file_path)
            print("File has been deleted.")
            return

    # 随机选择一个变量
    target_variable = random.choice(matches)

    # 随机生成优化目标
    optimization_target = random.choice(['(minimize {})'.format(target_variable),
                                         '(maximize {})'.format(target_variable)])

    # 在文件中插入优化目标
    check_sat_pattern = re.compile(r'\(check-sat\)')
    content = check_sat_pattern.sub('{}\n{}'.format(optimization_target, r'\g<0>'), content, count=1)

    # 写入修改后的内容到文件
    with open(file_path, 'w') as file:
        file.write(content)

    print(f"{file_path}: 文件修改成功。")

# modify_file("test.smt2")

group = sys.argv[1]
with open(group) as f:
    file_list = f.readlines()
    for line in file_list:
        file_path = line.strip()
        modify_file(file_path)