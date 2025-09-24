/*
 * frameprint.cpp
 *
 * Utility functions for printing the graphical console elements.
 *
 *  Created on: 24.09.2025
 *      Author: Marvin K. (M_Marvin)
 */

#include <stdio.h>
#include "frameprint.hpp"

void print_n(unsigned int n, const wchar_t* s) {
	for (unsigned int i = 0; i < n; i++)
		wprintf(s);
}

void pos_print(int up, int col) {
	if (up > 0)
		wprintf(L"\033[%dA", up);
	else if (up < 0)
		wprintf(L"\033[%dB", -up);

	wprintf(L"\033[%dG", col + 1);
//	if (left > 0)
//		wprintf(L"\033[%dD", left);
//	else if (left < 0)
//		wprintf(L"\033[%dC", -left);
}

void color_print_f(unsigned int r, unsigned int g, unsigned int b) {
	wprintf(L"\033[38;2;%d;%d;%dm", r, g, b);
}

void color_print_b(unsigned int r, unsigned int g, unsigned int b) {
	wprintf(L"\033[48;2;%d;%d;%dm", r, g, b);
}

void reset_print() {
	wprintf(L"\033[2;0;0m");
}

void print_frame_top(unsigned int width) {
	wprintf(L"╔");
	print_n(width, L"═");
	wprintf(L"╗\n");
}

void print_frame_side(unsigned int width) {
	wprintf(L"║");
	pos_print(0, width + 1);
	wprintf(L"║\n");
}

void print_frame_div(unsigned int width) {
	wprintf(L"╟");
	print_n(width, L"─");
	wprintf(L"╢\n");
}

void print_frame_bottom(unsigned int width) {
	wprintf(L"╚");
	print_n(width, L"═");
	wprintf(L"╝\n");
}

