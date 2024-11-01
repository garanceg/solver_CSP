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
#include <unordered_map>


CSP::CSP(vector<Constraint*>& constraints, vector<Variable>& variables) : constraints(constraints), variables(variables) {};

///////////////////////////
//     UTILS FUNCTONS    //
///////////////////////////

vector<Constraint*> CSP::get_constraints_linked_to(const int& variable_index) {
    vector<Constraint*> linked_constraints;
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
        pair<int, int> cons_var_idx = cons->get_variable_indexes();
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
//  Value order strat //
///////////////////////////

void CSP::sort_values(string value_order_strategy, Variable& variable, int k) {
    if ((value_order_strategy == "") || (value_order_strategy == "given_order")) {
        return;
    }
    else if (value_order_strategy == "min_value") {
        return min_value_order(variable, k);
    }
    else if (value_order_strategy == "random_value") {
        return random_value_order(variable, k);
    }
    else if (value_order_strategy == "most_supported_value") {
        return most_supported_value_order(variable, k);
    }
    else {
        std::stringstream ss;
        ss << "The value order startegy " << value_order_strategy << "is not supported. Supported strategies are [given_order, min_value, random_value, most_supported_value]";
        string error_message = ss.str();
        throw runtime_error(error_message);
    }
};


void CSP::min_value_order(Variable& current_variable, int k) {
    std::vector<int> first_elements(current_variable.domain.begin(), current_variable.domain.begin() + k);
    std::sort(first_elements.begin(), first_elements.end());
    std::copy(first_elements.begin(), first_elements.end(), current_variable.domain.begin());
}


void CSP::random_value_order(Variable& current_variable, int k) {
    std::random_device rd;
    std::default_random_engine rng(rd());
    std::vector<int> first_elements(current_variable.domain.begin(), current_variable.domain.begin() + k);
    std::shuffle(first_elements.begin(), first_elements.end(), rng);
    std::copy(first_elements.begin(), first_elements.end(), current_variable.domain.begin());
}


void CSP::most_supported_value_order(Variable& current_variable, int k) {
    std::unordered_map<int, int> support_count;

    for (const auto& cons : constraints) {
        if (cons->var_is_in_constraint(current_variable.idx)) {
            int var_idx = cons->get_var_index_in_constraint(current_variable.idx);
            if (var_idx == 0) {
                for (const auto& t : cons->get_cons_tuples()) {
                    int value = std::get<0>(t);
                    support_count[value]++;
                }
            }
            else if (var_idx == 1) {
                for (const auto& t : cons->get_cons_tuples()) {
                    int value = std::get<1>(t);
                    support_count[value]++;
                }
            }
        }
    }

    std::vector<int> first_elements(current_variable.domain.begin(), current_variable.domain.begin() + k);

    std::vector<std::pair<int, int>> value_support_pairs;
    for (int value : first_elements) {
        value_support_pairs.emplace_back(value, support_count[value]);
    }

    std::sort(value_support_pairs.begin(), value_support_pairs.end(),
        [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
            return a.second > b.second;
        });

    first_elements.clear();
    for (const auto& pair : value_support_pairs) {
        first_elements.push_back(pair.first);
    }
    std::copy(first_elements.begin(), first_elements.end(), current_variable.domain.begin());
}


///////////////////////////
//      FC AND MAC       //
///////////////////////////

void CSP::forward_checking(const int& new_assigned_variable_idx, const int& assigned_value,
    map<int, int>& domains_index, const map<int, int>& current_assignment) {

    vector<Constraint*> linked_constraints = get_constraints_linked_to(new_assigned_variable_idx);
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
        vector<Constraint*> linked_constraints = get_constraints_linked_to(index_var_to_test);
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
    vector<Constraint*> linked_constraints = get_constraints_linked_to(variable_index);
    for (auto& cons : linked_constraints) {
        int other_variable_index = cons->get_other_variable_index_from_variable(variable_index);
        if (assignment.count(other_variable_index) == 0) continue;

        if (not cons->check_if_constraint_is_satisfied(variable_index, value, other_variable_index, assignment.at(other_variable_index)))
            return false;
    }
    return true;
}

bool CSP::backtrack_iterative(map<int, int>& assignment, bool activate_FC, bool activate_MAC, string value_order_strategy,
    string variable_order_strategy, std::chrono::high_resolution_clock::time_point start, double max_duration) {

    stack<Backward_State> stack;
    // Initialization with smaller domain OR given order

    map<int, int> initial_domains_index;
    for (Variable var : variables)
        initial_domains_index[var.idx] = var.domain.size();
    int first_var_index = select_next_variable_index(variable_order_strategy, assignment, initial_domains_index);
    stack.push({ assignment, first_var_index, 0, initial_domains_index });

    while (!stack.empty()) {
        auto now = std::chrono::high_resolution_clock::now();
        double elapsed_time = std::chrono::duration_cast<std::chrono::duration<double>>(now - start).count();
        if (elapsed_time >= max_duration) {
            std::cout << "Backtracking interrupted after " << elapsed_time << " seconds." << std::endl;
            return false;
        }

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

        sort_values(value_order_strategy, variables[current_variable_idx], current_domains_index[current_variable_idx]);

        int value = variables[current_variable_idx].domain[value_index];
        state.value_index++;

        if (check_assignment_consistency(current_assignment, current_variable_idx, value)) {
            current_assignment[current_variable_idx] = value;

            if (activate_FC) {
                forward_checking(current_variable_idx, value, current_domains_index, current_assignment);
            }
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
    vector<Constraint*> linked_cons = get_constraints_linked_to(var_1_index);
    for (int val1 : variables[var_1_index].domain) {
        bool supported = false;
        for (auto& cons : linked_cons) {
            if (cons->var_is_in_constraint(var_2_index)) {
                for (int val2 : variables[var_2_index].domain) {
                    if (cons->check_if_constraint_is_satisfied(var_1_index, val1, var_2_index, val2)) {
                        supported = true;
                        break;
                    }
                }
            }
        }
        if (!supported) {
            to_be_removed.push_back(val1);
        }
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
            pair<int, int> cons_variables = cons->get_variable_indexes();
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

bool CSP::root_AC_3(map<int, int>& assigment) {
    vector<pair<int, int>> to_be_tested;
    for (auto& cons : constraints) {
        pair<int, int> cons_variables = cons->get_variable_indexes();
        if (variables[cons_variables.first].domain.size() != 1) {
            assert(assigment.count(cons_variables.first) == 0);
            to_be_tested.push_back(cons_variables);
        }
        if (variables[cons_variables.second].domain.size() != 1) {
            assert(assigment.count(cons_variables.second) == 0);
            to_be_tested.push_back(make_pair(cons_variables.second, cons_variables.second));
        }
    }

    while (to_be_tested.size() > 0) {
        int var_1_index = to_be_tested[0].first;
        int var_2_index = to_be_tested[0].second;
        to_be_tested.erase(to_be_tested.begin());
        if (update_variable_domain(var_1_index, var_2_index, variables)) {
            // Si le domaine de var_1 est réduit à zéro, le problème est inconsistant
            if (variables[var_1_index].domain.size() == 0) {
                return false;
            }

            else if (variables[var_1_index].domain.size() == 1)
                assigment[var_1_index] = variables[var_1_index].domain[0];
            // On ajoute les contraintes concernées par la modification de var_1
            for (auto& cons : constraints) {
                if (not cons->var_is_in_constraint(var_1_index))
                    continue;
                pair<int, int> cons_var_idx = cons->get_variable_indexes();
                if ((var_1_index == cons_var_idx.first) && (var_2_index != cons_var_idx.second) && (variables[cons_var_idx.second].domain.size() != 1)) {
                    to_be_tested.push_back(make_pair(cons_var_idx.second, var_1_index));
                }
                else if ((var_1_index == cons_var_idx.second) && (var_2_index != cons_var_idx.first) && (variables[cons_var_idx.first].domain.size() != 1)) {
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

tuple<map<int, int>, int, double> CSP::solve(map<int, int>& assignment,
    bool activate_AC1, bool activate_AC3, bool activate_FC, bool activate_MAC, string value_order_strategy, string variable_order_startegy, double max_duration) {

    if (activate_FC && activate_MAC) {
        throw runtime_error("Impossible to use FC and MAC at the same time.");
    }

    auto start = std::chrono::high_resolution_clock::now();
    bool csp_is_consistent = true;

    if (activate_AC3)
        csp_is_consistent = root_AC_3(assignment);

    auto end_consistence = std::chrono::high_resolution_clock::now();
    double time_consistence = std::chrono::duration_cast<std::chrono::duration<double>>(end_consistence - start).count();

    if (!csp_is_consistent) {
        cout << "CSP is inconsistent because of these variables: ";
        for (Variable var : variables) {
            if (var.domain.size() == 0)
                cout << var.name;
        }
        cout << " " << endl;
        return make_tuple(assignment, 0, 0);
    }

    bool sol_found = backtrack_iterative(assignment, activate_FC, activate_MAC, value_order_strategy, variable_order_startegy, start, max_duration);
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();

    cout << "Visited " << number_of_nodes << " nodes." << endl;
    if (sol_found) {
        cout << "Solution found in " << elapsed << " seconds." << endl;
        return make_tuple(assignment, number_of_nodes, elapsed);
    }
    cout << "No solution found in " << elapsed << " seconds." << endl;
    cout << "No solution found" << endl;
    return make_tuple(assignment, number_of_nodes, -1);
}
