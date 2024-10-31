#include "constraint.h"
#include "variable.h"

#include <iostream>

Constraint::Constraint(pair<int, int> variable_indexs, vector<tuple<int, int>> tuples) : variable_indexs(variable_indexs), tuples(tuples) {};


bool Constraint::check_if_constraint_is_satisfied(int var_1_index, int val_1, int var_2_index, int val_2) {
    assert(var_is_in_constraint(var_1_index));
    assert(var_is_in_constraint(var_2_index));
    int cons_var_index = get_var_index(var_1_index);
    if (cons_var_index == 0) {
        for (const tuple<int, int>& t : tuples) {
            if (std::get<0>(t) == val_1 && std::get<1>(t) == val_2) {
                return true;
            }
        }
    }
    else if (cons_var_index == 1) {
        for (const tuple<int, int>& t : tuples) {
            if (std::get<1>(t) == val_1 && std::get<0>(t) == val_2) {
                return true;
            }
        }
    }
    return false;
}


bool Constraint::propagate(const int& new_assigned_variable_idx, const int& assigned_value,
    map<int, int>& domains_index, const map<int, int>& current_assignment, vector<Variable>& variables) {
    bool has_propagated = false;
    int other_variable_index = get_other_variable_index_from_variable(new_assigned_variable_idx);
    if ((current_assignment.count(other_variable_index) > 0))
        return has_propagated;
    for (int i = 0; i < domains_index[other_variable_index]; i++) {
        int value = variables[other_variable_index].domain[i];
        if (not check_if_constraint_is_satisfied(new_assigned_variable_idx, assigned_value, other_variable_index, value)) {
            has_propagated = true;
            // If the value is not supported we swap it with the value just before the marker
            std::swap(variables[other_variable_index].domain[i], variables[other_variable_index].domain[domains_index[other_variable_index] - 1]);
            // Reduce the marker
            domains_index[other_variable_index] -= 1;
        }
    }
    return has_propagated;
}


Diff_Constraint::Diff_Constraint(pair<int, int> variable_indexs, pair<int, int> coeffs, int diff_nb) : variable_indexs(variable_indexs), coeffs(coeffs), diff_nb(diff_nb) {};

bool Diff_Constraint::check_if_constraint_is_satisfied(int var_1_index, int val_1, int var_2_index, int val_2) {
    assert(var_is_in_constraint(var_1_index));
    assert(var_is_in_constraint(var_2_index));
    int cons_var_1_index = get_var_index(var_1_index);
    if (cons_var_1_index == 0) {
        if (val_1 * coeffs.first + val_2 * coeffs.second != diff_nb)
            return true;
    }
    else if (cons_var_1_index == 1) {
        if (val_1 * coeffs.second + val_2 * coeffs.first != diff_nb)
            return true;
    }
    return false;
}

bool Diff_Constraint::propagate(const int& new_assigned_variable_idx, const int& assigned_value, map<int, int>& domains_index,
    const map<int, int>& current_assignment, vector<Variable>& variables) {
    bool has_propagated = false;
    int other_variable_index = get_other_variable_index_from_variable(new_assigned_variable_idx);
    if ((current_assignment.count(other_variable_index) > 0))
        return has_propagated;
    int cons_var_1_index = get_var_index(new_assigned_variable_idx);
    int forbidden_value;
    if (cons_var_1_index == 0)
    {
        forbidden_value = (diff_nb - assigned_value * coeffs.first) / coeffs.second;
    }
    else if (cons_var_1_index == 1) {
        forbidden_value = (diff_nb - assigned_value * coeffs.second) / coeffs.first;
    }
    for (int i = 0; i < domains_index[other_variable_index]; i++) {
        if (variables[other_variable_index].domain[i] == forbidden_value) {
            has_propagated = true;
            // If the value is not supported we swap it with the value just before the marker
            std::swap(variables[other_variable_index].domain[i], variables[other_variable_index].domain[domains_index[other_variable_index] - 1]);
            // Reduce the marker
            domains_index[other_variable_index] -= 1;
        }
    }
    return has_propagated;
}