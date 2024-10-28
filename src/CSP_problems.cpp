#include "CSP_problems.h"
#include "variable.h"
#include "constraint.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <random>

CSP create_n_queens_CSP(int n) {
    vector<int> domain;
    vector<Variable> variables;
    vector<Constraint> constraints;

    for (int i = 1; i < n + 1; i++) {
        domain.push_back(i);
    }

    for (int i = 0; i < n; i++) {
        Variable var("X_" + std::to_string(i), domain);
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
            Constraint cons(variables[i], variables[j], validPairs);
            constraints.push_back(cons);
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
    vector<Constraint> constraints;
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
                Variable var("X_" + std::to_string(i), domain);
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
            constraints.push_back(Constraint(var_1, var_2, tuples));
        }
    }
    file.close();
    assert(number_of_constraints == constraints.size());
    return CSP(constraints = constraints, variables = variables);
}


CSP generate_random_CSP(int num_variables, int domain_size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(0.0, 1.0);

    vector<Variable> variables;
    vector<Constraint> constraints;
    vector<int> domain;

    for (int i = 0; i < num_variables; i++) {
        string letter(1, static_cast<char>('A' + i));
        std::vector<int> domain;
        for (int j = 0; j < domain_size; j++) {
            domain.push_back(j);
        }
        Variable var(letter, domain);
        variables.push_back(var);
    }

    for (int i = 0; i < num_variables - 1; i++) {
        for (int j = i + 1; j < num_variables; j++) {
            std::vector<std::tuple<int, int>> allowed_pairs;
            for (int x = 0; x < domain_size; x++) {
                for (int y = 0; y < domain_size; y++) {
                    double z = distrib(gen);
                    if (z < 0.6) {
                        allowed_pairs.push_back(std::make_tuple(x, y));
                    }
                }
            }
            Constraint cons(variables[i], variables[j], allowed_pairs);
            constraints.push_back(cons);
        }
    }
    CSP csp(constraints = constraints, variables = variables);
    return csp;
}
