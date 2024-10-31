#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "variable.h"
#include <string>
#include <vector>
#include <tuple>
#include <cassert>
#include <map>
using namespace std;

class Mother_Constraint {
public:
    virtual pair<int, int> get_variable_indexs() = 0;
    virtual bool var_is_in_constraint(int var_index) = 0;
    virtual int get_var_index(int var) = 0;
    virtual int get_other_variable_index_from_variable(int var) = 0;
    virtual bool check_if_constraint_is_satisfied(int var_1_index, int val_1, int var_2_index, int val_2) = 0;
    virtual bool propagate(const int& new_assigned_variable_idx, const int& assigned_value,
        map<int, int>& domains_index, const map<int, int>& current_assignment, vector<Variable>& variables) = 0;

    virtual vector<tuple<int, int>> get_cons_tuples() {
        return {};
    }
};

class Constraint :public Mother_Constraint {
public:
    pair<int, int> variable_indexs;
    vector<tuple<int, int>> tuples;
    Constraint(pair<int, int> variable_indexs, vector<tuple<int, int>> tuples);

    pair<int, int> get_variable_indexs() {
        return variable_indexs;
    }

    bool var_is_in_constraint(int var_index) {
        if ((var_index == variable_indexs.first) || (var_index == variable_indexs.second))
            return true;
        return false;
    }
    int get_var_index(int variable_index) {
        assert(var_is_in_constraint(variable_index));
        if (variable_index == variable_indexs.first)
            return 0;
        else if (variable_index == variable_indexs.second)
            return 1;
        return -1;
    };

    int get_other_variable_index_from_variable(int variable_index) {
        assert(var_is_in_constraint(variable_index));
        if (variable_index == variable_indexs.first) {
            return variable_indexs.second;
        }
        else
            return variable_indexs.first;
    }

    vector<tuple<int, int>> get_cons_tuples() {
        return tuples;
    }

    bool check_if_constraint_is_satisfied(int var_1_index, int val_1, int var_2_index, int val_2);

    bool propagate(const int& new_assigned_variable_idx, const int& assigned_value, map<int, int>& domains_index,
        const map<int, int>& current_assignment, vector<Variable>& variables);
};


class Diff_Constraint :public Mother_Constraint {
public:
    pair<int, int> variable_indexs;
    pair<int, int> coeffs;
    int diff_nb;
    Diff_Constraint(pair<int, int> variable_indexs, pair<int, int> coeffs, int diff_nb);

    pair<int, int> get_variable_indexs() {
        return variable_indexs;
    }

    bool var_is_in_constraint(int var_index) {
        if ((var_index == variable_indexs.first) || (var_index == variable_indexs.second))
            return true;
        return false;
    }
    int get_var_index(int variable_index) {
        assert(var_is_in_constraint(variable_index));
        if (variable_index == variable_indexs.first)
            return 0;
        else if (variable_index == variable_indexs.second)
            return 1;
        return -1;
    };

    int get_other_variable_index_from_variable(int variable_index) {
        assert(var_is_in_constraint(variable_index));
        if (variable_index == variable_indexs.first) {
            return variable_indexs.second;
        }
        else
            return variable_indexs.first;
    }

    bool check_if_constraint_is_satisfied(int var_1_index, int val_1, int var_2_index, int val_2);

    bool propagate(const int& new_assigned_variable_idx, const int& assigned_value, map<int, int>& domains_index,
        const map<int, int>& current_assignment, vector<Variable>& variables);
};

#endif