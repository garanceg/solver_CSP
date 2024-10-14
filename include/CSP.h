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

struct Backward_State
{
    map<string, int> assignment;
    Variable variable;
    int value_index;
    map<string, int> domains_index;
};

class CSP
{
public:
    vector<Constraint> constraints;
    vector<Variable> variables;
    int number_of_nodes = 0;
    CSP() : constraints({}), variables({}) {}
    CSP(vector<Constraint> &constraints, vector<Variable> &variables);
    bool is_complete(const map<string, int> &assignment) const
    {
        return (assignment.size() == variables.size());
    }
    void forward_checking(Variable &new_assigned_variable, const int &assigned_value, map<string, int> &domains_index, const map<string, int> &current_assignment);
    bool backtrack(map<string, int> &assigment);
    bool backtrack_iterative(map<string, int> &assignment);
    Variable select_min_size_domain_variable(const map<string, int> &assigment);
    bool is_consistent(const map<string, int> &assignment, const Variable &variable, const int &value);
    map<string, int> solve();
};

#endif