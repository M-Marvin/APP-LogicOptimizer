/*
 * climain.cpp
 *
 * Main entry point of the solver.
 * Parses command line input, loads the truth table file and controls the processing.
 *
 *  Created on: 18.09.2025
 *      Author: Marvin K. (M_Marvin)
 */

#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "tableprint.hpp"
#include "truthtable.hpp"
#include "kvm.hpp"
#include "qmcp.hpp"

bool process_table(TruthTable& table, bool verbose)
{

	wprintf(L"[i] starting to process table ...\n");
	print_truthtable(table);

	std::vector<std::vector<QMCImplicant>> finalTerms;

	for (unsigned int o = 0; o < table.outputCount(); o++)
	{

		wprintf(L"[i] solve for output %u (%u/%u) ...\n", o, o+1, table.outputCount());

		/**
		 * here we convert the truth table to an KV-map
		 * this mainly serves two purposes:
		 * - fill in the gaps in the truth table (omitted don't care states)
		 * - give a visual representation of the function currently being processed
		 *
		 * this could be done without an KV-map
		 */
		wprintf(L"[i] generate KV map to generate fully defined function ...\n");
		KVMap kvMap(table, o);
		if (verbose) print_kvmap(kvMap);

		/**
		 * next the QMC (Quine–McCluskey) algorithm is applied.
		 * here we initialize the initial minterms in the QMC-stack with the states
		 * of the KV-map which evaluate either to TRUE or DONT_CARE.
		 */
		wprintf(L"[i] initialize QMC implicant chart ...\n");
		QMCStack implicantStack;
		implicantStack.initialize(kvMap);

		/**
		 * next the QMC algorithm is applied to find the prime implicants.
		 * these can not be combined any further and are highlighted in the printed table.
		 */
		wprintf(L"[i] searching for prime implicants ...\n");
		while (implicantStack.tryMerge());
		if (verbose) print_qmcstack(implicantStack);

		/**
		 * here we extract all prime implicants from the stack and put them
		 * into an QMC prime implicant chart.
		 */
		wprintf(L"[i] initializing prime implicant chart ...\n");
		QMCPrimeChart chart;
		chart.initialize(implicantStack);
		if (verbose) print_qmcchart(chart);

		/**
		 * identifying the essential prime implicant terms in the chart and
		 * remove them and store them in an list
		 */
		wprintf(L"[i] calculating optimal prime implicants for minimal logical function ...\n");
		wprintf(L"[i] identifying essential prime implicants ... ");
		std::vector<QMCImplicant> essentialPrimeImplicants;
		chart.extractEPIs(essentialPrimeImplicants);
		wprintf(L"EPIs: %u\n", essentialPrimeImplicants.size());

		/**
		 * if non essential primes are remaining, find the optimal combination of them
		 * which covers all remaining minterms with the least number of terms.
		 */
		if (chart.mintermIds().size() != 0)
		{
			wprintf(L"[i] non essential prime implicants remaining, continue ...\n");
			if (verbose) print_qmcchart(chart);
			chart.findOptimalPrimes(essentialPrimeImplicants);

		}

		finalTerms.push_back(essentialPrimeImplicants);
		wprintf(L"[i] -> final term: ");
		print_bool_term(essentialPrimeImplicants);
		wprintf(L"\n");

	}

	wprintf(L"[i] terms for all outputs completed:\n");
	print_result_table(finalTerms);

	return true;
}

