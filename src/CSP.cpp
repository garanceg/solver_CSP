#include "CSP.h"

#include <iostream>

CSP::CSP(const vector<Constraint> &constraints, const vector<Variable> &variables) : constraints(constraints), variables(variables) {};

vector<Variable> CSP::get_all_unassigned_variables(const map<string, int> &assigment)
{
    assert(variables.size() >= assigment.size());
    vector<Variable> unassigned_variables;
    for (int i = 0; i < variables.size(); i++)
    {
        if (assigment.count(variables[i].name) == 0)
        {
            unassigned_variables.push_back(variables[i]);
        };
    };
    return unassigned_variables;
};

Variable CSP::select_min_size_domain_variable(const map<string, int> &assigment)
{
    assert(variables.size() >= assigment.size());
    vector<Variable> unassigned_variables = get_all_unassigned_variables(assigment);
    assert(unassigned_variables.size() > 0);
    Variable variable_to_select = unassigned_variables[0];
    int min_size_domain = unassigned_variables[0].domain.size();
    for (int i = 0; i < unassigned_variables.size(); i++)
    {
        if (unassigned_variables[i].domain.size() < min_size_domain)
        {
            min_size_domain = unassigned_variables[i].domain.size();
            variable_to_select = unassigned_variables[i];
        };
    };
    return variable_to_select;
};

bool CSP::is_consistent(const map<string, int> &assignment, const Variable &variable, const int &value)
{
    // assert(value in variale.domain)
    for (Constraint cons : constraints)
    {
        Variable other_variable;
        int index_var;
        if (cons.var1.name == variable.name)
        {
            other_variable = cons.var2;
            index_var = 0;
        }
        else if (cons.var2.name == variable.name)
        {
            other_variable = cons.var1;
            index_var = 1;
        }
        else
            continue;

        // check if value in constraint tuple
        bool value_in_constraint_tuple = false;
        for (int j = 0; j < cons.tuples.size(); j++)
        {
            if (index_var == 0 && (get<0>(cons.get_tuple(j)) == value))
            {
                value_in_constraint_tuple = true;
                break;
            }
            else if (index_var == 1 && (get<1>(cons.get_tuple(j)) == value))
            {
                value_in_constraint_tuple = true;
                break;
            }
        };
        if (not value_in_constraint_tuple)
            return false;

        // if variable already assigned constraint respected
        if (assignment.count(other_variable.name) == 0)
            return true;
        // check if constraint respected
        bool cons_respected = false;
        for (tuple t : cons.tuples)
        {
            if (index_var == 0 && (get<0>(t) == value))
            {
                if (get<1>(t) == assignment.at(other_variable.name))
                {
                    cons_respected = true;
                    break;
                }
            }
            else if (index_var == 1 && (get<1>(t) == value))
            {
                if (get<0>(t) == assignment.at(other_variable.name))
                {
                    cons_respected = true;
                    break;
                }
            };
        };
        if (not cons_respected)
            return false;
    };
    return true;
};

bool CSP::backtrack(map<string, int> &assignment)
{
    if (is_complete(assignment))
        return true;

    Variable next_assigned_var = select_min_size_domain_variable(assignment);

    for (int val : next_assigned_var.domain)
    {
        cout << "branchement | variable = " << next_assigned_var.name << " valeur= " << val << endl;
        if (is_consistent(assignment, next_assigned_var, val))
        {
            assignment.insert(pair<string, int>(next_assigned_var.name, val));

            if (backtrack(assignment))
                return true;

            assignment.erase(next_assigned_var.name);
        }
    }

    return false;
};

map<string, int> CSP::solve()
{
    map<string, int> assignment;
    if (backtrack(assignment))
    {
        return assignment;
    };
    cout << "No solution found" << endl;
    return assignment;
};
