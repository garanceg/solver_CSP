#include <vector>

#include "variable.h"
#include "constraint.h"
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

    return CSP(constraints = constraints, variables = variables);
}