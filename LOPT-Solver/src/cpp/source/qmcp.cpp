/*
 * qmcp.cpp
 *
 * Implementation of the "Quine–McCluskey" algorithm to find the minal boolean equations for the truth table.
 * This is the code that does the actual processing.
 *
 * It also uses an modified version of the "Petrick's method" algorithm for the final solution of the prime chart.
 * The modification mainly is in the way the equations are processed.
 *
 *  Created on: 18.09.2025
 *      Author: Marvin K. (M_Marvin)
 */

#include <cstring>
#include <algorithm>
#include <set>
#include "qmcp.hpp"

/** QMC Implicant **/

void QMCImplicant::initialize(KVMap& map, unsigned int column, unsigned int row)
{
	for (unsigned int i = 0; i < map.variableCount(); i++)
	{
		this->states.push_back(map.inputAt(column, row, i));
	}
	// generate id for this minterm based on position in KV-map
	// note: we want to skip ids for DONT CARE terms, to make keep them out of the final prime chart
	if (map.valueAt(column, row) != TriStateBool::DONT_CARE)
		this->minterms.push_back(row * map.mapWidth() + column);
}

std::optional<QMCImplicant> QMCImplicant::tryMerge(const QMCImplicant& implicant)
{
	if (implicant.states.size() != this->states.size()) return std::nullopt;

	// test if the two implicants can be merged, this is the case if they only differ in one digit
	unsigned int mergeIndex = this->states.size();
	for (unsigned int i = 0; i < this->states.size(); i++)
	{
		if (this->states[i] == TriStateBool::DONT_CARE ^ implicant.states[i] == TriStateBool::DONT_CARE) return std::nullopt;
		if (this->states[i] != implicant.states[i])
		{
			if (mergeIndex != this->states.size()) return std::nullopt;
			mergeIndex = i;
		}
	}

	// merge the two implicants into a new one, set the one digit that differs to DONT_CARE
	QMCImplicant merged = *this;
	merged.states[mergeIndex] = TriStateBool::DONT_CARE;
	for (auto m : implicant.minterms)
		if (std::find(merged.minterms.begin(), merged.minterms.end(), m) == merged.minterms.end())
			merged.minterms.push_back(m);
	return merged;
}

const std::vector<TriStateBool>& QMCImplicant::variableStates() const
{
	return this->states;
}

const std::vector<unsigned int>& QMCImplicant::mintermSet() const
{
	return this->minterms;
}

void QMCImplicant::markPrime(bool prime)
{
	this->prime = prime;
}

bool QMCImplicant::isPrime() const
{
	return this->prime;
}

bool QMCImplicant::operator==(const QMCImplicant& other) const
{
	return this->states == other.states;
}
bool QMCImplicant::operator!=(const QMCImplicant& other) const
{
	return this->states != other.states;
}

unsigned int QMCImplicant::inputsTrueCount() const
{
	unsigned int numberOfOnes = 0;
	for (TriStateBool s : this->states)
		if (s == TriStateBool::TRUE) numberOfOnes++;
	return numberOfOnes;
}

unsigned int QMCImplicant::relevantInputCount() const
{
	unsigned int relevantInputs = 0;
	for (TriStateBool s : this->states)
		if (s != TriStateBool::DONT_CARE) relevantInputs++;
	return relevantInputs;
}

unsigned int QMCImplicant::variableCount() const
{
	return this->states.size();
}

/** QMC Implicant Set **/

void QMCImplicantSet::add(const QMCImplicant& implicant)
{
	if (std::find(this->implicants.begin(), this->implicants.end(), implicant) != this->implicants.end())
		return;
	this->implicants.push_back(implicant);
}

bool QMCImplicantSet::tryMerge(QMCImplicantSet& implicantSet, std::vector<QMCImplicantSet>& targetVector)
{
	// try to merge all implicants within this set with each other and fill the target vector with the resulting sets
	bool hasMerged = false;
	for (QMCImplicant& im1 : this->implicants)
	{
		for (QMCImplicant& im2 : implicantSet.implicants)
		{
			std::optional<QMCImplicant> merged = im1.tryMerge(im2);
			if (merged.has_value())
			{
				// by default every implicant is marked as prime, this will be removed here if it was merged at least once
				im1.markPrime(false);
				im2.markPrime(false);
				merged->markPrime(true);

				unsigned int numberOfOnes = merged->inputsTrueCount();
				targetVector.at(numberOfOnes).add(merged.value());
				hasMerged = true;
			}
		}
	}
	return hasMerged;
}

const std::vector<QMCImplicant>& QMCImplicantSet::implicantSet() const
{
	return this->implicants;
}

