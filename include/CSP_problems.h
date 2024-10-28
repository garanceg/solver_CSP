#ifndef CSP_problems_H
#define CSP_problems_H

#include "CSP.h"
#include <string>

CSP create_n_queens_CSP(int n);

CSP create_graph_coloring_CSP(const string instance_name, const int number_of_color);

CSP generate_random_CSP(int num_variables, int domain_size);


#endif