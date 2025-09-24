/*
 * kvm.cpp
 *
 * An implementation of an KV-Map.
 *
 * Mainly used to visually represent the function which is now analyzed.
 *
 *  Created on: 18.09.2025
 *      Author: Marvin K. (M_Marvin)
 */

#include <cmath>
#include "kvm.hpp"

#include <stdio.h>

bool kvm_cell_var(unsigned int var, unsigned int column, unsigned int row) {

	/**
	 * - area on axis where variable is 0
	 * = area on axis where variable is 1
	 * --/== length of 1/0 segments (i1)
	 * ~ half segment widh shift to use modulo operator
	 * | index of the cell (xcell)
	 *
	 * ~-==--==
	 *    ~|
	 *
	 * ~~
	 * ----====----====
	 *        ~~|
	 */

	if (var % 2 == 0) {
		unsigned int xvar = var / 2;
		unsigned int xcell = column;
		unsigned int i1 = std::pow(2, xvar + 1);
		unsigned int i2 = (xcell + i1 / 2) % (i1 * 2);
		return i2 >= i1;
	} else {
		unsigned int yvar = var / 2;
		unsigned int ycell = row;
		unsigned int i1 = std::pow(2, yvar + 1);
		unsigned int i2 = (ycell + i1 / 2) % (i1 * 2);
		return i2 >= i1;
	}

}

KVMap::KVMap(const TruthTable& table, unsigned int output)
{
	this->variables = table.inputCount();
	unsigned int ncell = std::pow(2, this->variables);
	this->width = this->variables % 2 ? std::sqrt(std::pow(2, this->variables + 1)) : std::sqrt(ncell);
	this->height = ncell / this->width;

	this->data.clear();
	this->data.resize(ncell);
	TriStateBool vars[this->variables];

	for (unsigned int column = 0; column < this->width; column++)
		for (unsigned int row = 0; row < this->height; row++)
		{
			for (unsigned int var = 0; var < this->variables; var++) {
				vars[var] = kvm_cell_var(var, column, row) ? TriStateBool::TRUE : TriStateBool::FALSE;
			}
			unsigned int stateIndx = table.find(vars);
			this->data[row * this->width + column] = stateIndx == table.stateCount() ? TriStateBool::DONT_CARE : table.output(stateIndx, output);
		}

}

unsigned int KVMap::mapWidth() const
{
	return this->width;
}

unsigned int KVMap::mapHeight() const
{
	return this->height;
}

unsigned int KVMap::variableCount() const
{
	return this->variables;
}

TriStateBool KVMap::valueAt(unsigned int column, unsigned int row) const
{
	if (column >= this->width) return TriStateBool::DONT_CARE;
	if (row >= this->height) return TriStateBool::DONT_CARE;
	return this->data[row * this->width + column];
}

TriStateBool KVMap::inputAt(unsigned int column, unsigned int row, unsigned int input) const
{
	if (column >= this->width) return TriStateBool::DONT_CARE;
	if (row >= this->height) return TriStateBool::DONT_CARE;
	return kvm_cell_var(input, column, row) ? TriStateBool::TRUE : TriStateBool::FALSE;
}
