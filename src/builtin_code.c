/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file builtin_code.c
 *
 * @author Šmíd Simon        (xsmidsi00)
 *
 * @brief Implementation of built in functions.
 *
\*****************************************************************************/
#include "builtin_code.h"

static inner_statement_t str_length_s9 = {
    .operation = INNER_RETURN,
    .next_statement = NULL,
};

static inner_statement_t str_length_s8 = {
    .operation = INNER_POPFRAME,
    .next_statement = &str_length_s9,
};

static inner_statement_t str_length_s7 = {
    .operation = INNER_PUSHS,
    .destination = {.type = INNER_SYM_VAR, .data.identificator = "LF@ret"},
    .next_statement = &str_length_s8,
};

static inner_statement_t str_length_s6 = {
    .operation = INNER_STRLEN,
    .destination = {.type = INNER_SYM_VAR, .data.identificator = "LF@ret"},
    .first_op = {.type = INNER_SYM_VAR, .data.identificator = "LF@value"},
    .next_statement = &str_length_s7,
};

static inner_statement_t str_length_s5 = {
    .operation = INNER_POPS,
    .destination = {.type = INNER_SYM_VAR, .data.identificator = "LF@value"},
    .next_statement = &str_length_s6,
};

static inner_statement_t str_length_s4 = {
    .operation = INNER_DEFVAR,
    .destination = {.type = INNER_SYM_VAR, .data.identificator = "LF@ret"},
    .next_statement = &str_length_s5,
};

static inner_statement_t str_length_s3 = {
    .operation = INNER_DEFVAR,
    .destination = {.type = INNER_SYM_VAR, .data.identificator = "LF@value"},
    .next_statement = &str_length_s4,
};

static inner_statement_t str_length_s2 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &str_length_s3,
};

static inner_statement_t str_length_s1 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &str_length_s2,
};

inner_statement_t str_length_code = {
    .operation = INNER_LABEL,
    .destination = {.type = INNER_SYM_LABEL, .data.identificator = "strLength"},
    .next_statement = &str_length_s1,
};