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
    CSP csp = graph_coloring("color_instances/queen6_6.col", 7);
    map<string, int> solution = csp.solve();
    for (const auto &[k, v] : solution)
        cout << "variable" << k << " : " << v << endl;
}
