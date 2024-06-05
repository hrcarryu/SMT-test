import pandas as pd
import os
import sys

class SMT_test:
    chart = None
    def __init__(self, benchmarks_path='../../benchmarks/', chart_path='../charts', result_path='../results'):
        self.benchmarks_path = benchmarks_path
        self.chart_path = chart_path
        self.result_path = result_path

    def init_chart(self, folder_path, read_path, save_path):
        full_execl_path = f'{self.chart_path}/{read_path}'
        self.chart = pd.read_excel(full_execl_path, header=[0], index_col = [0])
        full_folder_path = f'{self.benchmarks_path}{folder_path}'
        self.get_info(full_folder_path)
        full_save_path = f'{self.chart_path}/{save_path}'
        self.chart.to_excel(full_save_path)
    
    def get_info(self, folder_path):
        benchmarks = os.listdir(folder_path)
        for file in benchmarks:
            path = os.path.join(folder_path, file)
            if os.path.isdir(path):
                self.get_info(path)
            else:
                try:
                    with open(path) as f:
                        vars = 0
                        assertions = 0
                        status = 'none'
                        line = f.readline()
                        while line:
                            if line[:3] == '(de':
                                vars += 1
                            elif line[:3] == '(as':
                                assertions += 1
                            elif line[11:17] == 'status':
                                status = line.split(' ')[2][:-2]
                            line = f.readline()
                        category = path.split('/')[-2]
                        path = path.replace(self.benchmarks_path, '')
                        self.chart.loc[len(self.chart.index)] = [path, category, vars, assertions, status]
                except:
                    print(path)

    def arrange(self, solver, log_path, read_path, save_path):
        full_execl_path = f'{self.chart_path}/{read_path}'
        self.chart = pd.read_excel(full_execl_path, header=[0], index_col = [0])
        full_res_path = f'{self.result_path}/{solver}/{log_path}'
        results = os.listdir(full_res_path)
        result_name = f'result_{solver}_{log_path}'
        time_name = f'time_{solver}_{log_path}'
        total = len(self.chart.index)
        count = 0
        for file in results:
            with open(os.path.join(full_res_path, file)) as f:
                try:
                    if solver == 'z3' or solver == 'z3pp_ls' or solver == 'z3pp':
                        while f.readline():
                            result = f.readline().rstrip()
                            # OMT-test only
                            if result == "sat" or result == "unknown":
                                f.readline()
                                f.readline()
                                f.readline()
                            elif result[:3] == "./s":
                                result = "unknown"
                            info = f.readline().rstrip()
                            if info == "unknown":
                                info = f.readline().rstrip()
                            name = info.split(' : ')[0]
                            timeout = info.split(' : ')[1][:-4]
                            self.chart.loc[self.chart['filename'] == name, [result_name, time_name]] = [result, float(timeout)]
                            count += 1
                    elif solver == 'cvc5-omt':
                        while f.readline():
                            info = f.readline().rstrip()
                            result = info
                            if info == 'unknown':
                                info = f.readline().rstrip()
                                if len(info) < 10:
                                    if len(info) > 1:
                                        result = info
                                    info = f.readline().rstrip()
                            elif len(info) > 10:
                                result = "unknown"
                            else:
                                info = f.readline().rstrip()
                            name = info.split(' : ')[0]
                            timeout = info.split(' : ')[1][:-4]
                            self.chart.loc[self.chart['filename'] == name, [result_name, time_name]] = [result, float(timeout)]
                            count += 1
                    elif solver == 'PairLS':
                        while f.readline():
                            info = f.readline().rstrip()
                            result = info
                            if len(info) > 10:
                                result = "unknown"
                            info = f.readline().rstrip()
                            name = info.split(' : ')[0]
                            timeout = info.split(' : ')[1][:-4]
                            self.chart.loc[self.chart['filename'] == name, [result_name, time_name]] = [result, float(timeout)]
                            count += 1
                except:
                    print(file)

        print(f"count/total: {count}/{total}")
        
        full_save_path = f'{self.chart_path}/{save_path}'
        self.chart.to_excel(full_save_path)

if __name__ == "__main__":
    solver = sys.argv[1]
    if solver == "z3pp-ls":
        omt_lia = SMT_test()
        omt_lia.arrange('z3pp-ls', 'OMT_LIA', 'OMT_LIA.xlsx', 'result_OMT_LIA.xlsx')
        omt_lia.arrange('z3', 'OMT_LIA', 'result_OMT_LIA.xlsx', 'result_OMT_LIA.xlsx')
    elif solver == "cvc5-omt":
        omt_lia = SMT_test()
        omt_lia.arrange('z3pp-ls', 'OMT_LIA', 'OMT_LIA.xlsx', 'result_OMT_LIA.xlsx')
        omt_lia.arrange('z3', 'OMT_LIA', 'result_OMT_LIA.xlsx', 'result_OMT_LIA.xlsx')
    elif solver == "PairLS":
        omt_lia = SMT_test()
        omt_lia.arrange('z3pp-ls', 'OMT_LIA', 'OMT_LIA.xlsx', 'result_OMT_LIA.xlsx')
        omt_lia.arrange('z3', 'OMT_LIA', 'result_OMT_LIA.xlsx', 'result_OMT_LIA.xlsx')