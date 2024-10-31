#include "CSP.h"

#include <iostream>
#include <stdio.h>
#include <time.h>
#include <stack>
#include <algorithm>
#include <limits>
#include <set>
#include <random> 
#include <stdexcept>
#include <sstream>

CSP::CSP(vector<Mother_Constraint*>& constraints, vector<Variable>& variables) : constraints(constraints), variables(variables) {};

///////////////////////////
//     UTILS FUNCTONS    //
///////////////////////////

vector<Mother_Constraint*> CSP::get_constraints_linked_to(const int& variable_index) {
    vector<Mother_Constraint*> linked_constraints;
    for (auto& cons : constraints) {
        if (cons->var_is_in_constraint(variable_index))
            linked_constraints.push_back(cons);
    }
    return linked_constraints;
}


///////////////////////////
//  Variable order strat //
///////////////////////////

int CSP::select_next_variable_index(string variable_order_strategy, const map<int, int>& assigment, const map<int, int>& domains_index) {
    vector<int> unassigned_variable_order;
    for (const Variable& var : variables) {
        if (assigment.count(var.idx) == 0)
            unassigned_variable_order.push_back(var.idx);
    }
    if ((variable_order_strategy == "") || (variable_order_strategy == "given_order")) {
        return unassigned_variable_order[0];
    }
    else if (variable_order_strategy == "min_domain_size") {
        return min_domain_size_variable_order(unassigned_variable_order, domains_index);
    }
    else if (variable_order_strategy == "random") {
        return random_variable_order(unassigned_variable_order);
    }
    else if (variable_order_strategy == "most_constrained") {
        return most_constrained_variable_order(unassigned_variable_order);
    }
    else {
        std::stringstream ss;
        ss << "The variable order startegy " << variable_order_strategy << "is not supported. Supported strategies are [given_order, min_domain_size, random, most_constrained]";
        string error_message = ss.str();
        throw runtime_error(error_message);
    }
    return -1;
};

int CSP::min_domain_size_variable_order(vector<int>& variable_order, map<int, int> domains_index) {
    std::stable_sort(variable_order.begin(), variable_order.end(), [&domains_index](const int& idx_1, const int& idx_2) {
        return domains_index[idx_1] < domains_index[idx_2];
        });
    return variable_order[0];
}

int CSP::random_variable_order(vector<int>& variable_order) {
    std::random_device rd;
    std::default_random_engine rng(rd());
    std::shuffle(variable_order.begin(), variable_order.end(), rng);
    return variable_order[0];
}

int CSP::most_constrained_variable_order(vector<int>& variable_order) {
    // Count constraints per variable
    map<int, int> number_of_constraint_per_variable;
    for (auto& cons : constraints) {
        pair<int, int> cons_var_idx = cons->get_variable_indexs();
        number_of_constraint_per_variable[cons_var_idx.first]++;
        number_of_constraint_per_variable[cons_var_idx.second]++;
    }

    // Sort variables by constraint count in descending order
    std::stable_sort(variable_order.begin(), variable_order.end(), [&](const int& idx_1, const int& idx_2) {
        return number_of_constraint_per_variable[idx_1] > number_of_constraint_per_variable[idx_2];
        });

    return variable_order[0];
}

///////////////////////////
//      FC AND MAC       //
///////////////////////////

void CSP::forward_checking(const int& new_assigned_variable_idx, const int& assigned_value,
    map<int, int>& domains_index, const map<int, int>& current_assignment) {

    vector<Mother_Constraint*> linked_constraints = get_constraints_linked_to(new_assigned_variable_idx);
    for (auto& cons : linked_constraints) {
        // propagation
        bool has_propagated = cons->propagate(new_assigned_variable_idx, assigned_value, domains_index, current_assignment, variables);
    }
}

void CSP::leaf_AC_3(const int& new_assigned_variable_idx, const int& assigned_value, map<int, int>& domains_index,
    const map<int, int>& current_assignment) {
    vector<pair<int, int>> to_be_tested;
    to_be_tested.push_back(make_pair(new_assigned_variable_idx, assigned_value));
    while (to_be_tested.size() > 0) {
        bool has_propagated = false;
        int index_var_to_test = to_be_tested[0].first;
        int value_to_propagate = to_be_tested[0].second;
        to_be_tested.erase(to_be_tested.begin());
        vector<Mother_Constraint*> linked_constraints = get_constraints_linked_to(index_var_to_test);
        for (auto& cons : linked_constraints) {
            has_propagated = cons->propagate(index_var_to_test, value_to_propagate, domains_index, current_assignment, variables);
        }

        if (has_propagated) {
            if (has_propagated) {
                for (const auto& [var_idx, dom_index] : domains_index) {
                    if ((current_assignment.count(var_idx) == 0) && (dom_index == 1))
                        to_be_tested.push_back(make_pair(var_idx, variables[var_idx].domain[0]));
                }
            }
        }
    }
    return;
}

///////////////////////////
//      BACKTRACK        //
///////////////////////////

bool CSP::check_assignment_consistency(const map<int, int>& assignment, const int& variable_index, const int& value) {
    vector<Mother_Constraint*> linked_constraints = get_constraints_linked_to(variable_index);
    for (auto& cons : linked_constraints) {
        int other_variable_index = cons->get_other_variable_index_from_variable(variable_index);
        if (assignment.count(other_variable_index) == 0) continue;

        if (not cons->check_if_constraint_is_satisfied(variable_index, value, other_variable_index, assignment.at(other_variable_index)))
            return false;
    }
    return true;
}

