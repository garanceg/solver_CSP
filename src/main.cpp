#include <iostream>
#include <fstream>
#include <sstream>
#include <random>

#include "variable.h"
#include "constraint.h"
#include "CSP.h"

using namespace std;

CSP create_n_queens_CSP(int n)
{
    vector<int> domain;
    vector<Variable> variables;
    vector<Constraint> constraints;

    for (int i = 1; i < n + 1; i++)
    {
        domain.push_back(i);
    }

    for (int i = 0; i < n; i++)
    {
        Variable var("X_" + std::to_string(i), domain);
        variables.push_back(var);
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            vector<tuple<int, int>> validPairs;
            for (int vi : domain)
            {
                for (int vj : domain)
                {
                    if (vi != vj && abs(vi - vj) != abs(i - j))
                    {
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

CSP create_graph_coloring_CSP(const string instance_name, const int number_of_color)
{
    ifstream file(instance_name);

    // Check if the file is successfully opened
    if (!file.is_open())
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
    }

    int number_of_nodes;
    int number_of_constraints;

    while (getline(file, s))
    {
        if (s[0] == 'p')
        {
            stringstream ss(s);
            string token;
            vector<string> tokens;
            char delimiter = ' ';

            while (getline(ss, token, delimiter))
            {
                tokens.push_back(token);
            }
            number_of_nodes = stoi(tokens[2]);
            number_of_constraints = stoi(tokens[3]);
            cout << "Number of nodes variables : " << number_of_nodes << endl;
            cout << "Number of nodes constraints : " << number_of_constraints << endl;

            for (int i = 0; i < number_of_nodes; i++)
            {
                Variable var("X_" + std::to_string(i), domain);
                variables.push_back(var);
            }
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

CSP generate_random_CSP(int num_variables, int domain_size)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(0.0, 1.0);

    vector<Variable> variables;
    vector<Constraint> constraints;
    vector<int> domain;

    for (int i = 0; i < num_variables; i++)
    {
        string letter(1, static_cast<char>('A' + i));
        std::vector<int> domain;
        for (int j = 0; j < domain_size; j++)
        {
            domain.push_back(j);
        }
        Variable var(letter, domain);
        variables.push_back(var);
    }

    for (int i = 0; i < num_variables - 1; i++)
    {
        for (int j = i + 1; j < num_variables; j++)
        {
            std::vector<std::tuple<int, int>> allowed_pairs;
            for (int x = 0; x < domain_size; x++)
            {
                for (int y = 0; y < domain_size; y++)
                {
                    double z = distrib(gen);
                    if (z < 0.6)
                    {
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

int main()
{
    // int num_variables = 10;    // Nombre de variables
    // int domain_size = 5;      // Taille du domaine
    // CSP csp = generate_random_CSP(num_variables, domain_size);

    // vector<int> domain{0, 1, 2};
    // Variable A = Variable("A", domain);
    // Variable B = Variable("B", domain);
    // Variable C = Variable("C", domain);
    // // Variable D = Variable("D", domain);
    // vector<Variable> variables{A, B, C};

    // vector<tuple<int, int>> tuples_AB = {make_tuple(2, 1), make_tuple(2, 2)};
    // Constraint cons_AB = Constraint(A, B, tuples_AB);
    // vector<tuple<int, int>> tuples_BC = {make_tuple(1, 2), make_tuple(0, 2)};
    // Constraint cons_BC = Constraint(B, C, tuples_BC);
    // vector<tuple<int, int>> tuples_AC = {make_tuple(1, 1), make_tuple(2, 2)};
    // Constraint cons_AC = Constraint(C, A, tuples_AC);
    // vector<tuple<int, int>> tuples_AD = {make_tuple(2, 0), make_tuple(3, 2)};
    // Constraint cons_AD = Constraint(A, D, tuples_AD);
    // vector<tuple<int, int>> tuples_BD = {make_tuple(2, 0), make_tuple(3, 2)};
    // Constraint cons_BD = Constraint(B, D, tuples_BD);
    // vector<tuple<int, int>> tuples_CD = {make_tuple(2, 0), make_tuple(3, 2)};
    // Constraint cons_CD = Constraint(C, D, tuples_CD);
    // vector<Constraint> constraints{cons_AB, cons_AC, cons_BC, cons_AD, cons_BD, cons_CD};

    // vector<Constraint> constraints{cons_AB, cons_AC, cons_BC};
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
    CSP csp = create_n_queens_CSP(10);

    bool activate_AC1 = false;
    bool activate_AC3 = true;
    bool activate_FC = true;

    tuple<map<string, int>, int, double> solution = csp.solve(csp.constraints, csp.variables, activate_AC1, activate_AC3,
        activate_FC);

    for (const auto& [k, v] : std::get<0>(solution))
        cout << "variable" << k << " : " << v << endl;
}
