#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include <vector>
using namespace std;

class Variable {
public:
    string name;
    int idx;
    vector<int> domain;
    // Variable() : name(""), idx(0), domain({}) {}
    Variable(string name, int idx, vector<int>& domain);
};

#endif