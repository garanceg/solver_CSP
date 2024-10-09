#include "CSP.h"

#include <iostream>
#include <stdio.h>
#include <time.h>
#include <stack>
#include <algorithm>
#include <limits>

CSP::CSP(vector<Constraint> &constraints, vector<Variable> &variables) : constraints(constraints), variables(variables) {};

Variable CSP::select_min_size_domain_variable(const map<string, int> &assigment)
{
    assert(variables.size() >= assigment.size());
    Variable variable_to_select;
    int min_size_domain = numeric_limits<int>::max();
    for (const Variable var : variables)
    {
        if (assigment.count(var.name) > 0) // var is assigned
            continue;
        if (var.domain.size() < min_size_domain)
        {
            min_size_domain = var.domain.size();
            variable_to_select = var;
        };
    };
    return variable_to_select;
};

bool CSP::is_consistent(const std::map<std::string, int> &assignment, const Variable &variable, const int &value)
{
    // Iterate through all constraints
    for (const Constraint &cons : constraints)
    {
        Variable other_variable;
        int index_var;

        // Determine if the current variable is in the constraint
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
        {
            continue; // Skip if the variable is not part of the constraint
        }

        // Check if the value exists in the constraint's tuple
        bool value_in_constraint_tuple = false;
        for (const auto &t : cons.tuples)
        {
            if (index_var == 0 && (std::get<0>(t) == value))
            {
                value_in_constraint_tuple = true;
                break;
            }
            else if (index_var == 1 && (std::get<1>(t) == value))
            {
                value_in_constraint_tuple = true;
                break;
            }
        }

        // If the value is not valid according to the constraint, return false
        if (!value_in_constraint_tuple)
            return false;

        // If the other variable is not yet assigned AND the value is valid
        // The constraint is trivially respected
        if (assignment.count(other_variable.name) == 0)
            continue;

        // Check if the constraint is respected between
        bool cons_respected = false;
        for (const std::tuple<int, int> &t : cons.tuples)
        {
            if (index_var == 0 && (std::get<0>(t) == value))
            {
                if (std::get<1>(t) == assignment.at(other_variable.name))
                {
                    cons_respected = true;
                    break;
                }
            }
            else if (index_var == 1 && (std::get<1>(t) == value))
            {
                if (std::get<0>(t) == assignment.at(other_variable.name))
                {
                    cons_respected = true;
                    break;
                }
            }
        }

        // If one constraint is not respected, return false
        if (!cons_respected)
            return false;
    }

    // If all constraints are respected, return true
    return true;
}

// recursive backtrack
bool CSP::backtrack(map<string, int> &assignment)
{
    if (is_complete(assignment))
        return true;

    Variable next_assigned_var = select_min_size_domain_variable(assignment);

    for (int val : next_assigned_var.domain)
    {
        number_of_nodes += 1;
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

void CSP::forward_checking(Variable &new_assigned_variable, const int &assigned_value, map<string, int> &domains_index, const map<string, int> &current_assignment)
{

    for (Variable &var : variables)
    {
        // do not apply for already instanciated variables
        if ((current_assignment.count(var.name) > 0) || var.name == new_assigned_variable.name)
            continue;

        vector<tuple<int, int>> constraint_tuples;
        int cons_index_assigned_var = -1;
        for (const Constraint &cons : constraints)
        {
            // find the constraint associated with var and new_assigned_variable
            if (cons.var1.name == var.name && cons.var2.name == new_assigned_variable.name)
            {
                cons_index_assigned_var = 1;
                constraint_tuples = cons.tuples;
                break;
            }
            else if (cons.var2.name == var.name && cons.var1.name == new_assigned_variable.name)
            {
                cons_index_assigned_var = 0;
                constraint_tuples = cons.tuples;
                break;
            }
        };
        // if no constraint link the variables, skip
        if (cons_index_assigned_var == -1)
            continue;

        assert(domains_index[var.name] >= 0);
        // iterate on all the supported variable
        for (int i = 0; i < domains_index[var.name]; i++)
        {
            bool val_supported = false;
            int value = var.domain[i];
            // check if variable is supported
            for (const std::tuple<int, int> &t : constraint_tuples)
            {
                if ((cons_index_assigned_var == 0) && (std::get<0>(t) == assigned_value) && (std::get<1>(t) == value))
                {
                    val_supported = true;
                    break;
                }
                else if ((cons_index_assigned_var == 1) && (std::get<1>(t) == assigned_value) && (std::get<0>(t) == value))
                {
                    val_supported = true;
                    break;
                }
            };
            if (not val_supported)
            {
                // If the value is not supported we swap it with the value
                // just before the marker
                var.domain[i] = var.domain[domains_index[var.name] - 1];
                var.domain[domains_index[var.name] - 1] = value;
                // we then reduce the maker
                domains_index[var.name] -= 1;
            }
        }
    }
}

bool CSP::backtrack_iterative(map<string, int> &assignment)
{
    stack<Backward_State> stack;
    // Initialization with smaller domain OR given order
    Variable first_var = select_min_size_domain_variable(assignment);
    map<string, int> initial_domains_index;
    for (Variable var : variables)
        initial_domains_index[var.name] = var.domain.size();

    stack.push({assignment, first_var, 0, initial_domains_index});

    while (!stack.empty())
    {
        Backward_State &current_state = stack.top();
        map<string, int> current_assignment = current_state.assignment;
        Variable current_variable = current_state.variable;
        int value_index = current_state.value_index;
        map<string, int> current_domains_index = current_state.domains_index;

        // If not more supported values in the domain, backtrack
        if (value_index >= current_domains_index[current_variable.name])
        {
            stack.pop();
            continue;
        }
        number_of_nodes += 1;

        // branch with next value
        int value = current_variable.domain[value_index];
        current_state.value_index++;

        if (is_consistent(current_assignment, current_variable, value))
        {
            current_assignment[current_variable.name] = value;
            // forward checking
            forward_checking(current_variable, value, current_domains_index, current_assignment);

            if (is_complete(current_assignment))
            {
                assignment = current_assignment;
                return true;
            }
            // Next branch
            Variable next_variable = select_min_size_domain_variable(current_assignment);
            stack.push({current_assignment, next_variable, 0, current_domains_index});
        }
    }
    return false;
}

map<string, int> CSP::solve()
{
    map<string, int> assignment;
    clock_t start = clock();
    bool sol_found = backtrack_iterative(assignment);
    clock_t end = clock();
    double elapsed = double(end - start) / CLOCKS_PER_SEC;
    cout << "Visited " << number_of_nodes << " nodes." << endl;
    if (sol_found)
    {
        cout << "Solution found in " << elapsed << " seconds." << endl;
        return assignment;
    };
    cout << "No solution found in " << elapsed << " seconds." << endl;
    return assignment;
};
