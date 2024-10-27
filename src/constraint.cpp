#include "constraint.h"
#include "variable.h"

Constraint::Constraint(Variable var1, Variable var2, vector<tuple<int, int>> tuples) : var1(var1), var2(var2), tuples(tuples) {};
