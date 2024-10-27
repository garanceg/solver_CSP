#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include <vector>
using namespace std;

class Variable {
public:
    string name;
    vector<int> domain;
    Variable() : name(""), domain({}) {}
    Variable(string name, vector<int> &domain);
};

#endif