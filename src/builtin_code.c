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

// LABEL $$print_end
static inner_statement_t print_s12 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$print_end",
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

// LABEL $$print_unit
static inner_statement_t print_s10 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$print_unit",
        },
    .next_statement = &print_s11,
    .is_static = true,
};

// JUMP $$print_end
static inner_statement_t print_s9 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$print_end",
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

// JUMPIFEQ $$print_unit LF@type string@nil
static inner_statement_t print_s7 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$print_unit",
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

// LABEL $$read_line_end
static inner_statement_t readline_s9 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$read_line_end",
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

// LABEL $$read_line_eof
static inner_statement_t readline_s7 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$read_line_eof",
        },
    .next_statement = &readline_s8,
    .is_static = true,
};

// JUMP $$read_line_end
static inner_statement_t readline_s6 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$read_line_end",
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
            .data.identificator = "$$read_line_eof",
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

// LABEL $$read_int_end
static inner_statement_t readint_s9 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$read_int_end",
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

// LABEL $$read_int_nil
static inner_statement_t readint_s7 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$read_int_nil",
        },
    .next_statement = &readint_s8,
    .is_static = true,
};

// JUMP $$read_int_end
static inner_statement_t readint_s6 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$read_int_end",
        },
    .next_statement = &readint_s7,
    .is_static = true,
};

// JUMPIFEQ $$read_int_nil LF@readed_value nil@nil
static inner_statement_t readint_s5 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$read_int_nil",
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

// LABEL $$read_double_end
static inner_statement_t readdouble_s9 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$read_double_end",
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

// LABEL $$read_double_nil
static inner_statement_t readdouble_s7 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$read_double_nil",
        },
    .next_statement = &readdouble_s8,
    .is_static = true,
};

// JUMP $$read_double_end
static inner_statement_t readdouble_s6 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$read_double_end",
        },
    .next_statement = &readdouble_s7,
    .is_static = true,
};

// JUMPIFEQ $$read_double_nil LF@readed_value nil@nil
static inner_statement_t readdouble_s5 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$read_double_nil",
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

/**
 * toStr(value: Any): String
 */

// RETURN
static inner_statement_t to_str_s30 = {
    .operation = INNER_RETURN,
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t to_str_s29 = {
    .operation = INNER_POPFRAME,
    .next_statement = &to_str_s30,
    .is_static = true,
};

// PUSHS LF@return_val
static inner_statement_t to_str_s28 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .next_statement = &to_str_s29,
    .is_static = true,
};

// LABEL $$to_str_end
static inner_statement_t to_str_s27 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_end",
        },
    .next_statement = &to_str_s28,
    .is_static = true,
};

// MOVE LF@retrurn_val string@()
static inner_statement_t to_str_s26 = {
    .operation = INNER_MOVE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .first_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_STRING,
                    .value.inner_string = " () ",
                },
        },
    .next_statement = &to_str_s27,
    .is_static = true,
};

// LABEL $$to_str_nil
static inner_statement_t to_str_s25 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_nil",
        },
    .next_statement = &to_str_s26,
    .is_static = true,
};

// JUMP $$to_str_end
static inner_statement_t to_str_s24 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_end",
        },
    .next_statement = &to_str_s25,
    .is_static = true,
};

// MOVE LF@return_val string@true
static inner_statement_t to_str_s23 = {
    .operation = INNER_MOVE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .first_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_STRING,
                    .value.inner_string = "true",
                },
        },
    .next_statement = &to_str_s24,
    .is_static = true,
};

// JUMPIFNEQ $$to_str_end LF@value bool@true
static inner_statement_t to_str_s22 = {
    .operation = INNER_JUMPIFNEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_end",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_BOOL,
                    .value.inner_bool = true,
                },
        },
    .next_statement = &to_str_s23,
    .is_static = true,
};

// MOVE LF@return_val string@false
static inner_statement_t to_str_s21 = {
    .operation = INNER_MOVE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .first_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_STRING,
                    .value.inner_string = "false",
                },
        },
    .next_statement = &to_str_s22,
    .is_static = true,
};

// LABEL $$to_str_bool
static inner_statement_t to_str_s20 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_bool",
        },
    .next_statement = &to_str_s21,
    .is_static = true,
};

// JUMP $$to_str_end
static inner_statement_t to_str_s19 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_end",
        },
    .next_statement = &to_str_s20,
    .is_static = true,
};

