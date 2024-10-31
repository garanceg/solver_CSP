#include "CSP_problems.h"
#include "variable.h"
#include "constraint.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <random>

CSP create_n_queens_CSP_tuples(int n) {
    vector<int> domain;
    vector<Variable> variables;
    vector<Constraint*> constraints;

    for (int i = 1; i < n + 1; i++) {
        domain.push_back(i);
    }

    for (int i = 0; i < n; i++) {
        Variable var("X_" + std::to_string(i), i, domain);
        variables.push_back(var);
    }

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            vector<tuple<int, int>> validPairs;
            for (int vi : domain) {
                for (int vj : domain) {
                    if (vi != vj && abs(vi - vj) != abs(i - j)) {
                        validPairs.push_back(make_tuple(vi, vj));
                    }
                }
            }
            Constraint* cons = new Tuple_Constraint(make_pair(i, j), validPairs);
            constraints.push_back(cons);
        }
    }
    return CSP(constraints = constraints, variables = variables);
}

CSP create_n_queens_CSP_diff(int n) {
    vector<int> domain;
    vector<Variable> variables;
    vector<Constraint*> constraints;

    for (int i = 1; i < n + 1; i++) {
        domain.push_back(i);
    }

    for (int i = 0; i < n; i++) {
        Variable var("X_" + std::to_string(i), i, domain);
        variables.push_back(var);
    }

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            Constraint* cons_diff = new Diff_Constraint(make_pair(i, j), make_pair(1, -1), 0);
            Constraint* cons_diag_1 = new Diff_Constraint(make_pair(i, j), make_pair(1, -1), i - j);
            Constraint* cons_diag_2 = new Diff_Constraint(make_pair(i, j), make_pair(1, -1), j - i);
            constraints.push_back(cons_diff);
            constraints.push_back(cons_diag_1);
            constraints.push_back(cons_diag_2);
        }
    }
    return CSP(constraints = constraints, variables = variables);
}


CSP create_graph_coloring_CSP(const string instance_name, const int number_of_color) {
    ifstream file(instance_name);

    // Check if the file is successfully opened
    if (!file.is_open()) {
        cerr << "Error opening the file!";
        return CSP();
    }

    // String variable to store the read data
    string s;
    vector<Variable> variables;
    vector<Constraint*> constraints;
    vector<int> domain;
    vector<tuple<int, int>> tuples;
    for (int i = 0; i < number_of_color; i++) {
        domain.push_back(i);
    }
    for (int i = 0; i < number_of_color; i++) {
        for (int j = 0; j < number_of_color; j++) {
            if (i != j) {
                tuples.push_back(make_tuple(i, j));
            }
        }
    }

    int number_of_nodes;
    int number_of_constraints;

    while (getline(file, s)) {
        if (s[0] == 'p') {
            stringstream ss(s);
            string token;
            vector<string> tokens;
            char delimiter = ' ';

            while (getline(ss, token, delimiter)) {
                tokens.push_back(token);
            }
            number_of_nodes = stoi(tokens[2]);
            number_of_constraints = stoi(tokens[3]);
            cout << "Number of nodes variables : " << number_of_nodes << endl;
            cout << "Number of nodes constraints : " << number_of_constraints << endl;

            for (int i = 0; i < number_of_nodes; i++) {
                Variable var("X_" + std::to_string(i), i, domain);
                variables.push_back(var);
            }
            assert(number_of_nodes == variables.size());
        }

        if (s[0] == 'e') {
            stringstream ss(s);
            string token;
            vector<string> tokens;
            char delimiter = ' ';

            while (getline(ss, token, delimiter)) {
                tokens.push_back(token);
            }
            assert(tokens.size() == 3);
            assert(stoi(tokens[1]) <= number_of_nodes);
            assert(stoi(tokens[2]) <= number_of_nodes);
            Variable var_1 = variables[stoi(tokens[1]) - 1];
            Variable var_2 = variables[stoi(tokens[2]) - 1];
            Constraint* cons = new Tuple_Constraint(make_pair(stoi(tokens[1])-1, stoi(tokens[2])-1), tuples);
            constraints.push_back(cons);
        }
    }
    file.close();
    assert(number_of_constraints == constraints.size());
    return CSP(constraints = constraints, variables = variables);
}