unsigned int QMCImplicantSet::variableCount() const
{
	if (this->implicants.empty()) return 0;
	return this->implicants.begin()->variableCount();
}

/** QMC Stack **/

void QMCStack::initialize(KVMap& map)
{

	this->stages.clear();
	std::vector<QMCImplicantSet>& implicantSets = this->stages.emplace_back();;

	for (unsigned int i = 0; i <= map.variableCount(); i++)
		implicantSets.emplace_back();

	for (unsigned int row = 0; row < map.mapHeight(); row++)
		for (unsigned int column = 0; column < map.mapWidth(); column++)
		{

			if (map.valueAt(column, row) == TriStateBool::FALSE) continue;

			QMCImplicant implicant;
			implicant.initialize(map, column, row);
			unsigned int numberOfOnes = implicant.inputsTrueCount();

			implicantSets.at(numberOfOnes).add(implicant);

		}

}

bool QMCStack::tryMerge()
{
	unsigned int nextStage = this->stages.size();

	// create new stage vector
	std::vector<QMCImplicantSet>& stage2Set = this->stages.emplace_back();
	std::vector<QMCImplicantSet>& stage1Set = this->stages.at(this->stages.size() - 2);

	for (unsigned int i = 0; i < stage1Set.size(); i++) {
		stage2Set.emplace_back();
	}

	// try to merge all sets in the current stage with each other, place resulting sets into new stage
	bool hasMerged = false;
	for (unsigned int i = 0; i < stage1Set.size() - 1; i++)
	{
		if (stage1Set.at(i).tryMerge(stage1Set.at(i + 1), stage2Set))
			hasMerged = true;
	}
	return hasMerged;
}

unsigned int QMCStack::groupImplicantCount(unsigned int numberOfOnes) const
{
	unsigned int n = 0;
	for (auto g = this->stages.begin(); g != this->stages.end(); g++)
	{
		unsigned int n1 = g->at(numberOfOnes).implicantSet().size();
		if (n1 > n) n = n1;
	}
	return n;
}

unsigned int QMCStack::variableCount() const
{
	if (this->stages.empty()) return 0;
	for (auto s : *this->stages.begin())
		if (s.variableCount() > 0) return s.variableCount();
	return 0;
}

unsigned int QMCStack::stageCount() const
{
	return this->stages.size();
}

std::optional<std::reference_wrapper<const QMCImplicantSet>> QMCStack::implicantSetFor(unsigned int stage, unsigned int numberOfOnes) const
{
	if (stage >= this->stages.size()) return std::nullopt;
	if (numberOfOnes > variableCount()) return std::nullopt;
	return std::ref(this->stages.at(stage).at(numberOfOnes));
}

/** QMC Prime Chart **/

void QMCPrimeChart::initialize(const QMCStack& stack)
{
	for (unsigned int stage = 0; stage < stack.stageCount(); stage++)
		for (unsigned int numberOfOnes = 0; numberOfOnes < stack.variableCount(); numberOfOnes++)
		{
			QMCImplicantSetOpt implicants = stack.implicantSetFor(stage, numberOfOnes);
			for (auto implicant = implicants->get().implicantSet().begin(); implicant != implicants->get().implicantSet().end(); implicant++)
				if (implicant->isPrime() && !implicant->mintermSet().empty())
				{
					this->primes.push_back(*implicant);
					for (auto mintermId = implicant->mintermSet().begin(); mintermId != implicant->mintermSet().end(); mintermId++)
						if (std::find(this->minterms.begin(), this->minterms.end(), *mintermId) == this->minterms.end())
							this->minterms.push_back(*mintermId);
				}
		}
}

void QMCPrimeChart::extractEPIs(std::vector<QMCImplicant>& essentialPrimes)
{
	// find all essential primes, these are the ones which are the only ones which fulfill certain minterms in the table
	for (auto minterm = this->minterms.begin(); minterm != this->minterms.end(); minterm++)
	{
		std::vector<QMCImplicant>::iterator epi = this->primes.end();
		for (auto primeImplicant = this->primes.begin(); primeImplicant != this->primes.end(); primeImplicant++) {
			if (std::find(primeImplicant->mintermSet().begin(), primeImplicant->mintermSet().end(), *minterm) != primeImplicant->mintermSet().end())
				if (epi != this->primes.end())
					goto not_essential;
				else
					epi = primeImplicant;
		}

		if (epi != this->primes.end() && std::find(essentialPrimes.begin(), essentialPrimes.end(), *epi) == essentialPrimes.end())
			essentialPrimes.push_back(*epi);
		not_essential:
	}

	// remove essential primes from the chart
	this->primes.erase(std::remove_if(this->primes.begin(), this->primes.end(), [essentialPrimes](const QMCImplicant& pi){
		return std::find(essentialPrimes.begin(), essentialPrimes.end(), pi) != essentialPrimes.end();
	}), this->primes.end());

	// remove all minterms fulfilled by the primes
	this->minterms.erase(std::remove_if(this->minterms.begin(), this->minterms.end(), [essentialPrimes](unsigned int m){
		for (const QMCImplicant& epi : essentialPrimes)
			if (std::find(epi.mintermSet().begin(), epi.mintermSet().end(), m) != epi.mintermSet().end())
				return true;
		return false;
	}), this->minterms.end());
}

