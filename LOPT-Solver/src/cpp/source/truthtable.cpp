/*
 * truthtable.cpp
 *
 * An implementation of an truth table.
 *
 * The state of individual inputs can be set and read using the input() function which returns a pointer to the value stored in the table.
 * The state of individual outputs can be set and read using the output() function which returns a pointer to the value stored in the table.
 * The state index of an combination of all inputs can be queried using the find() function.
 *
 *  Created on: 18.09.2025
 *      Author: Marvin K. (M_Marvin)
 */

#include "truthtable.hpp"

TruthTable::TruthTable(std::vector<TriStateBool>& tableData, unsigned int inputs, unsigned int outputs)
{
	this->inputs = inputs;
	this->outputs = outputs;
	this->data = std::vector<TriStateBool>(tableData);
}

TriStateBool TruthTable::input(unsigned int state, unsigned int input) const
{
	if (state >= stateCount()) return TriStateBool::DONT_CARE;
	if (input >= this->inputs) return TriStateBool::DONT_CARE;
	return this->data[(state * (this->inputs + this->outputs)) + input];
}

TriStateBool TruthTable::output(unsigned int state, unsigned int output) const
{
	if (state >= stateCount()) return TriStateBool::DONT_CARE;
	if (output >= this->outputs) return TriStateBool::DONT_CARE;
	return this->data[(state * (this->inputs + this->outputs)) + this->inputs + output];
}

unsigned int TruthTable::find(TriStateBool* inputs) const
{

	for (unsigned int s = 0; s < stateCount(); s++)
	{
		for (unsigned int i = 0; i < this->inputs; i++)
		{
			TriStateBool in = input(s, i);
			if (in != inputs[i] && in != TriStateBool::DONT_CARE) goto skip;
		}
		return s;
		skip:
	}

	return stateCount(); // this means it is not defined, aka dont't care
}

unsigned int TruthTable::inputCount() const
{
	return this->inputs;
}

unsigned int TruthTable::outputCount() const
{
	return this->outputs;
}

unsigned int TruthTable::width() const
{
	return this->inputs + this->outputs;
}

unsigned int TruthTable::stateCount() const
{
	return this->data.size() / width();
}