// FLOAT2STR LF@retrurn_val LF@value
static inner_statement_t to_str_s18 = {
    .operation = INNER_FLOAT2STR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &to_str_s19,
    .is_static = true,
};

// LABEL $$to_str_float
static inner_statement_t to_str_s17 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_float",
        },
    .next_statement = &to_str_s18,
    .is_static = true,
};

// JUMP $$to_str_end
static inner_statement_t to_str_s16 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_end",
        },
    .next_statement = &to_str_s17,
    .is_static = true,
};

// INT2STR LF@retrurn_val LF@value
static inner_statement_t to_str_s15 = {
    .operation = INNER_INT2STR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &to_str_s16,
    .is_static = true,
};

// LABEL $$to_str_int
static inner_statement_t to_str_s14 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_int",
        },
    .next_statement = &to_str_s15,
    .is_static = true,
};

// JUMP $$to_str_end
static inner_statement_t to_str_s13 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_end",
        },
    .next_statement = &to_str_s14,
    .is_static = true,
};

// MOVE LF@retrurn_val LF@value
static inner_statement_t to_str_s12 = {
    .operation = INNER_MOVE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &to_str_s13,
    .is_static = true,
};

// JUMPIFEQ $$to_str_nil LF@type string@nil
static inner_statement_t to_str_s11 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_nil",
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
    .next_statement = &to_str_s12,
    .is_static = true,
};

// JUMPIFEQ $$to_str_bool LF@type string@bool
static inner_statement_t to_str_s10 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_bool",
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
                    .value.inner_string = "bool",
                },
        },
    .next_statement = &to_str_s11,
    .is_static = true,
};

// JUMPIFEQ $$to_str_float LF@type string@float
static inner_statement_t to_str_s9 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_float",
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
                    .value.inner_string = "float",
                },
        },
    .next_statement = &to_str_s10,
    .is_static = true,
};

// JUMPIFEQ $$to_str_int LF@value string@int
static inner_statement_t to_str_s8 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_int",
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
                    .value.inner_string = "int",
                },
        },
    .next_statement = &to_str_s9,
    .is_static = true,
};

// TYPE LF@type LF@value
static inner_statement_t to_str_s7 = {
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
    .next_statement = &to_str_s8,
    .is_static = true,
};

// POPS LF@value
static inner_statement_t to_str_s6 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &to_str_s7,
    .is_static = true,
};

// DEFVAR LF@return_val
static inner_statement_t to_str_s5 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .next_statement = &to_str_s6,
    .is_static = true,
};

// DEFVAR LF@type
static inner_statement_t to_str_s4 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@type",
        },
    .next_statement = &to_str_s5,
    .is_static = true,
};

// DEFVAR LF@value
static inner_statement_t to_str_s3 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &to_str_s4,
    .is_static = true,
};

// PUSHFRAME
static inner_statement_t to_str_s2 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &to_str_s3,
    .is_static = true,
};

// CREATEFRAME
static inner_statement_t to_str_s1 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &to_str_s2,
    .is_static = true,
};

// LABEL toStr
inner_statement_t to_str_code = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "toStr",
        },
    .next_statement = &to_str_s1,
    .is_static = true,
};

/**
 * subStr(value: String, start: Int, end: Int): String
 */

// EXIT int@40
static inner_statement_t sub_str_s36 = {
    .operation = INNER_EXIT,
    .destination =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 40,
                },
        },
    .is_static = true,
};

// LABEL $$sub_str_error
static inner_statement_t sub_str_s35 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$sub_str_error",
        },
    .next_statement = &sub_str_s36,
    .is_static = true,
};

// RETURN
static inner_statement_t sub_str_s34 = {
    .operation = INNER_RETURN,
    .next_statement = &sub_str_s35,
    .is_static = true,
};

// POPFRAME
static inner_statement_t sub_str_s33 = {
    .operation = INNER_POPFRAME,
    .next_statement = &sub_str_s34,
    .is_static = true,
};

// PUSHS LF@return_val
static inner_statement_t sub_str_s32 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .next_statement = &sub_str_s33,
    .is_static = true,
};

// LABEL $$sub_str_loop_done
static inner_statement_t sub_str_s31 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$sub_str_loop_done",
        },
    .next_statement = &sub_str_s32,
    .is_static = true,
};

// JUMP $$to_str_loop
static inner_statement_t sub_str_s30 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$sub_str_loop",
        },
    .next_statement = &sub_str_s31,
    .is_static = true,
};

