/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file code_gen.h
 *
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Code generator interface specification
 *
\******************************************************************************/

#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "err_codes.h"
#include "inner_code.h"
#include <stdio.h>

/**
 * @brief Prints the whole ifjcode26 program
 *
 * @param statement The first statement of the program
 * @param stream Where to print the data (stdout,stderr,file,...)
 */
void codegen_print_program(inner_statement_t *statement, FILE *stream);

#endif
