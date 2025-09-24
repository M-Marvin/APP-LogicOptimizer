/*
 * tableprint.cpp
 *
 * Utility functions for printing the garphical representations of the different objects.
 *
 *  Created on: 18.09.2025
 *      Author: Marvin K. (M_Marvin)
 */

#include <stdio.h>
#include <cmath>
#include <algorithm>
#include "frameprint.hpp"
#include "tableprint.hpp"

void print_truthtable(const TruthTable& table)
{

	unsigned int width = std::max(table.width() * 3 + 3, 60U);
	unsigned int height = table.stateCount();

	// print title
	print_frame_top(width);
	print_frame_side(width);
	pos_print(1, 2);
	color_print_f(0, 200, 0); wprintf(L"Truth-Table [inputs: %u, outputs: %u, defined states: %u]\n", table.inputCount(), table.outputCount(), table.stateCount()); reset_print();
	print_frame_div(width);

	// print table column labels
	print_frame_side(width);
	pos_print(1, 1);
	color_print_f(255, 0, 0);
	for (unsigned int i = 0; i < table.inputCount(); i++)
		wprintf(L" %c ", 65 + i);
	wprintf(L" ");
	color_print_f(0, 255, 0);
	for (unsigned int o = 0; o < table.outputCount(); o++)
		wprintf(L" %u ", o);
	reset_print();
	wprintf(L"\n");

	// print table entries
	for (unsigned int s = 0; s < height; s++)
	{
		print_frame_side(width);
		pos_print(1, 2);

		// inputs
		for (unsigned int i = 0; i < table.inputCount(); i++)
		{
			switch (table.input(s, i)) {
			case TRUE:
				color_print_b(0, 255, 0);
				color_print_f(0, 120, 0);
				wprintf(L" 1 ");
				break;
			case FALSE:
				color_print_b(255, 0, 0);
				color_print_f(120, 0, 0);
				wprintf(L" 0 ");
				break;
			case DONT_CARE:
				color_print_b(128, 128, 128);
				color_print_f(80, 80, 80);
				wprintf(L" X ");
				break;
			}
		}

		reset_print(); wprintf(L" ");

		// outputs
		for (unsigned int o = 0; o < table.outputCount(); o++)
		{
			switch (table.output(s, o)) {
			case TRUE:
				color_print_b(0, 255, 0);
				color_print_f(0, 120, 0);
				wprintf(L" 1 ");
				break;
			case FALSE:
				color_print_b(255, 0, 0);
				color_print_f(120, 0, 0);
				wprintf(L" 0 ");
				break;
			case DONT_CARE:
				color_print_b(128, 128, 128);
				color_print_f(80, 80, 80);
				wprintf(L" X ");
				break;
			}
		}

		reset_print(); wprintf(L"\n");
	}

	print_frame_bottom(width);

}

void print_kvmap(const KVMap& map)
{

	unsigned int mwidth = map.mapWidth();
	unsigned int mheight = map.mapHeight();
	unsigned int variables = map.variableCount();

	unsigned int width = std::max((mwidth + variables / 2) * 3 + 2, 35U);

	// print title
	print_frame_top(width);
	print_frame_side(width);
	pos_print(1, 2);
	color_print_f(0, 200, 0); wprintf(L"KV-Map [inputs: %u, states: %u]\n", variables, mheight * mwidth); reset_print();
	print_frame_div(width);

	// number of variables on x and y axis
	unsigned int varny = variables / 2;
	unsigned int varnx = variables % 2 == 0 ? varny : varny + 1;

	// Print x axis variables
	for (unsigned int i = varnx; i > 0; i--) {
		unsigned int var = i * 2;
		unsigned int w = std::pow(2, i);
		unsigned int hw = w / 2;
		print_frame_side(width);
		pos_print(1, 2 + (varny + hw) * 3);
		for (unsigned int i1 = hw; i1 < mwidth; i1++) {
			if (((i1 + hw) / w) % 2) {
				color_print_f(255, 0, 0); wprintf(L" %lc ", 63 + var);
			} else {
				reset_print(); wprintf(L"   ");
			}
		}
		reset_print(); wprintf(L"\n");
	}

	for (unsigned int row = 0; row < mheight; row++) {

		print_frame_side(width);
		pos_print(1, 2);

		// Print y axis variables
		for (unsigned int i = varny; i > 0; i--) {
			unsigned int var = i * 2 + 1;
			unsigned int w = std::pow(2, i);
			unsigned int hw = w / 2;

			if (((row + hw) / w) % 2) {
				color_print_f(255, 0, 0); wprintf(L" %lc ", 63 + var); reset_print();
			} else {
				wprintf(L"   ");
			}
		}

		// Print table
		for (unsigned int col = 0; col < mwidth; col++) {

			switch (map.valueAt(col, row)) {
			case TRUE:
				color_print_b(0, 255, 0);
				color_print_f(0, 120, 0);
				wprintf(L" 1 ");
				break;
			case FALSE:
				color_print_b(255, 0, 0);
				color_print_f(120, 0, 0);
				wprintf(L" 0 ");
				break;
			case DONT_CARE:
				color_print_b(128, 128, 128);
				color_print_f(80, 80, 80);
				wprintf(L" X ");
				break;
			}

		}

		reset_print();
		wprintf(L"\n");

	}

	print_frame_bottom(width);

}

