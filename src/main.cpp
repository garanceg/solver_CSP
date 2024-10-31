#include <iostream>
#include <fstream>
#include <sstream>
#include <random>

#include "variable.h"
#include "constraint.h"
#include "CSP.h"
#include "CSP_problems.h"

using namespace std;


int main() {
    // int num_variables = 10;    // Nombre de variables
    // int domain_size = 5;      // Taille du domaine
    // CSP csp = generate_random_CSP(num_variables, domain_size);

    // vector<int> domain{ 0, 1, 2 };
    // Variable A = Variable("A", domain);
    // Variable B = Variable("B", domain = { 0, 1, 2, 3, 4 });
    // Variable C = Variable("C", domain = { 0, 1, 2, 3, 4, 5 });
    // Variable D = Variable("D", domain);
    // vector<Variable> variables{ A, B, C };

    // vector<tuple<int, int>> tuples_AB = { make_tuple(2, 1), make_tuple(2, 2) };
    // Constraint cons_AB = Constraint(A, B, tuples_AB);
    // vector<tuple<int, int>> tuples_BC = { make_tuple(1, 2), make_tuple(0, 2) };
    // Constraint cons_BC = Constraint(B, C, tuples_BC);
    // vector<tuple<int, int>> tuples_AC = { make_tuple(1, 1), make_tuple(2, 2) };
    // Constraint cons_AC = Constraint(C, A, tuples_AC);
    // vector<tuple<int, int>> tuples_AD = { make_tuple(2, 0), make_tuple(3, 2) };
    // Constraint cons_AD = Constraint(A, D, tuples_AD);
    // vector<tuple<int, int>> tuples_BD = { make_tuple(2, 0), make_tuple(3, 2) };
    // Constraint cons_BD = Constraint(B, D, tuples_BD);
    // vector<tuple<int, int>> tuples_CD = { make_tuple(2, 0), make_tuple(3, 2) };
    // Constraint cons_CD = Constraint(C, D, tuples_CD);
    // // vector<Constraint> constraints{ cons_AB, cons_AC, cons_BC, cons_AD, cons_BD, cons_CD };

    // vector<Constraint> constraints{ cons_AB, cons_BC };
    // CSP csp(constraints = constraints, variables = variables);


    // string filename1 = "results_FC.txt";
    // ofstream outputFile1(filename1);
    // bool activate_AC1 = false;
    // bool activate_AC3 = false;
    // bool activate_FC = true;

    // for (int n = 4; n < 20; n++) {
    //     CSP csp = create_n_queens_CSP(n);
    //     tuple<map<string, int>, int, double> solution = csp.solve(csp.constraints, csp.variables, activate_AC1, activate_AC3,
    //             activate_FC);
    //     int nodes = std::get<1>(solution);
    //     double elapsed = std::get<2>(solution);
    //     outputFile1 << n << " ; " << nodes << " ; " << elapsed << "\n";
    // }

    // outputFile1.close();

    // string filename2 = "results_without_FC.txt";
    // ofstream outputFile(filename2);
    // activate_AC1 = false;
    // activate_AC3 = false;
    // activate_FC = false;

    // for (int n = 4; n < 20; n++) {
    //     CSP csp = create_n_queens_CSP(n);
    //     tuple<map<string, int>, int, double> solution = csp.solve(csp.constraints, csp.variables, activate_AC1, activate_AC3,
    //             activate_FC);
    //     int nodes = std::get<1>(solution);
    //     double elapsed = std::get<2>(solution);
    //     outputFile << n << " ; " << nodes << " ; " << elapsed << "\n";
    // }
    // outputFile.close();

    // CSP csp = create_graph_coloring_CSP("color_instances/queen5_5.col", 5);
    CSP csp = create_n_queens_CSP_2(20);

    bool activate_AC1 = true;
    bool activate_AC3 = false;
    bool activate_FC = false;
    bool activate_MAC = true;

    string value_order_startegy = "";
    string variable_order_strategy = "min_domain_size";

    tuple<map<int, int>, int, double> solution = csp.solve(csp.constraints, csp.variables, activate_AC1, activate_AC3,
        activate_FC, activate_MAC, value_order_startegy, variable_order_strategy);

    for (const auto& [k, v] : std::get<0>(solution))
        cout << "variable" << k << " : " << v << endl;
}
