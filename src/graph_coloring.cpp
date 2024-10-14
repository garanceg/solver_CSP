#include <iostream>
#include <fstream>
#include <sstream>

#include "variable.h"
#include "constraint.h"
#include "CSP.h"
using namespace std;

CSP graph_coloring(const string instance_name, const int number_of_color)
{
    ifstream f(instance_name);

    // Check if the file is successfully opened
    if (!f.is_open())
    {
        cerr << "Error opening the file!";
        return CSP();
    }

    // String variable to store the read data
    string s;
    vector<Variable> variables;
    vector<Constraint> constraints;
    vector<int> domain;
    vector<tuple<int, int>> tuples;
    for (int i = 0; i < number_of_color; i++)
    {
        domain.push_back(i);
    }
    for (int i = 0; i < number_of_color; i++)
    {
        for (int j = 0; j < number_of_color; j++)
        {
            if (i != j)
            {
                tuples.push_back(make_tuple(i, j));
            }
        }
    };

    int number_of_nodes;
    int number_of_constraints;

    while (getline(f, s))
    {
        // Ensure the first character is compared correctly
        if (s[0] == 'p')
        {
            stringstream ss(s);
            string token;
            vector<string> tokens;
            char delimiter = ' ';

            while (getline(ss, token, delimiter))
            {
                tokens.push_back(token);
            };
            number_of_nodes = stoi(tokens[2]);
            number_of_constraints = stoi(tokens[3]);
            // Output the string after splitting
            cout << "Number of nodes variables : " << number_of_nodes << endl;
            cout << "Number of nodes constraints : " << number_of_constraints << endl;

            for (int i = 0; i < number_of_nodes; i++)
            {
                Variable var("X_" + std::to_string(i), domain);
                variables.push_back(var);
            };
            assert(number_of_nodes == variables.size());
        }

        if (s[0] == 'e')
        {
            stringstream ss(s);
            string token;
            vector<string> tokens;
            char delimiter = ' ';

            while (getline(ss, token, delimiter))
            {
                tokens.push_back(token);
            };
            assert(tokens.size() == 3);
            assert(stoi(tokens[1]) <= number_of_nodes);
            assert(stoi(tokens[2]) <= number_of_nodes);
            Variable var_1 = variables[stoi(tokens[1]) - 1];
            Variable var_2 = variables[stoi(tokens[2]) - 1];
            constraints.push_back(Constraint(var_1, var_2, tuples));
        }
    }
    f.close();
    assert(number_of_constraints == constraints.size());

    return CSP(constraints = constraints, variables = variables);
}