typedef std::set<const QMCImplicant*> term_t;
typedef std::vector<term_t> bracket_t;

bracket_t mergeBrackets(const std::vector<bracket_t>& brackets)
{

	// copy first bracket to merge with others
	bracket_t merged = *brackets.begin();

	// calculate how many entries this merged bracket will have
	size_t totalMergedTerms = 1;
	for (bracket_t bracket : brackets)
		totalMergedTerms *= bracket.size();

	// reserve space for all terms to avoid pointer invalidation
	merged.reserve(totalMergedTerms);

	// merge with all following brackets
	for (auto bracket = brackets.begin() + 1; bracket != brackets.end(); bracket++)
	{

		// merge terms of "merged" bracket with ...
		for (auto term1 = merged.begin(); term1 != merged.end(); term1++) {
			term_t term1tmp = *term1;

			// ... all terms of other bracket
			for (auto term2 = bracket->begin(); term2 != bracket->end(); term2++)
			{

				term1->insert(term2->begin(), term2->end());

				if (term2 + 1 != bracket->end())
					term1 = merged.insert(term1 + 1, term1tmp);
			}
		}
	}

	return merged;

}

void eliminateSupersets(bracket_t& bracket)
{
	// this essentially applies the absorption law to the supplied terms
	for (auto term1 = bracket.begin(); term1 != bracket.end() - 1;)
	{
		for (auto term2 = term1 + 1; term2 != bracket.end();)
		{
			if (std::includes(term1->begin(), term1->end(), term2->begin(), term2->end()))
			{
				// remove term1
				term1 = bracket.erase(term1);
				goto break_skip_increment;
			}
			else if (std::includes(term2->begin(), term2->end(), term1->begin(), term1->end()))
			{
				// remove term2
				term2 = bracket.erase(term2);
			}
			else
				term2++;
		}
		term1++;
		break_skip_increment:
	}
}

void QMCPrimeChart::findOptimalPrimes(std::vector<QMCImplicant>& optimalPrimes) const
{

	// create an product of sums, where each sum is true if its corresponding column is true (if one of its minterms is true)
	std::vector<bracket_t> productOfSums;
	for (unsigned int minterm : this->minterms)
	{
		productOfSums.push_back({});

		unsigned int id = 0;
		for (const QMCImplicant& implicant : this->primes)
		{
			if (std::find(implicant.mintermSet().begin(), implicant.mintermSet().end(), minterm) != implicant.mintermSet().end())
			{
				productOfSums.back().push_back({});
				productOfSums.back().back().insert(&implicant);
			}
			id++;
		}
	}

	// merge all sums into one to create an sum of products
	bracket_t sumOfProducts = mergeBrackets(productOfSums);

	// sort and remove duplicate terms and apply absorption law
	std::sort(sumOfProducts.begin(), sumOfProducts.end());
	sumOfProducts.erase(std::unique(sumOfProducts.begin(), sumOfProducts.end()), sumOfProducts.end());
	eliminateSupersets(sumOfProducts);

	// find shortest product in the sum of products
	unsigned int tlen = 0;
	term_t* shortestTerm = 0;
	for (term_t& term : sumOfProducts)
	{
		unsigned int l = 0;
		for (const QMCImplicant* implicant : term)
			l += implicant->relevantInputCount();
		if (l < tlen || tlen == 0)
		{
			tlen = l;
			shortestTerm = &term;
		}
	}

	// copy all primes contained within the shortest product to the output vector
	for (const QMCImplicant* impl : *shortestTerm)
		optimalPrimes.push_back(*impl);

}

const std::vector<QMCImplicant>& QMCPrimeChart::primeImplicants() const
{
	return this->primes;
}

const std::vector<unsigned int>& QMCPrimeChart::mintermIds() const
{
	return this->minterms;
}

unsigned int QMCPrimeChart::variableCount() const
{
	if (this->primes.empty()) return 0;
	return this->primes.begin()->variableCount();
}
