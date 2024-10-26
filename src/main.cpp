#include <iostream>
#include <fstream>
#include <sstream>

#include "variable.h"
#include "constraint.h"
#include "CSP.h"
#include "n_queens.cpp"
#include "graph_coloring.cpp"
using namespace std;

int main()
{
    CSP csp = graph_coloring("color_instances/le450_5a.col", 5);

    // vector<int> domain{0, 1, 2, 3, 4};
    // Variable A = Variable("A", domain);
    // Variable B = Variable("B", domain);
    // Variable C = Variable("C", domain);
    // Variable D = Variable("D", domain);
    // vector<Variable> variables{A, B, C, D};

    // vector<tuple<int, int>> tuples_AB = {make_tuple(2, 1), make_tuple(2, 2)};
    // Constraint cons_AB = Constraint(A, B, tuples_AB);
    // vector<tuple<int, int>> tuples_BC = {make_tuple(1, 1), make_tuple(0, 2)};
    // Constraint cons_BC = Constraint(B, C, tuples_BC);
    // vector<tuple<int, int>> tuples_AC = {make_tuple(1, 3), make_tuple(1, 2)};
    // Constraint cons_AC = Constraint(C, A, tuples_AC);

    // vector<tuple<int, int>> tuples_AD = {make_tuple(2, 0), make_tuple(3, 2)};
    // Constraint cons_AD = Constraint(A, D, tuples_AD);
    // vector<tuple<int, int>> tuples_BD = {make_tuple(2, 0), make_tuple(3, 2)};
    // Constraint cons_BD = Constraint(B, D, tuples_BD);
    // vector<tuple<int, int>> tuples_CD = {make_tuple(2, 0), make_tuple(3, 2)};
    // Constraint cons_CD = Constraint(C, D, tuples_CD);

    // // vector<Constraint> constraints{cons_AB, cons_AC, cons_BC, cons_AD, cons_BD, cons_CD};
    // vector<Constraint> constraints{cons_AB, cons_AC, cons_BC};

    // CSP csp(constraints = constraints, variables = variables);
    // CSP csp = n_reines(30);
    map<string, int> solution = csp.solve(csp.constraints, csp.variables);
    for (const auto &[k, v] : solution)
        cout << "variable" << k << " : " << v << endl;
}
