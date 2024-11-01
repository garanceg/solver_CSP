#ifndef CSP_problems_H
#define CSP_problems_H

#include "CSP.h"
#include <string>

CSP create_n_queens_CSP_tuples(int n);
CSP create_n_queens_CSP_diff(int n);
CSP create_graph_coloring_CSP(const string instance_name, const int number_of_color);
CSP create_sudokus_CSP_tuples(const string instance_name);
CSP create_sudokus_CSP_diff(const string instance_name);


#endif