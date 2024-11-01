#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <iomanip>
#include <cmath>

#include "variable.h"
#include "constraint.h"
#include "CSP.h"
#include "CSP_problems.h"

using namespace std;


void draw_sudoku(const std::map<int, int>& sudoku_dict, int grid_size) {
    int block_size = static_cast<int>(std::sqrt(grid_size));

    for (int i = 0; i < grid_size; ++i) {
        // Print horizontal block separator
        if (i % block_size == 0 && i != 0) {
            std::cout << std::string(grid_size * 3 + block_size - 1, '-') << '\n';
        }

        for (int j = 0; j < grid_size; ++j) {
            // Print vertical block separator
            if (j % block_size == 0 && j != 0) {
                std::cout << " | ";
            }

            // Calculate the key for the current cell, which is i * grid_size + j
            int key = i * grid_size + j;

            // Check if there's a value in the dictionary; print it or a blank space
            if (sudoku_dict.find(key) != sudoku_dict.end()) {
                std::cout << std::setw(2) << sudoku_dict.at(key) << " ";
            }
            else {
                std::cout << " . ";  // Blank cell
            }
        }
        std::cout << '\n';
    }
}

int main() {
    //--------------- N-Queens Problem ---------------//

    int number_of_queens = 10;
    CSP csp = create_n_queens_CSP_tuples(number_of_queens);

    bool activate_AC1 = false;
    bool activate_AC3 = true;
    bool activate_FC = true;
    bool activate_MAC = false;

    string value_order_startegy = "min_value";
    string variable_order_strategy = "min_domain_size";

    double max_duration = 20.0;

    map<int, int> assignment;
    for (Variable var : csp.variables) {
        if (var.domain.size() == 1)
            assignment[var.idx] = var.domain[0];
    }
    tuple<map<int, int>, int, double> solution = csp.solve(assignment, activate_AC1, activate_AC3,
        activate_FC, activate_MAC, value_order_startegy, variable_order_strategy, max_duration);
    cout << "Nb nodes : " << std::get<1>(solution) << endl;
    cout << "Elapsed Time : " << std::get<2>(solution) << endl;


    //---------- N-Queens Problem with Constraint Diff ---------//

    // int number_of_queens = 10;
    // CSP csp = create_n_queens_CSP_diff(number_of_queens);

    // bool activate_AC1 = false;
    // bool activate_AC3 = true;
    // bool activate_FC = true;
    // bool activate_MAC = false;

    // string value_order_startegy = "min_value";
    // string variable_order_strategy = "min_domain_size";

    // double max_duration = 20.0;

    // map<int, int> assignment;
    // for (Variable var : csp.variables) {
    //     if (var.domain.size() == 1)
    //         assignment[var.idx] = var.domain[0];
    // }
    // tuple<map<int, int>, int, double> solution = csp.solve(assignment, activate_AC1, activate_AC3,
    //     activate_FC, activate_MAC,value_order_startegy, variable_order_strategy, max_duration);
    // cout << "Nb nodes : " << std::get<1>(solution) << endl;
    // cout << "Elapsed Time : " << std::get<2>(solution) << endl;


    //---------------- Sudoku Problem -----------------//

    // string file_name = "sudokus/9-expert1.txt";
    // CSP csp = create_sudokus_CSP_tuples(file_name);

    // bool activate_AC1 = false;
    // bool activate_AC3 = true;
    // bool activate_FC = true;
    // bool activate_MAC = false;

    // string value_order_startegy = "min_value";
    // string variable_order_strategy = "min_domain_size";

    // double max_duration = 20.0;

    // map<int, int> assignment;
    // for (Variable var : csp.variables) {
    //     if (var.domain.size() == 1)
    //         assignment[var.idx] = var.domain[0];
    // }
    // draw_sudoku(assignment, 9);

    // tuple<map<int, int>, int, double> solution = csp.solve(assignment, activate_AC1, activate_AC3,
    //     activate_FC, activate_MAC, value_order_startegy, variable_order_strategy, max_duration);

    // draw_sudoku(std::get<0>(solution), 9);


    //---------------- Sudoku Problem with Constraint Diff -------------//

    // string file_name = "sudokus/9-expert1.txt";
    // CSP csp = create_sudokus_CSP_diff(file_name);

    // bool activate_AC1 = false;
    // bool activate_AC3 = true;
    // bool activate_FC = true;
    // bool activate_MAC = false;

    // string value_order_startegy = "min_value";
    // string variable_order_strategy = "min_domain_size";

    // double max_duration = 20.0;

    // map<int, int> assignment;
    // for (Variable var : csp.variables) {
    //     if (var.domain.size() == 1)
    //         assignment[var.idx] = var.domain[0];
    // }
    // draw_sudoku(assignment, 9);

    // tuple<map<int, int>, int, double> solution = csp.solve(assignment, activate_AC1, activate_AC3,
    //     activate_FC, activate_MAC, value_order_startegy, variable_order_strategy, max_duration);

    // draw_sudoku(std::get<0>(solution), 9);


    //------------------- Coloring ---------------------//

    // string file_name = "color_instances/le450_5a.col";
    // int number_of_colors = 5;
    // CSP csp = create_graph_coloring_CSP(file_name, number_of_colors);

    // bool activate_AC1 = false;
    // bool activate_AC3 = true;
    // bool activate_FC = true;
    // bool activate_MAC = false;

    // string value_order_startegy = "min_value";
    // string variable_order_strategy = "min_domain_size";

    // double max_duration = 20.0;

    // map<int, int> assignment;
    // for (Variable var : csp.variables) {
    //     if (var.domain.size() == 1) {
    //         assignment[var.idx] = var.domain[0];
    //     }
    // }
    // tuple<map<int, int>, int, double> solution = csp.solve(assignment, activate_AC1, activate_AC3,
    //     activate_FC, activate_MAC, value_order_startegy, variable_order_strategy, max_duration);
    // cout << "Nb nodes : " << std::get<1>(solution) << endl;
    // cout << "Elapsed Time : " << std::get<2>(solution) << endl;

}
