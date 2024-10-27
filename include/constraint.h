#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "variable.h"
#include <string>
#include <vector>
#include <tuple>
#include <cassert>
using namespace std;

class Constraint {
public:
    Variable var1;
    Variable var2;
    vector<tuple<int, int>> tuples;
    Constraint(Variable var1, Variable var2, vector<tuple<int, int>> tuples);
    tuple<int, int> get_tuple(int i) {
        assert(i < tuples.size());
        return tuples[i];
    }
};

#endif