// ADD LF@start LF@start int@1
static inner_statement_t sub_str_s29 = {
    .operation = INNER_ADD,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@start",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@start",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 1,
                },
        },
    .next_statement = &sub_str_s30,
    .is_static = true,
};

// CONCAT LF@return_val LF@return_val LF@character
static inner_statement_t sub_str_s28 = {
    .operation = INNER_CONCAT,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@character",
        },
    .next_statement = &sub_str_s29,
    .is_static = true,
};

// GETCHAR LF@character LF@value LF@start
static inner_statement_t sub_str_s27 = {
    .operation = INNER_GETCHAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@character",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@start",
        },
    .next_statement = &sub_str_s28,
    .is_static = true,
};

// JUMPIFEQ $$sub_str_loop_done LF@start LF@end
static inner_statement_t sub_str_s26 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$sub_str_loop_done",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@start",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@end",
        },
    .next_statement = &sub_str_s27,
    .is_static = true,
};

// LABEL $$sub_str_loop
static inner_statement_t sub_str_s25 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$sub_str_loop",
        },
    .next_statement = &sub_str_s26,
    .is_static = true,
};

// MOVE LF@return_val string@
static inner_statement_t sub_str_s24 = {
    .operation = INNER_MOVE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
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
    .next_statement = &sub_str_s25,
    .is_static = true,
};

// JUMPIFEQ $$sub_str_error LF@precondition bool@true
static inner_statement_t sub_str_s23 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$sub_str_error",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_BOOL,
                    .value.inner_bool = true,
                },
        },
    .next_statement = &sub_str_s24,
    .is_static = true,
};

// GT LF@precondition LF@end LF@value_len
static inner_statement_t sub_str_s22 = {
    .operation = INNER_GT,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@end",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value_len",
        },
    .next_statement = &sub_str_s23,
    .is_static = true,
};

// JUMPIFEQ $$sub_str_error LF@precondition bool@false
static inner_statement_t sub_str_s21 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$sub_str_error",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_BOOL,
                    .value.inner_bool = false,
                },
        },
    .next_statement = &sub_str_s22,
    .is_static = true,
};

// LT LF@precondition LF@start LF@value_len
static inner_statement_t sub_str_s20 = {
    .operation = INNER_LT,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@start",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value_len",
        },
    .next_statement = &sub_str_s21,
    .is_static = true,
};

// JUMPIFEQ $$sub_str_error LF@precondition bool@true
static inner_statement_t sub_str_s19 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$sub_str_error",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_BOOL,
                    .value.inner_bool = true,
                },
        },
    .next_statement = &sub_str_s20,
    .is_static = true,
};

// GT LF@precondition LF@start LF@end
static inner_statement_t sub_str_s18 = {
    .operation = INNER_GT,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@start",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@end",
        },
    .next_statement = &sub_str_s19,
    .is_static = true,
};

// JUMPIFEQ $$sub_str_error LF@precondition bool@true
static inner_statement_t sub_str_s17 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$sub_str_error",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_BOOL,
                    .value.inner_bool = true,
                },
        },
    .next_statement = &sub_str_s18,
    .is_static = true,
};

// LT LF@precondition LF@end int@0
static inner_statement_t sub_str_s16 = {
    .operation = INNER_LT,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@end",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 0,
                },
        },
    .next_statement = &sub_str_s17,
    .is_static = true,
};

// JUMPIFEQ $$sub_str_error LF@precondition bool@true
static inner_statement_t sub_str_s15 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$sub_str_error",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_BOOL,
                    .value.inner_bool = true,
                },
        },
    .next_statement = &sub_str_s16,
    .is_static = true,
};

// LT LF@precondition LF@start int@0
static inner_statement_t sub_str_s14 = {
    .operation = INNER_LT,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@start",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 0,
                },
        },
    .next_statement = &sub_str_s15,
    .is_static = true,
};

// STRLEN LF@value_len LF@value
static inner_statement_t sub_str_s13 = {
    .operation = INNER_STRLEN,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value_len",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &sub_str_s14,
    .is_static = true,
};

// POPS LF@value
static inner_statement_t sub_str_s12 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &sub_str_s13,
    .is_static = true,
};

// POPS LF@start
static inner_statement_t sub_str_s11 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@start",
        },
    .next_statement = &sub_str_s12,
    .is_static = true,
};

// POPS LF@end
static inner_statement_t sub_str_s10 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@end",
        },
    .next_statement = &sub_str_s11,
    .is_static = true,
};

