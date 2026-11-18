/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file builtin_code.h
 *
 * @author Simon Šmíd        (xsmidsi00)
 *
 * @brief Declaration of built in functions.
 *
\*****************************************************************************/

#ifndef BUILTIN_CODE_H
#define BUILTIN_CODE_H

#include "inner_code.h"

extern inner_statement_t str_length_code;
extern inner_statement_t to_int_code;
extern inner_statement_t print_code;
extern inner_statement_t readline_code;
extern inner_statement_t readint_code;
extern inner_statement_t readdouble_code;
extern inner_statement_t to_str_code;
extern inner_statement_t sub_str_code;
extern inner_statement_t ord_code;
extern inner_statement_t chr_code;
extern inner_statement_t readboolean_code;
extern inner_statement_t readint_maybe_code;
extern inner_statement_t readdouble_maybe_code;
extern inner_statement_t floor_code;
extern inner_statement_t head_code;
extern inner_statement_t tail_code;
extern inner_statement_t slice_code;
extern inner_statement_t take_code;
extern inner_statement_t drop_code;
extern inner_statement_t filter_code;
extern inner_statement_t take_while_code;
extern inner_statement_t drop_while_code;
extern inner_statement_t range_code;
extern inner_statement_t to_list_code;
extern inner_statement_t to_code;
extern inner_statement_t by_code;
extern inner_statement_t until_code;

#endif