int climain(std::string cmdname, std::vector<std::string> args)
{

	if (args.size() < 4)
	{
		wprintf(L"%s -tt [truth table txt] <-o [num of ouputs] | -i [num of inputs] | -v (verbose output enable)>\n", cmdname.c_str());
		return 0;
	}

	_setmode(_fileno(stdout), _O_U16TEXT);

	std::string tableFilePath;
	unsigned int inputs = 0;
	unsigned int outputs = 0;
	bool verbose = false;

	for (unsigned int i = 0; i < args.size(); i++)
	{
		std::string flag = args[i];
		if (i != args.size() - 1) {
			std::string val = args[i + 1];
			if (flag == "-tt") {
				tableFilePath = val;
				i++;
			} else if (flag == "-i") {
				inputs = std::stoul(val);
				i++;
			} else if (flag == "-o") {
				outputs = std::stoul(val);
				i++;
			}
		}
		if (flag == "-v") {
			verbose = true;
		}
	}

	if (tableFilePath.empty()) {
		wprintf(L"[!] truth table file not defined!\n");
		return 1;
	}

	if (inputs == 0 && outputs == 0) {
		wprintf(L"[!] number of inputs and/or outputs not defined!\n");
		return 1;
	}

	wprintf(L"[i] loading truth table from file ...\n");

	// try to load truth table file
	std::string tableStr;
	{
		std::ifstream tableFile;
		tableFile.open(tableFilePath);
		std::stringstream tableStream;
		tableStream << tableFile.rdbuf();
		tableFile.close();
		tableStr = tableStream.str();
	}

	wprintf(L"[i] parsing :\n%s\n", tableStr.c_str());

	// try to parse truth table string
	std::vector<TriStateBool> values;
	unsigned int width = 0;
	{
		unsigned int pos = 0;
		unsigned int lastCol = 0;
		unsigned int lastRow = 0;
		unsigned int columns = 0;
		while (true)
		{

			if (pos == lastRow)
			{
				lastRow = tableStr.find("\n", pos) + 1;
				if (lastRow == 0) break; // end of table string
				if (width > 0 && width != columns)
				{
					wprintf(L"[!] truth table rows mismatch in number of columns!\n");
					return 1;
				}
				width = columns;
				columns = 0;
			}

			lastCol = tableStr.find("\t", pos) + 1;
			if (lastCol == 0) lastCol = lastRow;

			unsigned int pos2 = std::min(lastCol, lastRow);

			std::string valStr = tableStr.substr(pos,  pos2 - pos);
			if (valStr.find("TRUE") != std::string::npos || valStr.find("true") != std::string::npos || valStr.find("1") != std::string::npos) {
				values.push_back(TriStateBool::TRUE);
			} else if (valStr.find("FALSE") != std::string::npos || valStr.find("false") != std::string::npos || valStr.find("0") != std::string::npos) {
				values.push_back(TriStateBool::FALSE);
			} else {
				values.push_back(TriStateBool::DONT_CARE);
			}

			pos = pos2;
			columns++;

		}
	}

	// validate data
	wprintf(L"[i] checking cell count : width = %u data = %lu ... ", width, values.size());
	if (width == 0) {
		wprintf(L"\n[!] empty table data!\n");
		return 1;
	} else if (values.size() % width != 0) {
		wprintf(L"\n[!] table data incomplete!\n");
		return 1;
	}
	wprintf(L"OK\n", width);

	wprintf(L"[i] checking input output count : width = %u inputs = %u outputs = %u ... ", width, inputs, outputs);
	if (inputs == 0)
		inputs = width - outputs;
	else if (outputs == 0)
		outputs = width - inputs;
	else if (outputs + inputs != width)
	{
		wprintf(L"\n[!] number of inputs + outputs does not match table width!\n");
		return 1;
	}
	wprintf(L"OK\n", width);

	// fill table with output states
	TruthTable table(values, inputs, outputs);

	wprintf(L"[i] table loaded successfully\n");

	return process_table(table, verbose) ? 0 : -1;

}

int main(int argc, const char** argv)
{

	std::string exec(argv[0]);
	std::string execName = exec.substr(exec.find_last_of("/\\") + 1);
	std::vector<std::string> args;
	for (unsigned int i1 = 1; i1 < argc; i1++)
		args.emplace_back(argv[i1]);

	return climain(execName, args);

}