// DEFVAR LF@return_val
static inner_statement_t sub_str_s9 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .next_statement = &sub_str_s10,
    .is_static = true,
};

// DEFVAR LF@character
static inner_statement_t sub_str_s8 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@character",
        },
    .next_statement = &sub_str_s9,
    .is_static = true,
};

// DEFVAR LF@precondition
static inner_statement_t sub_str_s7 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .next_statement = &sub_str_s8,
    .is_static = true,
};

// DEFVAR LF@value_len
static inner_statement_t sub_str_s6 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value_len",
        },
    .next_statement = &sub_str_s7,
    .is_static = true,
};

// DEFVAR LF@end
static inner_statement_t sub_str_s5 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@end",
        },
    .next_statement = &sub_str_s6,
    .is_static = true,
};

// DEFVAR LF@start
static inner_statement_t sub_str_s4 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@start",
        },
    .next_statement = &sub_str_s5,
    .is_static = true,
};

// DEFVAR LF@value
static inner_statement_t sub_str_s3 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &sub_str_s4,
    .is_static = true,
};

// PUSHFRAME
static inner_statement_t sub_str_s2 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &sub_str_s3,
    .is_static = true,
};

// CREATEFRAME
static inner_statement_t sub_str_s1 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &sub_str_s2,
    .is_static = true,
};

// LABEL subStr
inner_statement_t sub_str_code = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "subStr",
        },
    .next_statement = &sub_str_s1,
    .is_static = true,
};

/**
 * ord(value: String, index: Int): Int
 */

// RETURN
static inner_statement_t ord_s22 = {
    .operation = INNER_RETURN,
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t ord_s21 = {
    .operation = INNER_POPFRAME,
    .next_statement = &ord_s22,
    .is_static = true,
};

// PUSHS LF@return_val
static inner_statement_t ord_s20 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .next_statement = &ord_s21,
    .is_static = true,
};

// LABEL $$ord_end
static inner_statement_t ord_s19 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$ord_end",
        },
    .next_statement = &ord_s20,
    .is_static = true,
};

// MOVE LF@return_val int@-1
static inner_statement_t ord_s18 = {
    .operation = INNER_MOVE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .first_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = -1,
                },
        },
    .next_statement = &ord_s19,
    .is_static = true,
};

// LABEL $$ord_invalid_string
static inner_statement_t ord_s17 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$ord_invalid_string",
        },
    .next_statement = &ord_s18,
    .is_static = true,
};

// JUMP $$ord_end
static inner_statement_t ord_s16 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$ord_end",
        },
    .next_statement = &ord_s17,
    .is_static = true,
};

// STRI2INT LF@return_val LF@value LF@index
static inner_statement_t ord_s15 = {
    .operation = INNER_STRI2INT,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index",
        },
    .next_statement = &ord_s16,
    .is_static = true,
};

// JUMPIFEQ $$ord_invalid_string LF@precondition bool@false
static inner_statement_t ord_s14 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$ord_invalid_string",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_BOOL,
                    .value.inner_bool = false,
                },
        },
    .next_statement = &ord_s15,
    .is_static = true,
};

// LT LF@precondition LF@index LF@value_len
static inner_statement_t ord_s13 = {
    .operation = INNER_LT,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value_len",
        },
    .next_statement = &ord_s14,
    .is_static = true,
};

// JUMPIFEQ $$ord_invalid_string LF@precondition bool@true
static inner_statement_t ord_s12 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$ord_invalid_string",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_BOOL,
                    .value.inner_bool = true,
                },
        },
    .next_statement = &ord_s13,
    .is_static = true,
};

// LT LF@precondition LF@index int@0
static inner_statement_t ord_s11 = {
    .operation = INNER_LT,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 0,
                },
        },
    .next_statement = &ord_s12,
    .is_static = true,
};

// STRLEN LF@value_len LF@value
static inner_statement_t ord_s10 = {
    .operation = INNER_STRLEN,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value_len",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &ord_s11,
    .is_static = true,
};

// POPS LF@value
static inner_statement_t ord_s9 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &ord_s10,
    .is_static = true,
};

// POPS LF@index
static inner_statement_t ord_s8 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index",
        },
    .next_statement = &ord_s9,
    .is_static = true,
};

// DEFVAR LF@return_val
static inner_statement_t ord_s7 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .next_statement = &ord_s8,
    .is_static = true,
};

