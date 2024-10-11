#include <iostream>

#include "CSP.h"

CSP n_reines(int n)
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
    // Définir des contraintes pour éviter les conflits en diagonale et en colonne
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

    CSP csp(constraints = constraints, variables = variables);
    return csp;
}

int main()
{
    vector<int> domain{0, 1, 2};
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

    CSP csp_2 = n_reines(19);

    // for (Constraint cons : csp_2.constraints)
    // {
    //     cout << cons.var1.name << " " << cons.var2.name << "tuple : ";
    //     for (auto &tuple : cons.tuples)
    //     {
    //         cout << "{" << get<0>(tuple) << " " << get<1>(tuple) << "}";
    //     }
    //     cout << " " << endl;
    // }

    map<string, int> solution = csp_2.solve();
    for (const auto &[k, v] : solution)
        cout << "variable" << k << " : " << v << endl;
}
