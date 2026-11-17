/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file builtin_code.c
 *
 * @author Simon Šmíd        (xsmidsi00)
 *
 * @brief Implementation of built in functions.
 *
\*****************************************************************************/

#include "builtin_code.h"

/**
 * strLength(value: String): Int
 */

// RETURN
static inner_statement_t str_length_s9 = {
    .operation = INNER_RETURN,
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t str_length_s8 = {
    .operation = INNER_POPFRAME,
    .next_statement = &str_length_s9,
    .is_static = true,
};

// PUSHS LF@ret
static inner_statement_t str_length_s7 = {
    .operation = INNER_PUSHS,
    .destination = {.type = INNER_SYM_VAR, .data.identificator = "LF@ret"},
    .next_statement = &str_length_s8,
    .is_static = true,
};

// STRLEN LF@ret LF@value
static inner_statement_t str_length_s6 = {
    .operation = INNER_STRLEN,
    .destination = {.type = INNER_SYM_VAR, .data.identificator = "LF@ret"},
    .first_op = {.type = INNER_SYM_VAR, .data.identificator = "LF@value"},
    .next_statement = &str_length_s7,
    .is_static = true,
};

// POPS LF@value
static inner_statement_t str_length_s5 = {
    .operation = INNER_POPS,
    .destination = {.type = INNER_SYM_VAR, .data.identificator = "LF@value"},
    .next_statement = &str_length_s6,
    .is_static = true,
};

// DEFVAR LF@ret
static inner_statement_t str_length_s4 = {
    .operation = INNER_DEFVAR,
    .destination = {.type = INNER_SYM_VAR, .data.identificator = "LF@ret"},
    .next_statement = &str_length_s5,
    .is_static = true,
};

// DEFVAR LF@value
static inner_statement_t str_length_s3 = {
    .operation = INNER_DEFVAR,
    .destination = {.type = INNER_SYM_VAR, .data.identificator = "LF@value"},
    .next_statement = &str_length_s4,
    .is_static = true,
};

// PUSHFRAME
static inner_statement_t str_length_s2 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &str_length_s3,
    .is_static = true,
};

// CREATEFRAME
static inner_statement_t str_length_s1 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &str_length_s2,
    .is_static = true,
};

// LABEL strLength
inner_statement_t str_length_code = {
    .operation = INNER_LABEL,
    .destination = {.type = INNER_SYM_LABEL, .data.identificator = "strLength"},
    .next_statement = &str_length_s1,
    .is_static = true,
};

/**
 * toInt(value: Double): Int
 */

// RETURN
static inner_statement_t to_int_s2 = {
    .operation = INNER_RETURN,
    .next_statement = NULL,
    .is_static = true,
};

// FLOAT2INTS
static inner_statement_t to_int_s1 = {
    .operation = INNER_FLOAT2INTS,
    .next_statement = &to_int_s2,
    .is_static = true,
};

// LABEL toInt
inner_statement_t to_int_code = {
    .operation = INNER_LABEL,
    .destination = {.type = INNER_SYM_LABEL, .data.identificator = "toInt"},
    .next_statement = &to_int_s1,
    .is_static = true,
};