// DEFVAR LF@value_len
static inner_statement_t ord_s6 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value_len",
        },
    .next_statement = &ord_s7,
    .is_static = true,
};

// DEFVAR LF@precondition
static inner_statement_t ord_s5 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .next_statement = &ord_s6,
    .is_static = true,
};

// DEFVAR LF@index
static inner_statement_t ord_s4 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index",
        },
    .next_statement = &ord_s5,
    .is_static = true,
};

// DEFVAR LF@value
static inner_statement_t ord_s3 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &ord_s4,
    .is_static = true,
};

// PUSHFRAME
static inner_statement_t ord_s2 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &ord_s3,
    .is_static = true,
};

// CREATEFRAME
static inner_statement_t ord_s1 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &ord_s2,
    .is_static = true,
};

// LABEL ord
inner_statement_t ord_code = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "ord",
        },
    .next_statement = &ord_s1,
    .is_static = true,
};

/**
 * chr(code: Int): String
 */

// RETURN
static inner_statement_t chr_s9 = {
    .operation = INNER_RETURN,
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t chr_s8 = {
    .operation = INNER_POPFRAME,
    .next_statement = &chr_s9,
    .is_static = true,
};

// PUSHS LF@return_val
static inner_statement_t chr_s7 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .next_statement = &chr_s8,
    .is_static = true,
};

// INT2CHAR LF@return_val LF@code
static inner_statement_t chr_s6 = {
    .operation = INNER_INT2CHAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@code",
        },
    .next_statement = &chr_s7,
    .is_static = true,
};

// POPS LF@code
static inner_statement_t chr_s5 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@code",
        },
    .next_statement = &chr_s6,
    .is_static = true,
};

// DEFVAR LF@return_val
static inner_statement_t chr_s4 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .next_statement = &chr_s5,
    .is_static = true,
};

// DEFVAR LF@code
static inner_statement_t chr_s3 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@code",
        },
    .next_statement = &chr_s4,
    .is_static = true,
};

// PUSHFRAME
static inner_statement_t chr_s2 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &chr_s3,
    .is_static = true,
};

// CREATEFRAME
static inner_statement_t chr_s1 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &chr_s2,
    .is_static = true,
};

// LABEL chr
inner_statement_t chr_code = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "chr",
        },
    .next_statement = &chr_s1,
    .is_static = true,
};

/**
 * readBoolean(): Boolean
 */

// RETURN
static inner_statement_t readboolean_s7 = {
    .operation = INNER_RETURN,
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t readboolean_s6 = {
    .operation = INNER_POPFRAME,
    .next_statement = &readboolean_s7,
    .is_static = true,
};

// PUSHS LF@readed_value
static inner_statement_t readboolean_s5 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .next_statement = &readboolean_s6,
    .is_static = true,
};

// READ LF@readed_value bool
static inner_statement_t readboolean_s4 = {
    .operation = INNER_READ,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .first_op =
        {
            .type = INNER_SYM_ARR,
            .data.identificator = "bool",
        },
    .next_statement = &readboolean_s5,
    .is_static = true,
};

// DEFVAR LF@readed_value
static inner_statement_t readboolean_s3 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .next_statement = &readboolean_s4,
    .is_static = true,
};

// PUSHFRAME
static inner_statement_t readboolean_s2 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &readboolean_s3,
    .is_static = true,
};

// CREATEFRAME
static inner_statement_t readboolean_s1 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &readboolean_s2,
    .is_static = true,
};

// LABEL readBoolean
inner_statement_t readboolean_code = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "readBoolean",
        },
    .next_statement = &readboolean_s1,
    .is_static = true,
};

/**
 * readIntMaybe(): Int | Unit
 */

// RETURN
static inner_statement_t readint_maybe_s7 = {
    .operation = INNER_RETURN,
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t readint_maybe_s6 = {
    .operation = INNER_POPFRAME,
    .next_statement = &readint_maybe_s7,
    .is_static = true,
};

// PUSHS LF@readed_value
static inner_statement_t readint_maybe_s5 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .next_statement = &readint_maybe_s6,
    .is_static = true,
};

// READ LF@readed_value int
static inner_statement_t readint_maybe_s4 = {
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
    .next_statement = &readint_maybe_s5,
    .is_static = true,
};

// DEFVAR LF@readed_value
static inner_statement_t readint_maybe_s3 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .next_statement = &readint_maybe_s4,
    .is_static = true,
};

// PUSHFRAME
static inner_statement_t readint_maybe_s2 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &readint_maybe_s3,
    .is_static = true,
};

