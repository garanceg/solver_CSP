#ifndef CSP_H
#define CSP_H

#include "constraint.h"
#include "variable.h"
#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <cassert>
using namespace std;

struct Backward_State {
    map<string, int> assignment;
    Variable variable;
    int value_index;
    map<string, int> domains_index;
};

class CSP {
public:
    vector<Constraint> constraints;
    vector<Variable> variables;
    int number_of_nodes = 0;

    CSP() : constraints({}), variables({}) {}
    CSP(vector<Constraint>& constraints, vector<Variable>& variables);
    bool is_complete(const map<string, int>& assignment) const {
        return (assignment.size() == variables.size());
    }

    int get_variable_index_from_name(string name);

    // Variable order
    void sort_variables(string variable_order_strategy, vector<Variable>& variables);
    void min_domain_size_variable_order(vector<Variable>& variables);
    void most_constrained_variable_order(vector<Variable>& variables);
    void random_variable_order(vector<Variable>& variables);
    Variable select_next_variable(const map<string, int>& assigment);

    bool check_constraint_satisfaction(Variable& var1, int value1, Variable& var2, int value2);
    bool check_assignment_consistency(const map<string, int>& assignment, const Variable& variable, const int& value);
    bool update_variable_domain(Variable& var1, Variable& var2);

    bool backtrack_iterative(map<string, int>& assignment, bool activate_FC, map<string, vector<int>> sorted_domains);

    void forward_checking(Variable& new_assigned_variable, const int& assigned_value, map<string, int>& domains_index, const map<string, int>& current_assignment);

    bool AC_1(std::vector<Constraint>& constraints, vector<Variable>& variables);
    bool AC_3(std::vector<Constraint>& constraints, vector<Variable>& variables);

    tuple<map<string, int>, int, double> solve(std::vector<Constraint>& constraints, vector<Variable>& variables,
        bool activate_AC1 = false, bool activate_AC3 = true, bool activate_FC = true, string value_order_strategy = "", string variable_order_startegy = "");
};

#endif