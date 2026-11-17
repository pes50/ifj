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
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@ret",
        },
    .next_statement = &str_length_s8,
    .is_static = true,
};

// STRLEN LF@ret LF@value
static inner_statement_t str_length_s6 = {
    .operation = INNER_STRLEN,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@ret",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &str_length_s7,
    .is_static = true,
};

// POPS LF@value
static inner_statement_t str_length_s5 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &str_length_s6,
    .is_static = true,
};

// DEFVAR LF@ret
static inner_statement_t str_length_s4 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@ret",
        },
    .next_statement = &str_length_s5,
    .is_static = true,
};

// DEFVAR LF@value
static inner_statement_t str_length_s3 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
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
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "strLength",
        },
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
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "toInt",
        },
    .next_statement = &to_int_s1,
    .is_static = true,
};

/**
 * print(value: Any): Unit
 */

// RETURN
static inner_statement_t print_s15 = {
    .operation = INNER_RETURN,
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t print_s14 = {
    .operation = INNER_POPFRAME,
    .next_statement = &print_s15,
    .is_static = true,
};

// PUSHS nil@nil
static inner_statement_t print_s13 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_CONST,
            .data.constant.datatype = INNER_NIL,
        },
    .next_statement = &print_s14,
    .is_static = true,
};

// LABEL print_end
static inner_statement_t print_s12 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "print_end",
        },
    .next_statement = &print_s13,
    .is_static = true,
};

// WRITE " () "   Print " () " in case Unit type was given.
static inner_statement_t print_s11 = {
    .operation = INNER_WRITE,
    .destination =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_STRING,
                    .value.inner_string = " () ",
                },
        },
    .next_statement = &print_s12,
    .is_static = true,
};

// LABEL print_unit
static inner_statement_t print_s10 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "print_unit",
        },
    .next_statement = &print_s11,
    .is_static = true,
};

// JUMP print_end
static inner_statement_t print_s9 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "print_end",
        },
    .next_statement = &print_s10,
    .is_static = true,
};

// WRITE LF@value
static inner_statement_t print_s8 = {
    .operation = INNER_WRITE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &print_s9,
    .is_static = true,
};

// JUMPIFEQ print_unit LF@type string@nil
static inner_statement_t print_s7 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "print_unit",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@type",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_STRING,
                    .value.inner_string = "nil",
                },
        },
    .next_statement = &print_s8,
    .is_static = true,
};

// TYPE LF@type LF@value
static inner_statement_t print_s6 = {
    .operation = INNER_TYPE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@type",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &print_s7,
    .is_static = true,
};

// POPS LF@value
static inner_statement_t print_s5 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &print_s6,
    .is_static = true,
};

// DEFVAR LF@type
static inner_statement_t print_s4 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@type",
        },
    .next_statement = &print_s5,
    .is_static = true,
};

// DEFVAR LF@value
static inner_statement_t print_s3 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &print_s4,
    .is_static = true,
};

// PUSHFRAME
static inner_statement_t print_s2 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &print_s3,
    .is_static = true,
};

// CREATEFRAME
static inner_statement_t print_s1 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &print_s2,
    .is_static = true,
};

// LABEL print
inner_statement_t print_code = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "print",
        },
    .next_statement = &print_s1,
    .is_static = true,
};

/**
 * readLine(): String
 */

// RETURN
static inner_statement_t readline_s12 = {
    .operation = INNER_RETURN,
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t readline_s11 = {
    .operation = INNER_POPFRAME,
    .next_statement = &readline_s12,
    .is_static = true,
};

// PUSHS LF@readed_value
static inner_statement_t readline_s10 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .next_statement = &readline_s11,
    .is_static = true,
};

// LABEL read_line_end
static inner_statement_t readline_s9 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "read_line_end",
        },
    .next_statement = &readline_s10,
    .is_static = true,
};

// MOVE LF@readed_vale string@
static inner_statement_t readline_s8 = {
    .operation = INNER_MOVE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .first_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_STRING,
                    .value.inner_string = "",
                },
        },
    .next_statement = &readline_s9,
    .is_static = true,
};

// LABEL read_line_eof
static inner_statement_t readline_s7 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "read_line_eof",
        },
    .next_statement = &readline_s8,
    .is_static = true,
};

// JUMP read_line_end
static inner_statement_t readline_s6 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "read_line_end",
        },
    .next_statement = &readline_s7,
    .is_static = true,
};

// JUMPIFEQ readline_eof LF@readed_value nil@nil
static inner_statement_t readline_s5 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "read_line_eof",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant.datatype = INNER_NIL,
        },
    .next_statement = &readline_s6,
    .is_static = true,
};

// READ LF@readed_value string
static inner_statement_t readline_s4 = {
    .operation = INNER_READ,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .first_op =
        {
            .type = INNER_SYM_ARR,
            .data.identificator = "string",
        },
    .next_statement = &readline_s5,
    .is_static = true,
};

