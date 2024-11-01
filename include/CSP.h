#ifndef CSP_H
#define CSP_H

#include "constraint.h"
#include "variable.h"
#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <cassert>

#include <chrono>

using namespace std;

struct Backward_State {
    map<int, int> assignment;
    int variable_index;
    int value_index;
    map<int, int> domains_index;
};

class CSP {
public:
    vector<Constraint*> constraints;
    vector<Variable> variables;
    int number_of_nodes = 0;


    CSP() : constraints({}), variables({}) {}
    CSP(vector<Constraint*>& constraints, vector<Variable>& variables);
    bool is_complete(const map<int, int>& assignment) const {
        return (assignment.size() == variables.size());
    }
    ~CSP() {
        for (Constraint* cons : constraints)
            delete cons;
    }

    // Variable order
    int min_domain_size_variable_order(vector<int>& variable_order, map<int, int> domains_index);
    int most_constrained_variable_order(vector<int>& variable_order);
    int random_variable_order(vector<int>& variable_order);
    int select_next_variable_index(string variable_ordre_strategy, const map<int, int>& assigment, const map<int, int>& domains_index);

    // Value order
    void sort_values(string value_order_strategy, Variable& variable, int k);
    void min_value_order(Variable& current_variable, int k);
    void random_value_order(Variable& current_variable, int k);
    void most_supported_value_order(Variable& current_variable, int k);


    // constraints
    vector<Constraint*> get_constraints_linked_to(const int& variable_index);

    bool check_assignment_consistency(const map<int, int>& assignment, const int& variable_index, const int& value);
    bool update_variable_domain(const int& var_1_index, const int& var_2_index, vector<Variable>& variables);

    bool backtrack_iterative(map<int, int>& assignment, bool activate_FC, bool activate_MAC, string value_order_strategy,
        string variable_order_strategy, std::chrono::high_resolution_clock::time_point start, double max_duration);

    void forward_checking(const int& new_assigned_variable_idx, const int& assigned_value, map<int, int>& domains_index, const map<int, int>& current_assignment);

    bool AC_1();
    bool root_AC_3(map<int, int>& assigment);
    void leaf_AC_3(const int& new_assigned_variable_idx, const int& assigned_value,
        map<int, int>& domains_index, const map<int, int>& current_assignment);

    tuple<map<int, int>, int, double> solve(map<int, int>& assignment, bool activate_AC1 = false,
        bool activate_AC3 = true, bool activate_FC = true, bool activate_MAC = false, string value_order_strategy = "", string variable_order_startegy = "", double max_duration = 1000);
};

#endif