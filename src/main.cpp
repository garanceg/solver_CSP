#include <iostream>
#include <fstream>
#include <sstream>

#include "variable.h"
#include "constraint.h"
#include "CSP.h"
// #include "n_queens.cpp"
// #include "graph_coloring.cpp"
using namespace std;

int main()
{
    // CSP csp = graph_coloring("color_instances/queen6_6.col", 7);

    vector<int> domain{0, 1, 2, 3, 4};
    Variable A = Variable("A", domain);
    Variable B = Variable("B", domain);
    Variable C = Variable("C", domain);
    vector<Variable> variables{A, B, C};

    vector<tuple<int, int>> tuples_AB = {make_tuple(0, 1), make_tuple(0, 0)};
    Constraint cons_AB = Constraint(A, B, tuples_AB);
    vector<tuple<int, int>> tuples_BC = {make_tuple(1, 1), make_tuple(0, 2)};
    Constraint cons_BC = Constraint(B, C, tuples_BC);
    vector<tuple<int, int>> tuples_AC = {make_tuple(1, 0), make_tuple(1, 2)};
    Constraint cons_AC = Constraint(C, A, tuples_AC);

    vector<Constraint> constraints{cons_AB, cons_AC, cons_BC};

    CSP csp(constraints = constraints, variables = variables);
    map<string, int> solution = csp.solve(constraints, variables);
    for (const auto &[k, v] : solution)
        cout << "variable" << k << " : " << v << endl;
}