void print_qmcstack(const QMCStack& stack)
{

	unsigned int variables = stack.variableCount();
	unsigned int stages = stack.stageCount();
	unsigned int width = std::max(stages * (variables + 1) + 8, 35U);

	// print title
	print_frame_top(width);
	print_frame_side(width);
	pos_print(1, 2);
	color_print_f(0, 200, 0); wprintf(L"QMC-Chart [inputs: %u, stages: %u]\n", variables, stages);
	reset_print();
	print_frame_div(width);

	// print stage variable labels
	print_frame_side(width);
	pos_print(1, 2);
	wprintf(L"Nr.o.1");
	color_print_f(255, 0, 0);
	for (unsigned int s = 0; s < stack.stageCount(); s++)
	{
		wprintf(L" ");
		for (unsigned int v = 0; v < variables; v++) {
			wprintf(L"%c", 65 + v);
		}
	}
	reset_print();
	wprintf(L"\n");

	// print stack table
	for (unsigned int numberOfOnes = 0; numberOfOnes < variables; numberOfOnes++)
		for (unsigned int implicantIndx = 0; implicantIndx < stack.groupImplicantCount(numberOfOnes); implicantIndx++)
		{

			print_frame_side(width);
			pos_print(1, 2);

			// print number of ones for group
			wprintf(L" %03u   ", numberOfOnes);

			bool noMoreEntries = true;
			for (unsigned int stage = 0; stage < stages; stage++)
			{

				std::optional<std::reference_wrapper<const QMCImplicantSet>> implicantSet = stack.implicantSetFor(stage, numberOfOnes);

				if (!implicantSet.has_value() || implicantIndx >= implicantSet->get().implicantSet().size())
				{
					// empty cell int he table representation
					print_n(variables, L"-");
				}
				else
				{

					const QMCImplicant& implicant = implicantSet->get().implicantSet().at(implicantIndx);
					bool isPrime = implicant.isPrime();
					noMoreEntries = false;

					// print implicant variables
					for (auto varstate = implicant.variableStates().begin(); varstate != implicant.variableStates().end(); varstate++)
						switch(*varstate)
						{
						case TRUE:
							if (!isPrime) {
								color_print_b(0, 120, 0);
								color_print_f(0, 80, 0);
							} else {
								color_print_b(0, 255, 0);
								color_print_f(0, 120, 0);
							}
							wprintf(L"1");
							break;
						case FALSE:
							if (!isPrime) {
								color_print_b(120, 0, 0);
								color_print_f(80, 0, 0);
							} else {
								color_print_b(255, 0, 0);
								color_print_f(120, 0, 0);
							}
							wprintf(L"0");
							break;
						case DONT_CARE:
							if (!isPrime) {
								color_print_b(80, 80, 80);
								color_print_f(60, 60, 60);
							} else {
								color_print_b(128, 128, 128);
								color_print_f(80, 80, 80);
							}
							wprintf(L"-");
							break;
						}

				}

				reset_print();
				wprintf(L" ");

			}

			reset_print();
			wprintf(L"\n");

		}

	print_frame_bottom(width);

}