// DEFVAR LF@readed_value
static inner_statement_t readline_s3 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .next_statement = &readline_s4,
    .is_static = true,
};

// PUSHFRAME
static inner_statement_t readline_s2 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &readline_s3,
    .is_static = true,
};

// CREATEFRAME
static inner_statement_t readline_s1 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &readline_s2,
    .is_static = true,
};

// LABEL readLine
inner_statement_t readline_code = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "readLine",
        },
    .next_statement = &readline_s1,
    .is_static = true,
};

/**
 * readInt(): Int
 */

// RETURN
static inner_statement_t readint_s12 = {
    .operation = INNER_RETURN,
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t readint_s11 = {
    .operation = INNER_POPFRAME,
    .next_statement = &readint_s12,
    .is_static = true,
};

// PUSHS LF@readed_value
static inner_statement_t readint_s10 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .next_statement = &readint_s11,
    .is_static = true,
};

// LABEL read_int_end
static inner_statement_t readint_s9 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "read_int_end",
        },
    .next_statement = &readint_s10,
    .is_static = true,
};

// MOVE LF@readed_value int@0
static inner_statement_t readint_s8 = {
    .operation = INNER_MOVE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .first_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 0,
                },
        },
    .next_statement = &readint_s9,
    .is_static = true,
};

// LABEL read_int_nil
static inner_statement_t readint_s7 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "read_int_nil",
        },
    .next_statement = &readint_s8,
    .is_static = true,
};

// JUMP read_int_end
static inner_statement_t readint_s6 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "read_int_end",
        },
    .next_statement = &readint_s7,
    .is_static = true,
};

// JUMPIFEQ read_int_nil LF@readed_value nil@nil
static inner_statement_t readint_s5 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "read_int_nil",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant.datatype = INNER_NIL,
        },
    .next_statement = &readint_s6,
    .is_static = true,
};

// READ LF@readed_value int
static inner_statement_t readint_s4 = {
    .operation = INNER_READ,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .first_op =
        {
            .type = INNER_SYM_ARR,
            .data.identificator = "int",
        },
    .next_statement = &readint_s5,
    .is_static = true,
};

// DEFVAR LF@readed_value
static inner_statement_t readint_s3 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .next_statement = &readint_s4,
    .is_static = true,
};

// PUSHFRAME
static inner_statement_t readint_s2 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &readint_s3,
    .is_static = true,
};

// CREATEFRAME
static inner_statement_t readint_s1 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &readint_s2,
    .is_static = true,
};

// LABEL readInt
inner_statement_t readint_code = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "readInt",
        },
    .next_statement = &readint_s1,
    .is_static = true,
};

/**
 * readDouble(): Double
 */

// RETURN
static inner_statement_t readdouble_s12 = {
    .operation = INNER_RETURN,
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t readdouble_s11 = {
    .operation = INNER_POPFRAME,
    .next_statement = &readdouble_s12,
    .is_static = true,
};

// PUSHS LF@readed_value
static inner_statement_t readdouble_s10 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .next_statement = &readdouble_s11,
    .is_static = true,
};

// LABEL read_double_end
static inner_statement_t readdouble_s9 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "read_double_end",
        },
    .next_statement = &readdouble_s10,
    .is_static = true,
};

// MOVE LF@readed_value float@0
static inner_statement_t readdouble_s8 = {
    .operation = INNER_MOVE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .first_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_FLOAT,
                    .value.inner_float = 0.0,
                },
        },
    .next_statement = &readdouble_s9,
    .is_static = true,
};

// LABEL read_double_nil
static inner_statement_t readdouble_s7 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "read_double_nil",
        },
    .next_statement = &readdouble_s8,
    .is_static = true,
};

// JUMP read_double_end
static inner_statement_t readdouble_s6 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "read_double_end",
        },
    .next_statement = &readdouble_s7,
    .is_static = true,
};

// JUMPIFEQ read_double_nil LF@readed_value nil@nil
static inner_statement_t readdouble_s5 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "read_double_nil",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant.datatype = INNER_NIL,
        },
    .next_statement = &readdouble_s6,
    .is_static = true,
};

// READ LF@readed_value float
static inner_statement_t readdouble_s4 = {
    .operation = INNER_READ,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .first_op =
        {
            .type = INNER_SYM_ARR,
            .data.identificator = "float",
        },
    .next_statement = &readdouble_s5,
    .is_static = true,
};

// DEFVAR LF@readed_value
static inner_statement_t readdouble_s3 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .next_statement = &readdouble_s4,
    .is_static = true,
};

// PUSHFRAME
static inner_statement_t readdouble_s2 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &readdouble_s3,
    .is_static = true,
};

// CREATEFRAME
static inner_statement_t readdouble_s1 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &readdouble_s2,
    .is_static = true,
};

// LABEL readDouble
inner_statement_t readdouble_code = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "readDouble",
        },
    .next_statement = &readdouble_s1,
    .is_static = true,
};
