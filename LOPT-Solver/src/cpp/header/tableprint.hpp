/*
 * tableprint.hpp
 *
 * Utility functions for printing the garphical representations of the different objects.
 *
 *  Created on: 18.09.2025
 *      Author: Marvin K. (M_Marvin)
 */

#ifndef SRC_CPP_HEADER_TABLEPRINT_HPP_
#define SRC_CPP_HEADER_TABLEPRINT_HPP_

#include "truthtable.hpp"
#include "kvm.hpp"
#include "qmcp.hpp"

void print_truthtable(const TruthTable& table);
void print_kvmap(const KVMap& map);
void print_qmcstack(const QMCStack& stack);
void print_qmcchart(const QMCPrimeChart& chart);
void print_bool_term(const std::vector<QMCImplicant>& term);
void print_result_table(const std::vector<std::vector<QMCImplicant>>& finalTerms);

#endif /* SRC_CPP_HEADER_TABLEPRINT_HPP_ */