void print_qmcchart(const QMCPrimeChart& chart)
{

	unsigned int primes = chart.primeImplicants().size();
	unsigned int minterms = chart.mintermIds().size();
	unsigned int variables = chart.variableCount();
	unsigned int width = std::max(minterms * 5 + variables + 2, 55U);

	// print title
	print_frame_top(width);
	print_frame_side(width);
	pos_print(1, 2);
	color_print_f(0, 200, 0); wprintf(L"QMC Prime-Implicants Chart [primes: %u, minterms: %u]\n", primes, minterms);
	reset_print();
	print_frame_div(width);

	// print chart column labels
	print_frame_side(width);
	pos_print(1, 2);
	for (auto mintermId = chart.mintermIds().begin(); mintermId != chart.mintermIds().end(); mintermId++)
	{
		wprintf(L"m%03u|", *mintermId);
	}
	color_print_f(255, 0, 0);
	for (unsigned int v = 0; v < variables; v++) {
		wprintf(L"%c", 65 + v);
	}
	reset_print();
	wprintf(L"\n");

	for (auto implicant = chart.primeImplicants().begin(); implicant != chart.primeImplicants().end(); implicant++)
	{

		print_frame_side(width);
		pos_print(1, 2);

		// print minterm checkmarks
		for (auto mintermId = chart.mintermIds().begin(); mintermId != chart.mintermIds().end(); mintermId++)
		{

			bool coversTerm = std::find(implicant->mintermSet().begin(), implicant->mintermSet().end(), *mintermId) != implicant->mintermSet().end();

			if (coversTerm) {
				color_print_f(0, 255, 0);
				wprintf(L"  ✓ ");
				reset_print();
				wprintf(L"|");
			} else {
				wprintf(L"    |");
			}

		}

		// print implicant variable states
		for (auto varstate = implicant->variableStates().begin(); varstate != implicant->variableStates().end(); varstate++)
			switch(*varstate)
			{
			case TRUE:
				color_print_f(0, 120, 0);
				wprintf(L"1");
				break;
			case FALSE:
				color_print_f(120, 0, 0);
				wprintf(L"0");
				break;
			case DONT_CARE:
				color_print_f(80, 80, 80);
				wprintf(L"x");
				break;
			}

		reset_print();
		wprintf(L"\n");

	}

	print_frame_bottom(width);

}

void print_bool_term(const std::vector<QMCImplicant>& term)
{

	for (const QMCImplicant& implicant : term)
	{

		if (implicant != *term.begin())
		{
			reset_print();
			wprintf(L" + ");
		}

		for (unsigned int input = 0; input < implicant.variableCount(); input++)
		{

			TriStateBool state = implicant.variableStates().at(input);
			if (state != TriStateBool::DONT_CARE)
			{

				if (state == TriStateBool::TRUE)
				{
					color_print_f(0, 255, 0);
					wprintf(L"%c", 65 + input);
				}
				else
				{
					color_print_f(255, 0, 0);
					wprintf(L"%c'", 65 + input);
				}

			}

		}

	}
	reset_print();

}

void print_result_table(const std::vector<std::vector<QMCImplicant>>& finalTerms)
{

	unsigned int maxTermLen = 0;
	unsigned int inputCount = 0;
	for (const std::vector<QMCImplicant>& term : finalTerms) {
		unsigned long l = 0;
		for (const QMCImplicant& implicant : term) {
			unsigned int inputs = implicant.relevantInputCount();
			if (inputs > inputCount)
				inputCount = inputs;
			l += inputs * 2 - implicant.inputsTrueCount();
		}
		l += finalTerms.size() - 1 * 3;
		if (l > maxTermLen)
			maxTermLen = l;
	}
	unsigned int width = std::max(maxTermLen + 2, 50U);

	print_frame_top(width);
	print_frame_side(width);
	pos_print(1, 2);
	color_print_f(0, 200, 0); wprintf(L"Final Equation Table [equations: %u, inputs: %u]\n", finalTerms.size(), inputCount);
	reset_print();
	print_frame_div(width);

	for (unsigned int output = 0; output < finalTerms.size(); output++)
	{

		print_frame_side(width);
		pos_print(1, 2);

		color_print_f(0, 255, 0);
		wprintf(L"%02u", output);
		reset_print();
		wprintf(L" = ");
		print_bool_term(finalTerms.at(output));
		wprintf(L"\n");

	}

	print_frame_bottom(width);

}
