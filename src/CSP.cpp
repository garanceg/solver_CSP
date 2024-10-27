#include "CSP.h"

#include <iostream>
#include <stdio.h>
#include <time.h>
#include <stack>
#include <algorithm>
#include <limits>

CSP::CSP(vector<Constraint> &constraints, vector<Variable> &variables) : constraints(constraints), variables(variables) {};

int CSP::get_variable_index_from_name(string name) {
    int var_index = 0;
    for (Variable var : variables) {
        if (var.name == name)
            return var_index;
        var_index += 1;
    }
    assert(var_index < variables.size());
    return -1;
}

Variable CSP::select_min_size_domain_variable(const map<string, int> &assigment) {
    assert(variables.size() >= assigment.size());
    Variable selected_variable;
    int min_size_domain = numeric_limits<int>::max();
    for (const Variable var : variables) {
        if (assigment.count(var.name) == 0 && var.domain.size() < min_size_domain) {
            min_size_domain = var.domain.size();
            selected_variable = var;
        };
    };
    return selected_variable;
};


bool CSP::check_assignment_consistency(const std::map<std::string, int> &assignment, const Variable &variable, const int &value) {
    for (const Constraint &cons : constraints) {
        Variable other_var;
        int index_var;

        // Determine if the current variable is in the constraint
        if (cons.var1.name == variable.name) {
            other_var = cons.var2;
            index_var = 0;
        }
        else if (cons.var2.name == variable.name) {
            other_var = cons.var1;
            index_var = 1;
        }
        else continue; 

        // Check if the value exists in the constraint's tuples
        bool value_in_tuple = false;
        for (const tuple<int, int> &t : cons.tuples) {
            if (index_var == 0 && (std::get<0>(t) == value)) {
                value_in_tuple = true;
                break;
            }
            else if (index_var == 1 && (std::get<1>(t) == value)) {
                value_in_tuple = true;
                break;
            }
        }
        // If the value is not valid according to the constraint, return false
        if (!value_in_tuple) return false;

        // If the other variable is not yet assigned AND the value is valid
        // The constraint is trivially respected
        if (assignment.count(other_var.name) == 0) continue;

        // Check if the constraint is respected between
        bool constraint_satisfied  = false;
        for (const tuple<int, int> &t : cons.tuples) {
            if (index_var == 0 && std::get<0>(t) == value && std::get<1>(t) == assignment.at(other_var.name)) {
                constraint_satisfied = true;
                break;
            } 
            else if (index_var == 1 && std::get<1>(t) == value && std::get<0>(t) == assignment.at(other_var.name)) {
                constraint_satisfied = true;
                break;
            }
            
        }
        if (!constraint_satisfied) return false;
    }
    return true;
}

