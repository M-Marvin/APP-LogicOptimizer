/*
 * kvm.hpp
 *
 *  Created on: 18.09.2025
 *      Author: Marvin K. (M_Marvin)
 */

#ifndef SRC_CPP_HEADER_KVM_HPP_
#define SRC_CPP_HEADER_KVM_HPP_

#include <vector>
#include "truthtable.hpp"

class KVMap {

private:
	unsigned int variables;
	unsigned int width;
	unsigned int height;
	std::vector<TriStateBool> data;

public:
	KVMap(const TruthTable& table, unsigned int output);

	unsigned int mapWidth() const;
	unsigned int mapHeight() const;
	unsigned int variableCount() const;
	TriStateBool valueAt(unsigned int column, unsigned int row) const;
	TriStateBool inputAt(unsigned int column, unsigned int row, unsigned int input) const;

};



#endif /* SRC_CPP_HEADER_KVM_HPP_ */
