#include <iostream>

#include "variable.h"
#include "constraint.h"

int main()
{
    Variable A = Variable("A", {1, 2, 3});
    Variable B = Variable("B", {4, 2, 3});
    std::vector<std::tuple<int, int>> tuples = {std::make_tuple(1, 2)};
    Constraint cons = Constraint(A, B, tuples);

    std::cout << A.name << std::endl;
    std::cout << std::get<0>(cons.get_tuple(0)) << std::endl;
}
