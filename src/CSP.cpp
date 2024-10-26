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

// Fonction pour trouver les contraintes concernées par un couple de variables
std::vector<Constraint> CSP::find_constraints_for_variables(const std::vector<Constraint> &constraints, const std::string &var1Name, const std::string &var2Name)
{
    std::vector<Constraint> relevant_constraints;

    for (const Constraint &cons : constraints)
    {
        if (cons.var1.name == var1Name && cons.var2.name == var2Name)
        {
            relevant_constraints.push_back(cons);
        }
    }

    return relevant_constraints;
}

bool CSP::revise(Variable &var1, Variable &var2)
{
    // cout << "var to be tested " << var1.name << " " << var2.name << endl;
    vector<int> to_be_removed;
    // for (int val : var1.domain)
    //     cout << val;
    // cout << " " << endl;
    for (int value1 : var1.domain)
    {
        // std::cout << "value " << value1 << std::endl;
        bool supported = false;

        // On regarde s'il y a une valeur consistante dans le domaine de var2
        for (int value2 : var2.domain)
        {
            if (constraint_satisfied(var1, value1, var2, value2))
            {
                supported = true;
                break;
            }
        }
        // Si aucune valeur dans le domaine de var2 ne supporte value1, on le retire du domaine de var1
        if (!supported)
        {
            to_be_removed.push_back(value1);
        }
    }
    if (to_be_removed.size() == 0)
        return false;

    for (int val : to_be_removed)
        var1.domain.erase(std::remove(var1.domain.begin(), var1.domain.end(), val), var1.domain.end());

    // for (int val : var1.domain)
    //     cout
    //         << val;
    // cout << " " << endl;
    return true;
}

bool CSP::constraint_satisfied(Variable &var1, int value1, Variable &var2, int value2)
{
    for (const Constraint &cons : constraints)
    {
        if ((cons.var1.name == var1.name && cons.var2.name == var2.name) ||
            (cons.var1.name == var2.name && cons.var2.name == var1.name))
        {
            for (const auto &[v1, v2] : cons.tuples)
            {
                if ((v1 == value1 && v2 == value2) || (v2 == value1 && v1 == value2))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

int CSP::get_variable_index_from_name(string name)
{
    int var_index = 0;
    for (Variable var : variables)
    {
        if (var.name == name)
            return var_index;
        var_index += 1;
    }
    assert(var_index < variables.size());
    return -1;
}

bool CSP::AC_3(std::vector<Constraint> &constraints, vector<Variable> &variables)
{
    vector<tuple<int, int>> to_be_tested;
    for (const Constraint cons : constraints)
    {
        // cout << cons.var1.name << " " << cons.var2.name << endl;
        int index_var_1 = get_variable_index_from_name(cons.var1.name);
        int index_var_2 = get_variable_index_from_name(cons.var2.name);
        assert(index_var_1 != -1);
        assert(index_var_2 != -1);
        to_be_tested.push_back(std::make_tuple(index_var_1, index_var_2));
        to_be_tested.push_back(std::make_tuple(index_var_2, index_var_1));
    }

    while (to_be_tested.size() > 0)
    {
        // std::cout << "TBT size : " << to_be_tested.size() << std::endl;
        int index_var_1 = get<0>(to_be_tested[0]);
        int index_var_2 = get<1>(to_be_tested[0]);
        Variable &var_1 = variables[index_var_1];
        Variable &var_2 = variables[index_var_2];
        to_be_tested.erase(to_be_tested.begin());

        if (revise(var_1, var_2))
        {
            // Si le domaine de var_1 est réduit à zéro, le problème est inconsistant
            if (var_1.domain.size() == 0)
            {
                // std::cout << "inconsistent" << std::endl;
                return false;
            }

            // On ajoute les contraintes concernées par la modification de var_1
            for (const Constraint &cons : constraints)
            {
                if (cons.var1.name == var_1.name && cons.var2.name != var_2.name)
                {
                    int index_new_var = get_variable_index_from_name(cons.var2.name);
                    assert(index_new_var != -1);
                    to_be_tested.push_back(std::make_tuple(index_new_var, index_var_1));
                }
                else if (cons.var2.name == var_1.name && cons.var1.name != var_2.name)
                {
                    int index_new_var = get_variable_index_from_name(cons.var1.name);
                    assert(index_new_var != -1);
                    to_be_tested.push_back(std::make_tuple(index_new_var, index_var_1));
                }
            }
        }
    }
    return true;
}

map<string, int> CSP::solve(std::vector<Constraint> &constraints, vector<Variable> &variables)
{
    map<string, int> assignment;
    cout << "Domain sizes before AC_3: [";
    for (Variable var : variables)
        {
            cout << var.domain.size() << ", ";
        }
        cout << "]" << endl;
    clock_t start = clock();
    bool csp_is_consistent = AC_3(constraints, variables);
    clock_t end_consistence = clock();
    double time_consistence = double(end_consistence - start) / CLOCKS_PER_SEC;
    cout << "Checked consistence in " << time_consistence << " seconds" << endl;
    cout << "Domain sizes after AC_3: [";
    for (Variable var : variables)
        {
            cout << var.domain.size() << ", ";
        }
        cout << "]" << endl;
    if (!csp_is_consistent)
    {
        cout << "CSP is inconsistent because of these variables: ";
        for (Variable var : variables)
        {
            if (var.domain.size() == 0)
                cout << var.name;
        }
        cout << " " << endl;
        return assignment;
    }
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
