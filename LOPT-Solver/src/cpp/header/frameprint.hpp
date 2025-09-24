/*
 * tableprint.hpp
 *
 * Utility functions for printing the graphical console elements.
 *
 *  Created on: 18.09.2025
 *      Author: Marvin K. (M_Marvin)
 */

#ifndef SRC_CPP_HEADER_FRAMEPRINT_HPP_
#define SRC_CPP_HEADER_FRAMEPRINT_HPP_

void print_n(unsigned int n, const wchar_t* s);
void pos_print(int up, int col);
void color_print_f(unsigned int r, unsigned int g, unsigned int b);
void color_print_b(unsigned int r, unsigned int g, unsigned int b);
void reset_print();
void print_frame_top(unsigned int width);
void print_frame_side(unsigned int width);
void print_frame_div(unsigned int width);
void print_frame_bottom(unsigned int width);

#endif /* SRC_CPP_HEADER_FRAMEPRINT_HPP_ */