bool CSP::check_constraint_satisfaction(Variable &var1, int val1, Variable &var2, int val2) {
    for (const Constraint &cons : constraints) {
        if (cons.var1.name == var1.name && cons.var2.name == var2.name) {
            for (const auto &[v1, v2] : cons.tuples) {
                if (v1 == val1 && v2 == val2) {
                    return true;
                }
            }
        }
        else if (cons.var1.name == var2.name && cons.var2.name == var1.name) {
            for (const auto &[v1, v2] : cons.tuples) {
                if ((v2 == val1 && v1 == val2)) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Revise the domain of variable 1 according to the values supported by variable 2
bool CSP::update_variable_domain(Variable &var1, Variable &var2) {
    // cout << "var to be tested " << var1.name << " " << var2.name << endl;
    vector<int> to_be_removed;
    // for (int val : var1.domain)
    //     cout << val;
    // cout << " " << endl;
    for (int val1 : var1.domain) {
        // std::cout << "value " << val1 << std::endl;
        bool supported = false;

        for (int val2 : var2.domain) {
            if (check_constraint_satisfaction(var1, val1, var2, val2)) {
                supported = true;
                break;
            }
        }
        if (!supported) to_be_removed.push_back(val1);
    }
    if (to_be_removed.size() == 0) return false;

    for (int val : to_be_removed)
        var1.domain.erase(std::remove(var1.domain.begin(), var1.domain.end(), val), var1.domain.end());

    // for (int val : var1.domain)
    //     cout
    //         << val;
    // cout << " " << endl;
    return true;
}

void CSP::forward_checking(Variable &new_assigned_variable, const int &assigned_value, 
        map<string, int> &domains_index, const map<string, int> &current_assignment) {

    for (Variable &var : variables) {
        // Apply only for not instanciated variables
        if ((current_assignment.count(var.name) > 0) || var.name == new_assigned_variable.name)
            continue;

        int cons_index_assigned_var = -1;
        vector<tuple<int, int>> constraint_tuples;
        
        for (const Constraint &cons : constraints) {
            // Find the constraint associated with var and new_assigned_var
            if (cons.var1.name == var.name && cons.var2.name == new_assigned_variable.name) {
                cons_index_assigned_var = 1;
                constraint_tuples = cons.tuples;
                break;
            }
            else if (cons.var2.name == var.name && cons.var1.name == new_assigned_variable.name) {
                cons_index_assigned_var = 0;
                constraint_tuples = cons.tuples;
                break;
            }
        }

        if (cons_index_assigned_var == -1) continue;

        assert(domains_index[var.name] >= 0);
        // Iterate on all the supported variable
        for (int i = 0; i < domains_index[var.name]; i++) {
            bool val_supported = false;
            int value = var.domain[i];
            // Check if variable is supported
            for (const std::tuple<int, int> &t : constraint_tuples) {
                if ((cons_index_assigned_var == 0) && (std::get<0>(t) == assigned_value) && (std::get<1>(t) == value)) {
                    val_supported = true;
                    break;
                }
                else if ((cons_index_assigned_var == 1) && (std::get<1>(t) == assigned_value) && (std::get<0>(t) == value)) {
                    val_supported = true;
                    break;
                }
            }
            if (not val_supported) {
                // If the value is not supported we swap it with the value just before the marker
                std::swap(var.domain[i], var.domain[domains_index[var.name] - 1]);
                // Reduce the marker
                domains_index[var.name] -= 1;
            }
        }
    }
}

bool CSP::backtrack(map<string, int> &assignment) {
    if (is_complete(assignment)) return true;

    Variable var = select_min_size_domain_variable(assignment);

    for (int val : var.domain) {
        number_of_nodes++;
        if (check_assignment_consistency(assignment, var, val)) {
            assignment.insert(pair<string, int>(var.name, val));
            if (backtrack(assignment)) return true;
            assignment.erase(var.name);
        }
    }
    return false;
}

bool CSP::backtrack_iterative(map<string, int> &assignment, bool activate_FC) {
    stack<Backward_State> stack;
    // Initialization with smaller domain OR given order
    Variable first_var = select_min_size_domain_variable(assignment);
    map<string, int> initial_domains_index;
    for (Variable var : variables)
        initial_domains_index[var.name] = var.domain.size();

    stack.push({assignment, first_var, 0, initial_domains_index});

    while (!stack.empty()) {
        Backward_State &state = stack.top();
        map<string, int> current_assignment = state.assignment;
        Variable current_variable = state.variable;
        int value_index = state.value_index;
        map<string, int> current_domains_index = state.domains_index;

        // If not more supported values in the domain, backtrack
        if (value_index >= current_domains_index[current_variable.name]) {
            stack.pop();
            continue;
        }
        number_of_nodes++;

        // Branch with next value
        int value = current_variable.domain[value_index];
        state.value_index++;

        if (check_assignment_consistency(current_assignment, current_variable, value)) {
            current_assignment[current_variable.name] = value;

            if (activate_FC)
                forward_checking(current_variable, value, current_domains_index, current_assignment);

            if (is_complete(current_assignment)) {
                assignment = current_assignment;
                return true;
            }
            // Next branch
            Variable next_variable = select_min_size_domain_variable(current_assignment);
            stack.push({current_assignment, next_variable, 0, current_domains_index});
        }
    }
    return false;
}

bool CSP::AC_1(std::vector<Constraint> &constraints, vector<Variable> &variables) {
    bool term = false;
    while (term == false) {
        term = true;
        for (const Constraint cons : constraints) {
            int index_var_1 = get_variable_index_from_name(cons.var1.name);
            int index_var_2 = get_variable_index_from_name(cons.var2.name);
            Variable &var_1 = variables[index_var_1];
            Variable &var_2 = variables[index_var_2];
            if (update_variable_domain(var_1, var_2)) {
                if (var_1.domain.size() == 0) {
                    return false;
                }
                term = false;
            }
        }
    }
    return true;
}

bool CSP::AC_3(std::vector<Constraint> &constraints, vector<Variable> &variables) {
    vector<tuple<int, int>> to_be_tested;
    for (const Constraint cons : constraints) {
        int idx1 = get_variable_index_from_name(cons.var1.name);
        int idx2 = get_variable_index_from_name(cons.var2.name);
        assert(idx1 != -1);
        assert(idx2 != -1);
        to_be_tested.push_back(std::make_tuple(idx1, idx2));
        to_be_tested.push_back(std::make_tuple(idx2, idx1));
    }

    while (to_be_tested.size() > 0) {
        // std::cout << "TBT size : " << to_be_tested.size() << std::endl;
        int idx1 = get<0>(to_be_tested[0]);
        int idx2 = get<1>(to_be_tested[0]);
        Variable &var_1 = variables[idx1];
        Variable &var_2 = variables[idx2];
        to_be_tested.erase(to_be_tested.begin());

        if (update_variable_domain(var_1, var_2)) {
            // Si le domaine de var_1 est réduit à zéro, le problème est inconsistant
            if (var_1.domain.size() == 0) {
                return false;
            }

            // On ajoute les contraintes concernées par la modification de var_1
            for (const Constraint &cons : constraints) {
                if (cons.var1.name == var_1.name && cons.var2.name != var_2.name) {
                    int index_new_var = get_variable_index_from_name(cons.var2.name);
                    assert(index_new_var != -1);
                    to_be_tested.push_back(std::make_tuple(index_new_var, idx1));
                }
                else if (cons.var2.name == var_1.name && cons.var1.name != var_2.name) {
                    int index_new_var = get_variable_index_from_name(cons.var1.name);
                    assert(index_new_var != -1);
                    to_be_tested.push_back(std::make_tuple(index_new_var, idx1));
                }
            }
        }
    }
    return true;
}

tuple<map<string, int>, int, double> CSP::solve(std::vector<Constraint> &constraints, vector<Variable> &variables,
        bool activate_AC1, bool activate_AC3, bool activate_FC) {

    map<string, int> assignment;
    cout << "Domain sizes before AC_3: [";

    for (Variable var : variables) {
            cout << var.domain.size() << ", ";
        }
        cout << "]" << endl;

    clock_t start = clock();
    bool csp_is_consistent = true;

    if (activate_AC3)
        csp_is_consistent = AC_3(constraints, variables);
    
    clock_t end_consistence = clock();
    double time_consistence = double(end_consistence - start) / CLOCKS_PER_SEC;
    cout << "Checked consistence in " << time_consistence << " seconds" << endl;
    cout << "Domain sizes after AC_3: [";
    for (Variable var : variables) {
            cout << var.domain.size() << ", ";
        }
        cout << "]" << endl;
    if (!csp_is_consistent) {
        cout << "CSP is inconsistent because of these variables: ";
        for (Variable var : variables) {
            if (var.domain.size() == 0)
                cout << var.name;
        }
        cout << " " << endl;
        return make_tuple(assignment, 0, 0);
    }

    bool sol_found = backtrack_iterative(assignment, activate_FC);
    clock_t end = clock();
    double elapsed = double(end - start) / CLOCKS_PER_SEC;
    cout << "Visited " << number_of_nodes << " nodes." << endl;
    if (sol_found) {
        cout << "Solution found in " << elapsed << " seconds." << endl;
        return make_tuple(assignment, number_of_nodes, elapsed);
    }
    cout << "No solution found in " << elapsed << " seconds." << endl;
    return make_tuple(assignment, number_of_nodes, elapsed);
}
