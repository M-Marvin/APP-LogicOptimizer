/*
 * qmcp.hpp
 *
 *  Created on: 18.09.2025
 *      Author: Marvin K. (M_Marvin)
 */

#ifndef SRC_CPP_HEADER_QMCP_HPP_
#define SRC_CPP_HEADER_QMCP_HPP_

#include <vector>
#include <set>
#include <map>
#include <optional>
#include "kvm.hpp"

class QMCImplicant {

private:
	std::vector<TriStateBool> states;
	std::vector<unsigned int> minterms;
	bool prime = true;

public:
	void initialize(KVMap& map, unsigned int column, unsigned int row);
	std::optional<QMCImplicant> tryMerge(const QMCImplicant& implicant);
	const std::vector<TriStateBool>& variableStates() const;
	const std::vector<unsigned int>& mintermSet() const;
	unsigned int variableCount() const;
	unsigned int inputsTrueCount() const;
	unsigned int relevantInputCount() const;
	void markPrime(bool prime);
	bool isPrime() const;

	bool operator==(const QMCImplicant& other) const;
	bool operator!=(const QMCImplicant& other) const;

};

class QMCImplicantSet {

private:
	std::vector<QMCImplicant> implicants;

public:
	void add(const QMCImplicant& implicant);
	bool tryMerge(QMCImplicantSet& implicantSet, std::vector<QMCImplicantSet>& targetVector);
	unsigned int variableCount() const;
	const std::vector<QMCImplicant>& implicantSet() const;

};

typedef std::optional<std::reference_wrapper<const QMCImplicantSet>> QMCImplicantSetOpt;

class QMCStack {

private:
	std::vector<unsigned int> minterms;
	std::vector<std::vector<QMCImplicantSet>> stages;

public:
	void initialize(KVMap& map);
	bool tryMerge();
	QMCImplicantSetOpt implicantSetFor(unsigned int stage, unsigned int numberOfOnes) const;
	unsigned int groupImplicantCount(unsigned int numberOfOnes) const;
	unsigned int variableCount() const;
	unsigned int stageCount() const;

};

class QMCPrimeChart {

private:
	std::vector<QMCImplicant> primes;
	std::vector<unsigned int> minterms;

public:
	void initialize(const QMCStack& stack);
	void extractEPIs(std::vector<QMCImplicant>& essentialPrimes);
	void findOptimalPrimes(std::vector<QMCImplicant>& optimalPrimes) const;
	const std::vector<QMCImplicant>& primeImplicants() const;
	const std::vector<unsigned int>& mintermIds() const;
	unsigned int variableCount() const;

};

#endif /* SRC_CPP_HEADER_QMCP_HPP_ */