bool CSP::backtrack_iterative(map<int, int>& assignment, bool activate_FC, bool activate_MAC, string variable_order_strategy) {
    stack<Backward_State> stack;
    // Initialization with smaller domain OR given order

    map<int, int> initial_domains_index;
    for (Variable var : variables)
        initial_domains_index[var.idx] = var.domain.size();
    int first_var_index = select_next_variable_index(variable_order_strategy, assignment, initial_domains_index);
    stack.push({ assignment, first_var_index, 0, initial_domains_index });

    while (!stack.empty()) {
        Backward_State& state = stack.top();
        map<int, int> current_assignment = state.assignment;
        int current_variable_idx = state.variable_index;
        int value_index = state.value_index;
        map<int, int> current_domains_index = state.domains_index;
        // If not more supported values in the domain, backtrack
        vector<int> supported_values_to_be_tried;
        for (int i = value_index; i < current_domains_index[current_variable_idx]; i++) {
            supported_values_to_be_tried.push_back(variables[current_variable_idx].domain[i]);
        }
        if (value_index >= current_domains_index[current_variable_idx]) {
            assert(supported_values_to_be_tried.size() == 0);
            stack.pop();
            continue;
        }

        number_of_nodes++;

        int value = variables[current_variable_idx].domain[value_index];
        state.value_index++;

        if (check_assignment_consistency(current_assignment, current_variable_idx, value)) {
            current_assignment[current_variable_idx] = value;

            if (activate_FC)
                forward_checking(current_variable_idx, value, current_domains_index, current_assignment);

            if (activate_MAC)
                leaf_AC_3(current_variable_idx, value, current_domains_index, current_assignment);

            if (is_complete(current_assignment)) {
                assignment = current_assignment;
                return true;
            }
            // Next branch
            int next_variable_index = select_next_variable_index(variable_order_strategy, current_assignment, current_domains_index);

            stack.push({ current_assignment, next_variable_index, 0, current_domains_index });
        }
    }
    return false;
}

///////////////////////////
//           AC          //
///////////////////////////

bool CSP::update_variable_domain(const int& var_1_index, const int& var_2_index, vector<Variable>& variables) {
    vector<int> to_be_removed;
    for (int val1 : variables[var_1_index].domain) {
        bool supported = false;

        for (int val2 : variables[var_2_index].domain) {
            for (auto& cons : constraints) {
                if (!(cons->var_is_in_constraint(var_1_index)) || !(cons->var_is_in_constraint(var_2_index)))
                    continue;
                if (cons->check_if_constraint_is_satisfied(var_1_index, val1, var_2_index, val2)) {
                    supported = true;
                    break;
                }
            }
        }
        if (!supported) to_be_removed.push_back(val1);
    }
    if (to_be_removed.size() == 0) return false;

    for (int val : to_be_removed)
        variables[var_1_index].domain.erase(std::remove(variables[var_1_index].domain.begin(), variables[var_1_index].domain.end(), val), variables[var_1_index].domain.end());
    return true;
}

bool CSP::AC_1() {
    bool term = false;
    while (not term) {
        term = true;
        for (auto& cons : constraints) {
            pair<int, int> cons_variables = cons->get_variable_indexs();
            if (update_variable_domain(cons_variables.first, cons_variables.second, variables)) {
                if (variables[cons_variables.first].domain.size() == 0) {
                    return false;
                }
                term = false;
            }
        }
    }
    return true;
}

bool CSP::root_AC_3() {
    vector<pair<int, int>> to_be_tested;
    for (auto& cons : constraints) {
        pair<int, int> cons_variables = cons->get_variable_indexs();
        to_be_tested.push_back(cons_variables);
        to_be_tested.push_back(make_pair(cons_variables.second, cons_variables.second));
    }

    while (to_be_tested.size() > 0) {
        // std::cout << "TBT size : " << to_be_tested.size() << std::endl;
        int var_1_index = to_be_tested[0].first;
        int var_2_index = to_be_tested[0].second;
        to_be_tested.erase(to_be_tested.begin());

        if (update_variable_domain(var_1_index, var_2_index, variables)) {
            // Si le domaine de var_1 est réduit à zéro, le problème est inconsistant
            if (variables[var_1_index].domain.size() == 0) {
                return false;
            }

            // On ajoute les contraintes concernées par la modification de var_1
            for (auto& cons : constraints) {
                if (not cons->var_is_in_constraint(var_1_index))
                    continue;
                pair<int, int> cons_var_idx = cons->get_variable_indexs();
                if ((var_1_index == cons_var_idx.first) && (var_2_index != cons_var_idx.second)) {
                    to_be_tested.push_back(make_pair(cons_var_idx.second, var_1_index));
                }
                else if ((var_1_index == cons_var_idx.second) && (var_2_index != cons_var_idx.first)) {
                    to_be_tested.push_back(make_pair(cons_var_idx.first, var_1_index));
                }
            }
        }
    }
    return true;
}

///////////////////////////
//         SOLVE         //
///////////////////////////

tuple<map<int, int>, int, double> CSP::solve(std::vector<Mother_Constraint*>& constraints, vector<Variable>& variables,
    bool activate_AC1, bool activate_AC3, bool activate_FC, bool activate_MAC, string value_order_strategy, string variable_order_startegy) {

    if (activate_FC && activate_MAC) {
        throw runtime_error("Impossible to use FC and MAC at the same time.");
    }
    map<int, int> assignment;
    cout << "Domain sizes before AC_3: [";

    for (Variable var : variables) {
        cout << var.domain.size() << ", ";
    }
    cout << "]" << endl;

    clock_t start = clock();
    bool csp_is_consistent = true;

    if (activate_AC3)
        csp_is_consistent = root_AC_3();

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

    bool sol_found = backtrack_iterative(assignment, activate_FC, activate_MAC, variable_order_startegy);
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
