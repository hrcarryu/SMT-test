#include <iostream>
#include <cstdlib>
#include <string>

void call_solver(const std::string &solver_command) {
    int ret = std::system(solver_command.c_str());
    if (ret != 0) {
        std::cerr << "Error: Failed to call solver with command: " << solver_command << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 7 && argc != 9) {
        std::cerr << "Usage: " << argv[0] << " -solver <solver> -smt2 <smt_file> -t <time> [-output <0 or 1>]" << std::endl;
        return 1;
    }

    std::string solver = argv[2];
    std::string smt_file = argv[4];
    std::string time = argv[6];
    bool output_model = true;

    if (argc == 8) {
        output_model = std::stoi(argv[8]) != 0;
    }

    if (solver == "c") {
        std::string cvc5_command = "timeout " + time + " solvers/cvc5-ocac/bin/cvc5 " + smt_file;
        call_solver(cvc5_command);
    } else if (solver == "p") {
        std::string pairls_command = "solvers/PairLS/bin/PairLS " + smt_file + " " + time + " " + (output_model ? "1" : "0");
        call_solver(pairls_command);
    } else if (solver == "z") {
        std::string z3_command = "solvers/z3pp-ls/bin/z3 -smt2 " + smt_file + " -T:" + time;
        call_solver(z3_command);
    } else {
        std::cerr << "Unknown solver: " << solver << std::endl;
        return 1;
    }

    return 0;
}