CSP create_sudokus_CSP_tuples(const string instance_name) {
    vector<Variable> variables;
    vector<Constraint*> constraints;
    ifstream file(instance_name);

    // Check if the file is successfully opened
    if (!file.is_open()) {
        cerr << "Error opening the file!";
        return CSP();
    }
    string line;
    vector<string> line_splitted;

    getline(file, line);
    stringstream ss(line);
    string token;
    char delimiter = ' ';

    while (getline(ss, token, delimiter)) {
        line_splitted.push_back(token);
    }
    int grid_size = stoi(line_splitted[0]);
    int block_size = std::round(std::sqrt(grid_size));
    vector<int> domain;
    for (int i = 1; i < grid_size + 1; i++) {
        domain.push_back(i);
    }

    for (int i = 0; i < grid_size; i++) {
        getline(file, line);
        stringstream ss(line);
        string token;
        vector<string> line_splitted;
        char delimiter = ',';
        while (getline(ss, token, delimiter)) {
            line_splitted.push_back(token);
        }
        for (int j = 0; j < grid_size; j++) {
            string name_var = "X_" + std::to_string(i) + "_" + std::to_string(j);
            int var_idx = i * grid_size + j;
            int given_nb = stoi(line_splitted[j]);
            if (given_nb != 0) {
                variables.push_back(Variable(name_var, var_idx, { given_nb }));

            }
            else
                variables.push_back(Variable(name_var, var_idx, domain));
        }
    }
    for (int i = 0; i < grid_size * grid_size; i++) {
        for (int j = i + 1; j < grid_size * grid_size; j++) {
            int col_i = i % grid_size;
            int row_i = i / grid_size;
            int col_j = j % grid_size;
            int row_j = j / grid_size;
            int block_i = col_i / block_size + (row_i / block_size) * block_size;
            int block_j = col_j / block_size + (row_j / block_size) * block_size;

            // Only add constraints for variables in the same row, column, or block
            if (col_i == col_j || row_i == row_j || block_i == block_j) {
                vector<tuple<int, int>> admissible_tuples;

                // Populate admissible tuples where values differ
                for (int vi : domain) {
                    for (int vj : domain) {
                        if (vi != vj) {  // Values must be different
                            admissible_tuples.push_back(make_tuple(vi, vj));
                        }
                    }
                }

                // Create the constraint with the admissible tuples
                Constraint* cons = new Tuple_Constraint(make_pair(i, j), admissible_tuples);
                constraints.push_back(cons);
            }
        }
    }
    return CSP(constraints = constraints, variables = variables);
}


CSP create_sudokus_CSP_diff(const string instance_name) {
    vector<Variable> variables;
    vector<Constraint*> constraints;
    ifstream file(instance_name);

    // Check if the file is successfully opened
    if (!file.is_open()) {
        cerr << "Error opening the file!";
        return CSP();
    }
    string line;
    vector<string> line_splitted;

    getline(file, line);
    stringstream ss(line);
    string token;
    char delimiter = ' ';

    while (getline(ss, token, delimiter)) {
        line_splitted.push_back(token);
    }
    int grid_size = stoi(line_splitted[0]);
    int block_size = std::round(std::sqrt(grid_size));
    vector<int> domain;
    for (int i = 1; i < grid_size + 1; i++) {
        domain.push_back(i);
    }

    for (int i = 0; i < grid_size; i++) {
        getline(file, line);
        stringstream ss(line);
        string token;
        vector<string> line_splitted;
        char delimiter = ',';
        while (getline(ss, token, delimiter)) {
            line_splitted.push_back(token);
        }
        for (int j = 0; j < grid_size; j++) {
            string name_var = "X_" + std::to_string(i) + "_" + std::to_string(j);
            int var_idx = i * grid_size + j;
            int given_nb = stoi(line_splitted[j]);
            if (given_nb != 0) {
                variables.push_back(Variable(name_var, var_idx, { given_nb }));

            }
            else
                variables.push_back(Variable(name_var, var_idx, domain));
        }
    }
    for (int i = 0; i < grid_size * grid_size; i++) {
        for (int j = i + 1; j < grid_size * grid_size; j++) {
            int col_i = i % grid_size;
            int row_i = i / grid_size;
            int col_j = j % grid_size;
            int row_j = j / grid_size;
            int block_i = col_i / block_size + (row_i / block_size) * block_size;
            int block_j = col_j / block_size + (row_j / block_size) * block_size;
            if (col_i == col_j || row_i == row_j || block_i == block_j) {
                Constraint* cons = new Diff_Constraint(make_pair(i, j), make_pair(1, -1), 0);
                constraints.push_back(cons);
            }
        }
    }
    return CSP(constraints = constraints, variables = variables);
}
