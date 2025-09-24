/*
 * truthtable.hpp
 *
 *  Created on: 18.09.2025
 *      Author: Marvin K. (M_Marvin)
 */

#ifndef SRC_CPP_HEADER_TRUTHTABLE_HPP_
#define SRC_CPP_HEADER_TRUTHTABLE_HPP_

#include <vector>

enum TriStateBool {
	FALSE = 0,
	TRUE = 1,
	DONT_CARE = 2
};

class TruthTable {

private:
	unsigned int inputs;
	unsigned int outputs;
	std::vector<TriStateBool> data;

public:
	TruthTable(std::vector<TriStateBool>& tableData, unsigned int inputs, unsigned int outputs);

	unsigned int find(TriStateBool* inputs) const;
	TriStateBool input(unsigned int state, unsigned int input) const;
	TriStateBool output(unsigned int state, unsigned int output) const;

	unsigned int inputCount() const;
	unsigned int outputCount() const;
	unsigned int width() const;
	unsigned int stateCount() const;

	TriStateBool* dataPtr() const;

};



#endif /* SRC_CPP_HEADER_TRUTHTABLE_HPP_ */