// CREATEFRAME
static inner_statement_t readint_maybe_s1 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &readint_maybe_s2,
    .is_static = true,
};

// LABEL readIntMaybe
inner_statement_t readint_maybe_code = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "readIntMaybe",
        },
    .next_statement = &readint_maybe_s1,
    .is_static = true,
};

/**
 * readDoubleMaybe(): Double | Unit
 */

// RETURN
static inner_statement_t readdouble_maybe_s7 = {
    .operation = INNER_RETURN,
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t readdouble_maybe_s6 = {
    .operation = INNER_POPFRAME,
    .next_statement = &readdouble_maybe_s7,
    .is_static = true,
};

// PUSHS LF@readed_value
static inner_statement_t readdouble_maybe_s5 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .next_statement = &readdouble_maybe_s6,
    .is_static = true,
};

// READ LF@readed_value float
static inner_statement_t readdouble_maybe_s4 = {
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
    .next_statement = &readdouble_maybe_s5,
    .is_static = true,
};

// DEFVAR LF@readed_value
static inner_statement_t readdouble_maybe_s3 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@readed_value",
        },
    .next_statement = &readdouble_maybe_s4,
    .is_static = true,
};

// PUSHFRAME
static inner_statement_t readdouble_maybe_s2 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &readdouble_maybe_s3,
    .is_static = true,
};

// CREATEFRAME
static inner_statement_t readdouble_maybe_s1 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &readdouble_maybe_s2,
    .is_static = true,
};

// LABEL readDoubleMaybe
inner_statement_t readdouble_maybe_code = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "readDoubleMaybe",
        },
    .next_statement = &readdouble_maybe_s1,
    .is_static = true,
};

/**
 * Double.floor: Double
 */

// RETURN
static inner_statement_t floor_s16 = {
    .operation = INNER_RETURN,
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t floor_s15 = {
    .operation = INNER_POPFRAME,
    .next_statement = &floor_s16,
    .is_static = true,
};

// PUSHS LF@return_val
static inner_statement_t floor_s14 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .next_statement = &floor_s15,
    .is_static = true,
};

// LABEL $$floor_end
static inner_statement_t floor_s13 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$floor_end",
        },
    .next_statement = &floor_s14,
    .is_static = true,
};
// SUB LF@return_val LF@return_val float@1.0
static inner_statement_t floor_s12 = {
    .operation = INNER_SUB,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_FLOAT,
                    .value.inner_float = 1.0,
                },
        },
    .next_statement = &floor_s13,
    .is_static = true,
};

// JUMPIFEQ $$floor_end LF@precondition bool@false
static inner_statement_t floor_s11 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$floor_end",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_BOOL,
                    .value.inner_bool = false,
                },
        },
    .next_statement = &floor_s12,
    .is_static = true,
};

// LT LF@precondition LF@value LF@return_val
static inner_statement_t floor_s10 = {
    .operation = INNER_LT,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .next_statement = &floor_s11,
    .is_static = true,
};

// INT2FLOAT LF@return_val LF@to_int
static inner_statement_t floor_s9 = {
    .operation = INNER_INT2FLOAT,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@to_int",
        },
    .next_statement = &floor_s10,
    .is_static = true,
};

// FLOAT2INT LF@to_int LF@value
static inner_statement_t floor_s8 = {
    .operation = INNER_FLOAT2INT,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@to_int",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &floor_s9,
    .is_static = true,
};

// POPS LF@value
static inner_statement_t floor_s7 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &floor_s8,
    .is_static = true,
};

// DEFVAR LF@precondition
static inner_statement_t floor_s6 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .next_statement = &floor_s7,
    .is_static = true,
};

// DEFVAR LF@return_val
static inner_statement_t floor_s5 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .next_statement = &floor_s6,
    .is_static = true,
};

// DEFVAR LF@to_int
static inner_statement_t floor_s4 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@to_int",
        },
    .next_statement = &floor_s5,
    .is_static = true,
};

// DEFVAR LF@value
static inner_statement_t floor_s3 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &floor_s4,
    .is_static = true,
};

// PUSHFRAME
static inner_statement_t floor_s2 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &floor_s3,
    .is_static = true,
};

// CREATEFRAME
static inner_statement_t floor_s1 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &floor_s2,
    .is_static = true,
};

// LABEL floor
inner_statement_t floor_code = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "floor",
        },
    .next_statement = &floor_s1,
    .is_static = true,
};