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

class CSP
{
public:
    vector<Constraint> constraints;
    vector<Variable> variables;
    CSP(const vector<Constraint> &constraints, const vector<Variable> &variables);
    bool is_complete(const map<string, int> &assignment) const
    {
        return (assignment.size() == variables.size());
    }

    bool backtrack(map<string, int> &assigment);
    vector<Variable> get_all_unassigned_variables(const map<string, int> &assigment);
    Variable select_min_size_domain_variable(const map<string, int> &assigment);
    bool is_consistent(const map<string, int> &assignment, const Variable &variable, const int &value);
    map<string, int> solve();
    };

#endif