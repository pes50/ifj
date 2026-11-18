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
static inner_statement_t print_s22 = {
    .operation = INNER_RETURN,
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t print_s21 = {
    .operation = INNER_POPFRAME,
    .next_statement = &print_s22,
    .is_static = true,
};

// PUSHS nil@nil
static inner_statement_t print_s20 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_CONST,
            .data.constant.datatype = INNER_NIL,
        },
    .next_statement = &print_s21,
    .is_static = true,
};

// LABEL $$print_end
static inner_statement_t print_s19 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$print_end",
        },
    .next_statement = &print_s20,
    .is_static = true,
};

// WRITE LF@value
static inner_statement_t print_s18 = {
    .operation = INNER_WRITE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &print_s19,
    .is_static = true,
};

// POPS LF@value
static inner_statement_t print_s17 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &print_s18,
    .is_static = true,
};

// CALL toStr
static inner_statement_t print_s16 = {
    .operation = INNER_CALL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "toStr",
        },
    .next_statement = &print_s17,
    .is_static = true,
};

// PUSHS LF@value
static inner_statement_t print_s15 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &print_s16,
    .is_static = true,
};

// LABEL $$print_array
static inner_statement_t print_s14 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$print_array",
        },
    .next_statement = &print_s15,
    .is_static = true,
};

// JUMP $$print_end
static inner_statement_t print_s13 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$print_end",
        },
    .next_statement = &print_s14,
    .is_static = true,
};

// WRITE " () "   Print " () " in case Unit type was given.
static inner_statement_t print_s12 = {
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
    .next_statement = &print_s13,
    .is_static = true,
};

// LABEL $$print_unit
static inner_statement_t print_s11 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$print_unit",
        },
    .next_statement = &print_s12,
    .is_static = true,
};

// JUMP $$print_end
static inner_statement_t print_s10 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$print_end",
        },
    .next_statement = &print_s11,
    .is_static = true,
};

// WRITE LF@value
static inner_statement_t print_s9 = {
    .operation = INNER_WRITE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &print_s10,
    .is_static = true,
};

// JUMPIFEQ $$print_array LF@type string@array
static inner_statement_t print_s8 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$print_array",
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
                    .value.inner_string = "array",
                },
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
static inner_statement_t to_str_s54 = {
    .operation = INNER_RETURN,
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t to_str_s53 = {
    .operation = INNER_POPFRAME,
    .next_statement = &to_str_s54,
    .is_static = true,
};

// PUSHS LF@return_val
static inner_statement_t to_str_s52 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .next_statement = &to_str_s53,
    .is_static = true,
};

// LABEL $$to_str_end
static inner_statement_t to_str_s51 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_end",
        },
    .next_statement = &to_str_s52,
    .is_static = true,
};

// CONCAT LF@return_val LF@return_val string@)
static inner_statement_t to_str_s50 = {
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
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_STRING,
                    .value.inner_string = ")",
                },
        },
    .next_statement = &to_str_s51,
    .is_static = true,
};

// LABEL $$to_str_array_done
static inner_statement_t to_str_s49 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_array_done",
        },
    .next_statement = &to_str_s50,
    .is_static = true,
};

// JUMP $$to_str_array_loop
static inner_statement_t to_str_s48 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_array_loop",
        },
    .next_statement = &to_str_s49,
    .is_static = true,
};

// ADD LF@index LF@index int@1
static inner_statement_t to_str_s47 = {
    .operation = INNER_ADD,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index",
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
                    .value.inner_int = 1,
                },
        },
    .next_statement = &to_str_s48,
    .is_static = true,
};

// CONCAT LF@return_val LF@return_val LF@list_item_string
static inner_statement_t to_str_s46 = {
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
            .data.identificator = "LF@list_item_string",
        },
    .next_statement = &to_str_s47,
    .is_static = true,
};

// POPS LF@list_item_string
static inner_statement_t to_str_s45 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_item_string",
        },
    .next_statement = &to_str_s46,
    .is_static = true,
};

// CALL toStr
static inner_statement_t to_str_s44 = {
    .operation = INNER_CALL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "toStr",
        },
    .next_statement = &to_str_s45,
    .is_static = true,
};

// PUSHS LF@item
static inner_statement_t to_str_s43 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@item",
        },
    .next_statement = &to_str_s44,
    .is_static = true,
};

// ARRAYGET LF@item LF@value LF@index
static inner_statement_t to_str_s42 = {
    .operation = INNER_ARRAYGET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_item",
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
    .next_statement = &to_str_s43,
    .is_static = true,
};

// LABEL $$to_str_array_first
static inner_statement_t to_str_s41 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_array_first",
        },
    .next_statement = &to_str_s42,
    .is_static = true,
};

// CONCAT LF@return_val LF@return_val string@,
static inner_statement_t to_str_s40 = {
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
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_STRING,
                    .value.inner_string = ", ",
                },
        },
    .next_statement = &to_str_s41,
    .is_static = true,
};

// JUMPIFEQ $$to_str_array_first LF@index int@2
static inner_statement_t to_str_s39 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_array_first",
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
                    .value.inner_int = 2,
                },
        },
    .next_statement = &to_str_s40,
    .is_static = true,
};

// JUMPIFEQ $$to_str_array_done LF@index LF@size
static inner_statement_t to_str_s38 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_array_done",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@size",
        },
    .next_statement = &to_str_s39,
    .is_static = true,
};

// LABEL $$to_str_array_loop
static inner_statement_t to_str_s37 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_array_loop",
        },
    .next_statement = &to_str_s38,
    .is_static = true,
};

// MOVE LF@index int@2
static inner_statement_t to_str_s36 = {
    .operation = INNER_MOVE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index",
        },
    .first_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 2,
                },
        },
    .next_statement = &to_str_s37,
    .is_static = true,
};

// ARRAYGET LF@size LF@value int@1
static inner_statement_t to_str_s35 = {
    .operation = INNER_ARRAYGET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@size",
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
                    .datatype = INNER_INT,
                    .value.inner_int = 1,
                },
        },
    .next_statement = &to_str_s36,
    .is_static = true,
};

// MOVE LF@return_val string@List(
static inner_statement_t to_str_s34 = {
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
                    .value.inner_string = "List(",
                },
        },
    .next_statement = &to_str_s35,
    .is_static = true,
};

// LABEL $$to_str_array
static inner_statement_t to_str_s33 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_array",
        },
    .next_statement = &to_str_s34,
    .is_static = true,
};

// JUMP $$to_str_end
static inner_statement_t to_str_s32 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_end",
        },
    .next_statement = &to_str_s33,
    .is_static = true,
};

// MOVE LF@return_val string@ ()
static inner_statement_t to_str_s31 = {
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
    .next_statement = &to_str_s32,
    .is_static = true,
};

// LABEL $$to_str_nil
static inner_statement_t to_str_s30 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_nil",
        },
    .next_statement = &to_str_s31,
    .is_static = true,
};

// JUMP $$to_str_end
static inner_statement_t to_str_s29 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_end",
        },
    .next_statement = &to_str_s30,
    .is_static = true,
};

// MOVE LF@return_val string@true
static inner_statement_t to_str_s28 = {
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
    .next_statement = &to_str_s29,
    .is_static = true,
};

// JUMPIFNEQ $$to_str_end LF@value bool@true
static inner_statement_t to_str_s27 = {
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
    .next_statement = &to_str_s28,
    .is_static = true,
};

// MOVE LF@return_val string@false
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
                    .value.inner_string = "false",
                },
        },
    .next_statement = &to_str_s27,
    .is_static = true,
};

// LABEL $$to_str_bool
static inner_statement_t to_str_s25 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_bool",
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

// FLOAT2STR LF@retrn_val LF@value
static inner_statement_t to_str_s23 = {
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
    .next_statement = &to_str_s24,
    .is_static = true,
};

// LABEL $$to_str_float
static inner_statement_t to_str_s22 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_float",
        },
    .next_statement = &to_str_s23,
    .is_static = true,
};

// JUMP $$to_str_end
static inner_statement_t to_str_s21 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_end",
        },
    .next_statement = &to_str_s22,
    .is_static = true,
};

// INT2STR LF@return_val LF@value
static inner_statement_t to_str_s20 = {
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
    .next_statement = &to_str_s21,
    .is_static = true,
};

// LABEL $$to_str_int
static inner_statement_t to_str_s19 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_int",
        },
    .next_statement = &to_str_s20,
    .is_static = true,
};

// JUMP $$to_str_end
static inner_statement_t to_str_s18 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_end",
        },
    .next_statement = &to_str_s19,
    .is_static = true,
};

// MOVE LF@return_val LF@value
static inner_statement_t to_str_s17 = {
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
    .next_statement = &to_str_s18,
    .is_static = true,
};

// JUMPIFEQ $$to_str_array LF@type string@array
static inner_statement_t to_str_s16 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$to_str_array",
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
                    .value.inner_string = "array",
                },
        },
    .next_statement = &to_str_s17,
    .is_static = true,
};

// JUMPIFEQ $$to_str_nil LF@type string@nil
static inner_statement_t to_str_s15 = {
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
    .next_statement = &to_str_s16,
    .is_static = true,
};

// JUMPIFEQ $$to_str_bool LF@type string@bool
static inner_statement_t to_str_s14 = {
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
    .next_statement = &to_str_s15,
    .is_static = true,
};

// JUMPIFEQ $$to_str_float LF@type string@float
static inner_statement_t to_str_s13 = {
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
    .next_statement = &to_str_s14,
    .is_static = true,
};

// JUMPIFEQ $$to_str_int LF@type string@int
static inner_statement_t to_str_s12 = {
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
    .next_statement = &to_str_s13,
    .is_static = true,
};

// TYPE LF@type LF@value
static inner_statement_t to_str_s11 = {
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
    .next_statement = &to_str_s12,
    .is_static = true,
};

// POPS LF@value
static inner_statement_t to_str_s10 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@value",
        },
    .next_statement = &to_str_s11,
    .is_static = true,
};

// DEFVAR LF@list_item_string
static inner_statement_t to_str_s9 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_item_string",
        },
    .next_statement = &to_str_s10,
    .is_static = true,
};

// DEFVAR LF@list_item
static inner_statement_t to_str_s8 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_item",
        },
    .next_statement = &to_str_s9,
    .is_static = true,
};

// DEFVAR LF@index
static inner_statement_t to_str_s7 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index",
        },
    .next_statement = &to_str_s8,
    .is_static = true,
};

// DEFVAR LF@size
static inner_statement_t to_str_s6 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@size",
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

/**
 * head: T
 */

// RETURN
static inner_statement_t head_s8 = {
    .operation = INNER_RETURN,
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t head_s7 = {
    .operation = INNER_POPFRAME,
    .next_statement = &head_s8,
    .is_static = true,
};

// PUSHS LF@return_val
static inner_statement_t head_s6 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .next_statement = &head_s7,
    .is_static = true,
};

// ARRAYGET LF@return_val LF@list int@2
static inner_statement_t head_s5 = {
    .operation = INNER_ARRAYGET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 2,
                },
        },
    .next_statement = &head_s6,
    .is_static = true,
};

// POPS LF@list
static inner_statement_t head_s4 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
        },
    .next_statement = &head_s5,
    .is_static = true,
};

// DEFVAR LF@return_val
static inner_statement_t head_s3 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_val",
        },
    .next_statement = &head_s4,
    .is_static = true,
};

// DEFVAR LF@list
static inner_statement_t head_s2 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
        },
    .next_statement = &head_s3,
    .is_static = true,
};

// PUSHFRAME
static inner_statement_t head_s1 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &head_s2,
    .is_static = true,
};

// CREATEFRAME
static inner_statement_t head_s0 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &head_s1,
    .is_static = true,
};

// LABEL head
inner_statement_t head_code = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "head",
        },
    .next_statement = &head_s0,
    .is_static = true,
};

/**
 * slice(from, until): List[T]  (jadro, ostatni metody na nej skacou)
 */

// RETURN
static inner_statement_t slice_s49 = {
    .operation = INNER_RETURN,
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t slice_s48 = {
    .operation = INNER_POPFRAME,
    .next_statement = &slice_s49,
    .is_static = true,
};

// PUSHS LF@result
static inner_statement_t slice_s47 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@result",
        },
    .next_statement = &slice_s48,
    .is_static = true,
};

// LABEL $$slice_loop_done
static inner_statement_t slice_s46 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$slice_loop_done",
        },
    .next_statement = &slice_s47,
    .is_static = true,
};

// JUMP $$slice_loop
static inner_statement_t slice_s45 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$slice_loop",
        },
    .next_statement = &slice_s46,
    .is_static = true,
};

// ADD LF@index_1 LF@index_1 int@1
static inner_statement_t slice_s44 = {
    .operation = INNER_ADD,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index_1",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index_1",
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
    .next_statement = &slice_s45,
    .is_static = true,
};

// ARRAYSET LF@result LF@index_1 LF@list_item
static inner_statement_t slice_s43 = {
    .operation = INNER_ARRAYSET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@result",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index_1",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_item",
        },
    .next_statement = &slice_s44,
    .is_static = true,
};

// ARRAYGET LF@list_item LF@list LF@index_2
static inner_statement_t slice_s42 = {
    .operation = INNER_ARRAYGET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_item",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index_2",
        },
    .next_statement = &slice_s43,
    .is_static = true,
};

// ADD LF@index_2 LF@index_1 LF@start
static inner_statement_t slice_s41 = {
    .operation = INNER_ADD,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index_2",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index_1",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@start",
        },
    .next_statement = &slice_s42,
    .is_static = true,
};

// JUMPIFEQ $$slice_loop_done LF@index_1 LF@size
static inner_statement_t slice_s40 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$slice_loop_done",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index_1",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@size",
        },
    .next_statement = &slice_s41,
    .is_static = true,
};

// LABEL $$slice_loop
static inner_statement_t slice_s39 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$slice_loop",
        },
    .next_statement = &slice_s40,
    .is_static = true,
};

// MOVE LF@index_1 int@2
static inner_statement_t slice_s38 = {
    .operation = INNER_MOVE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index_1",
        },
    .first_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 2,
                },
        },
    .next_statement = &slice_s39,
    .is_static = true,
};

// ARRAYSET LF@result int@1 LF@size
static inner_statement_t slice_s37 = {
    .operation = INNER_ARRAYSET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@result",
        },
    .first_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 1,
                },
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@size",
        },
    .next_statement = &slice_s38,
    .is_static = true,
};

// ARRAYSET LF@result int@0 LF@list_type
static inner_statement_t slice_s36 = {
    .operation = INNER_ARRAYSET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@result",
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
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_type",
        },
    .next_statement = &slice_s37,
    .is_static = true,
};

// NEWARRAY LF@result LF@size
static inner_statement_t slice_s35 = {
    .operation = INNER_NEWARRAY,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@result",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@size",
        },
    .next_statement = &slice_s36,
    .is_static = true,
};

// ADD LF@size LF@counter int@2
static inner_statement_t slice_s34 = {
    .operation = INNER_ADD,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@size",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@counter",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 2,
                },
        },
    .next_statement = &slice_s35,
    .is_static = true,
};

// LABEL $$slice_build_list
static inner_statement_t slice_s33 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$slice_build_list",
        },
    .next_statement = &slice_s34,
    .is_static = true,
};

// MOVE LF@counter int@0
static inner_statement_t slice_s32 = {
    .operation = INNER_MOVE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@counter",
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
    .next_statement = &slice_s33,
    .is_static = true,
};

// JUMPIFEQ $$slice_build_list LF@precondition bool@true
static inner_statement_t slice_s31 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$slice_build_list",
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
    .next_statement = &slice_s32,
    .is_static = true,
};

// GT LF@precondition LF@counter int@0
static inner_statement_t slice_s30 = {
    .operation = INNER_GT,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@counter",
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
    .next_statement = &slice_s31,
    .is_static = true,
};

// SUB LF@counter LF@end LF@start
static inner_statement_t slice_s29 = {
    .operation = INNER_SUB,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@counter",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@end",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@start",
        },
    .next_statement = &slice_s30,
    .is_static = true,
};

// LABEL $$slice_end
static inner_statement_t slice_s28 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$slice_end",
        },
    .next_statement = &slice_s29,
    .is_static = true,
};

// MOVE LF@end LF@list_length
static inner_statement_t slice_s27 = {
    .operation = INNER_MOVE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@end",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_length",
        },
    .next_statement = &slice_s28,
    .is_static = true,
};

// JUMPIFEQ $$slice_end LF@precondition bool@false
static inner_statement_t slice_s26 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$slice_end",
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
    .next_statement = &slice_s27,
    .is_static = true,
};

// GT LF@precondition LF@end LF@list_length
static inner_statement_t slice_s25 = {
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
            .data.identificator = "LF@list_length",
        },
    .next_statement = &slice_s26,
    .is_static = true,
};

// LABEL $$slice_start
static inner_statement_t slice_s24 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$slice_start",
        },
    .next_statement = &slice_s25,
    .is_static = true,
};

// MOVE LF@start int@0
static inner_statement_t slice_s23 = {
    .operation = INNER_MOVE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@start",
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
    .next_statement = &slice_s24,
    .is_static = true,
};

// JUMPIFEQ $$slice_start LF@precondition bool@false
static inner_statement_t slice_s22 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$slice_start",
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
    .next_statement = &slice_s23,
    .is_static = true,
};

// LT LF@precondition LF@start int@0
static inner_statement_t slice_s21 = {
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
    .next_statement = &slice_s22,
    .is_static = true,
};

// SUB LF@list_length LF@list_length int@2
static inner_statement_t slice_s20 = {
    .operation = INNER_SUB,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_length",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_length",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 2,
                },
        },
    .next_statement = &slice_s21,
    .is_static = true,
};

// ARRAYGET LF@list_length LF@list int@1
static inner_statement_t slice_s19 = {
    .operation = INNER_ARRAYGET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_length",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
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
    .next_statement = &slice_s20,
    .is_static = true,
};

// ARRAYGET LF@list_type LF@list int@0
static inner_statement_t slice_s18 = {
    .operation = INNER_ARRAYGET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_type",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
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
    .next_statement = &slice_s19,
    .is_static = true,
};

// POPS LF@list
static inner_statement_t slice_s17 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
        },
    .next_statement = &slice_s18,
    .is_static = true,
};

// POPS LF@start
static inner_statement_t slice_s16 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@start",
        },
    .next_statement = &slice_s17,
    .is_static = true,
};

// POPS LF@end
static inner_statement_t slice_s15 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@end",
        },
    .next_statement = &slice_s16,
    .is_static = true,
};

// DEFVAR LF@list_item
static inner_statement_t slice_s14 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_item",
        },
    .next_statement = &slice_s15,
    .is_static = true,
};

// DEFVAR LF@index_2
static inner_statement_t slice_s13 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index_2",
        },
    .next_statement = &slice_s14,
    .is_static = true,
};

// DEFVAR LF@index_1
static inner_statement_t slice_s12 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index_1",
        },
    .next_statement = &slice_s13,
    .is_static = true,
};

// DEFVAR LF@result
static inner_statement_t slice_s11 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@result",
        },
    .next_statement = &slice_s12,
    .is_static = true,
};

// DEFVAR LF@size
static inner_statement_t slice_s10 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@size",
        },
    .next_statement = &slice_s11,
    .is_static = true,
};

// DEFVAR LF@counter
static inner_statement_t slice_s9 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@counter",
        },
    .next_statement = &slice_s10,
    .is_static = true,
};

// DEFVAR LF@precondition
static inner_statement_t slice_s8 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@precondition",
        },
    .next_statement = &slice_s9,
    .is_static = true,
};

// DEFVAR LF@list_length
static inner_statement_t slice_s7 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_length",
        },
    .next_statement = &slice_s8,
    .is_static = true,
};

// DEFVAR LF@list_type
static inner_statement_t slice_s6 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_type",
        },
    .next_statement = &slice_s7,
    .is_static = true,
};

// DEFVAR LF@end
static inner_statement_t slice_s5 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@end",
        },
    .next_statement = &slice_s6,
    .is_static = true,
};

// DEFVAR LF@start
static inner_statement_t slice_s4 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@start",
        },
    .next_statement = &slice_s5,
    .is_static = true,
};

// DEFVAR LF@list
static inner_statement_t slice_s3 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
        },
    .next_statement = &slice_s4,
    .is_static = true,
};

// PUSHFRAME
static inner_statement_t slice_s2 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &slice_s3,
    .is_static = true,
};

// CREATEFRAME
static inner_statement_t slice_s1 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &slice_s2,
    .is_static = true,
};

// LABEL slice
inner_statement_t slice_code = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "slice",
        },
    .next_statement = &slice_s1,
    .is_static = true,
};

/**
 * drop(n): List[T]
 */

// JUMP slice
static inner_statement_t drop_s14 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "slice",
        },
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t drop_s13 = {
    .operation = INNER_POPFRAME,
    .next_statement = &drop_s14,
    .is_static = true,
};

// PUSHS LF@list_length
static inner_statement_t drop_s12 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_length",
        },
    .next_statement = &drop_s13,
    .is_static = true,
};

// PUSHS LF@throw_away
static inner_statement_t drop_s11 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@throw_away",
        },
    .next_statement = &drop_s12,
    .is_static = true,
};

// PUSHS LF@list
static inner_statement_t drop_s10 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
        },
    .next_statement = &drop_s11,
    .is_static = true,
};

// SUB LF@list_length LF@list_length int@2
static inner_statement_t drop_s9 = {
    .operation = INNER_SUB,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_length",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_length",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 2,
                },
        },
    .next_statement = &drop_s10,
    .is_static = true,
};

// ARRAYGET LF@list_length LF@list int@1
static inner_statement_t drop_s8 = {
    .operation = INNER_ARRAYGET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_length",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
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
    .next_statement = &drop_s9,
    .is_static = true,
};

// POPS LF@list
static inner_statement_t drop_s7 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
        },
    .next_statement = &drop_s8,
    .is_static = true,
};

// POPS LF@throw_away
static inner_statement_t drop_s6 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@throw_away",
        },
    .next_statement = &drop_s7,
    .is_static = true,
};

// DEFVAR LF@list_length
static inner_statement_t drop_s5 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_length",
        },
    .next_statement = &drop_s6,
    .is_static = true,
};

// DEFVAR LF@throw_away
static inner_statement_t drop_s4 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@throw_away",
        },
    .next_statement = &drop_s5,
    .is_static = true,
};

// DEFVAR LF@list
static inner_statement_t drop_s3 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
        },
    .next_statement = &drop_s4,
    .is_static = true,
};

// PUSHFRAME
static inner_statement_t drop_s2 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &drop_s3,
    .is_static = true,
};

// CREATEFRAME
static inner_statement_t drop_s1 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &drop_s2,
    .is_static = true,
};

// LABEL drop
inner_statement_t drop_code = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "drop",
        },
    .next_statement = &drop_s1,
    .is_static = true,
};

/**
 * take(n): List[T]
 */

// JUMP slice
static inner_statement_t take_s11 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "slice",
        },
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t take_s10 = {
    .operation = INNER_POPFRAME,
    .next_statement = &take_s11,
    .is_static = true,
};

// PUSHS LF@max_num
static inner_statement_t take_s9 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@max_num",
        },
    .next_statement = &take_s10,
    .is_static = true,
};

// PUSHS int@0
static inner_statement_t take_s8 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 0,
                },
        },
    .next_statement = &take_s9,
    .is_static = true,
};

// PUSHS LF@list
static inner_statement_t take_s7 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
        },
    .next_statement = &take_s8,
    .is_static = true,
};

// POPS LF@list
static inner_statement_t take_s6 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
        },
    .next_statement = &take_s7,
    .is_static = true,
};

// POPS LF@max_num
static inner_statement_t take_s5 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@max_num",
        },
    .next_statement = &take_s6,
    .is_static = true,
};

// DEFVAR LF@max_num
static inner_statement_t take_s4 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@max_num",
        },
    .next_statement = &take_s5,
    .is_static = true,
};

// DEFVAR LF@list
static inner_statement_t take_s3 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
        },
    .next_statement = &take_s4,
    .is_static = true,
};

// PUSHFRAME
static inner_statement_t take_s2 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &take_s3,
    .is_static = true,
};

// CREATEFRAME
static inner_statement_t take_s1 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &take_s2,
    .is_static = true,
};

// LABEL take
inner_statement_t take_code = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "take",
        },
    .next_statement = &take_s1,
    .is_static = true,
};

/**
 * tail: List[T]
 */

// JUMP slice
static inner_statement_t tail_s14 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "slice",
        },
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t tail_s13 = {
    .operation = INNER_POPFRAME,
    .next_statement = &tail_s14,
    .is_static = true,
};

// PUSHS LF@list_length
static inner_statement_t tail_s12 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_length",
        },
    .next_statement = &tail_s13,
    .is_static = true,
};

// PUSHS int@1
static inner_statement_t tail_s11 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 1,
                },
        },
    .next_statement = &tail_s12,
    .is_static = true,
};

// PUSHS LF@list
static inner_statement_t tail_s10 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
        },
    .next_statement = &tail_s11,
    .is_static = true,
};

// SUB LF@list_length LF@list_length int@2
static inner_statement_t tail_s9 = {
    .operation = INNER_SUB,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_length",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_length",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 2,
                },
        },
    .next_statement = &tail_s10,
    .is_static = true,
};

// ARRAYGET LF@list_length LF@list int@1
static inner_statement_t tail_s8 = {
    .operation = INNER_ARRAYGET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_length",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
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
    .next_statement = &tail_s9,
    .is_static = true,
};

// ARRAYGET LF@first LF@list int@2
static inner_statement_t tail_s7 = {
    .operation = INNER_ARRAYGET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@first",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
        },
    .second_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 2,
                },
        },
    .next_statement = &tail_s8,
    .is_static = true,
};

// POPS LF@list
static inner_statement_t tail_s6 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
        },
    .next_statement = &tail_s7,
    .is_static = true,
};

// DEFVAR LF@first
static inner_statement_t tail_s5 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@first",
        },
    .next_statement = &tail_s6,
    .is_static = true,
};

// DEFVAR LF@list_length
static inner_statement_t tail_s4 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_length",
        },
    .next_statement = &tail_s5,
    .is_static = true,
};

// DEFVAR LF@list
static inner_statement_t tail_s3 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
        },
    .next_statement = &tail_s4,
    .is_static = true,
};

// PUSHFRAME
static inner_statement_t tail_s2 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &tail_s3,
    .is_static = true,
};

// CREATEFRAME
static inner_statement_t tail_s1 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &tail_s2,
    .is_static = true,
};

// LABEL tail
inner_statement_t tail_code = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "tail",
        },
    .next_statement = &tail_s1,
    .is_static = true,
};

/**
 * filter((T) => Boolean)
 */

// RETURN
static inner_statement_t filter_s46 = {
    .operation = INNER_RETURN,
    .next_statement = NULL,
    .is_static = true,
};

// POPFRAME
static inner_statement_t filter_s45 = {
    .operation = INNER_POPFRAME,
    .next_statement = &filter_s46,
    .is_static = true,
};

// PUSHS LF@return_list
static inner_statement_t filter_s44 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_list",
        },
    .next_statement = &filter_s45,
    .is_static = true,
};

// LABEL $$filter_done
static inner_statement_t filter_s43 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$filter_done",
        },
    .next_statement = &filter_s44,
    .is_static = true,
};

// JUMP $$filter_copy
static inner_statement_t filter_s42 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$filter_copy",
        },
    .next_statement = &filter_s43,
    .is_static = true,
};

// ADD LF@index LF@index int@1
static inner_statement_t filter_s41 = {
    .operation = INNER_ADD,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index",
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
                    .value.inner_int = 1,
                },
        },
    .next_statement = &filter_s42,
    .is_static = true,
};

// ARRAYSET LF@return_list LF@index LF@list_item
static inner_statement_t filter_s40 = {
    .operation = INNER_ARRAYSET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_list",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_item",
        },
    .next_statement = &filter_s41,
    .is_static = true,
};

// ARRAYGET LF@list_item LF@list_copied_with_cond LF@index
static inner_statement_t filter_s39 = {
    .operation = INNER_ARRAYGET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_item",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_copied_with_cond",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index",
        },
    .next_statement = &filter_s40,
    .is_static = true,
};

// JUMPIFEQ $$filter_done LF@index LF@out_size
static inner_statement_t filter_s38 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$filter_done",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@out_size",
        },
    .next_statement = &filter_s39,
    .is_static = true,
};

// LABEL $$filter_copy
static inner_statement_t filter_s37 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$filter_copy",
        },
    .next_statement = &filter_s38,
    .is_static = true,
};

// MOVE LF@index int@2
static inner_statement_t filter_s36 = {
    .operation = INNER_MOVE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index",
        },
    .first_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 2,
                },
        },
    .next_statement = &filter_s37,
    .is_static = true,
};

// ARRAYSET LF@return_list int@1 LF@out_size
static inner_statement_t filter_s35 = {
    .operation = INNER_ARRAYSET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_list",
        },
    .first_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 1,
                },
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@out_size",
        },
    .next_statement = &filter_s36,
    .is_static = true,
};

// ARRAYSET LF@return_list int@0 LF@list_type
static inner_statement_t filter_s34 = {
    .operation = INNER_ARRAYSET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_list",
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
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_type",
        },
    .next_statement = &filter_s35,
    .is_static = true,
};

// NEWARRAY LF@return_list LF@out_size
static inner_statement_t filter_s33 = {
    .operation = INNER_NEWARRAY,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_list",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@out_size",
        },
    .next_statement = &filter_s34,
    .is_static = true,
};

// LABEL $$filter_loop_done
static inner_statement_t filter_s32 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$filter_loop_done",
        },
    .next_statement = &filter_s33,
    .is_static = true,
};

// JUMP $$filter_loop
static inner_statement_t filter_s31 = {
    .operation = INNER_JUMP,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$filter_loop",
        },
    .next_statement = &filter_s32,
    .is_static = true,
};

// ADD LF@index LF@index int@1
static inner_statement_t filter_s30 = {
    .operation = INNER_ADD,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index",
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
                    .value.inner_int = 1,
                },
        },
    .next_statement = &filter_s31,
    .is_static = true,
};

// LABEL $$filter_next
static inner_statement_t filter_s29 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$filter_next",
        },
    .next_statement = &filter_s30,
    .is_static = true,
};

// ADD LF@out_size LF@out_size int@1
static inner_statement_t filter_s28 = {
    .operation = INNER_ADD,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@out_size",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@out_size",
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
    .next_statement = &filter_s29,
    .is_static = true,
};

// ARRAYSET LF@list_copied_with_cond LF@out_size LF@list_item
static inner_statement_t filter_s27 = {
    .operation = INNER_ARRAYSET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_copied_with_cond",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@out_size",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_item",
        },
    .next_statement = &filter_s28,
    .is_static = true,
};

// JUMPIFEQ $$filter_next LF@list_item_filtered bool@false
static inner_statement_t filter_s26 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$filter_next",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_item_filtered",
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
    .next_statement = &filter_s27,
    .is_static = true,
};

// POPS LF@list_item_filtered
static inner_statement_t filter_s25 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_item_filtered",
        },
    .next_statement = &filter_s26,
    .is_static = true,
};

// CALL LF@condition
static inner_statement_t filter_s24 = {
    .operation = INNER_CALL,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@condition",
        },
    .next_statement = &filter_s25,
    .is_static = true,
};

// PUSHS LF@list_item
static inner_statement_t filter_s23 = {
    .operation = INNER_PUSHS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_item",
        },
    .next_statement = &filter_s24,
    .is_static = true,
};

// ARRAYGET LF@list_item LF@list LF@index
static inner_statement_t filter_s22 = {
    .operation = INNER_ARRAYGET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_item",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index",
        },
    .next_statement = &filter_s23,
    .is_static = true,
};

// JUMPIFEQ $$filter_loop_done LF@index LF@size
static inner_statement_t filter_s21 = {
    .operation = INNER_JUMPIFEQ,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$filter_loop_done",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index",
        },
    .second_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@size",
        },
    .next_statement = &filter_s22,
    .is_static = true,
};

// LABEL $$filter_loop
static inner_statement_t filter_s20 = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "$$filter_loop",
        },
    .next_statement = &filter_s21,
    .is_static = true,
};

// MOVE LF@out_size int@2
static inner_statement_t filter_s19 = {
    .operation = INNER_MOVE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@out_size",
        },
    .first_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 2,
                },
        },
    .next_statement = &filter_s20,
    .is_static = true,
};

// MOVE LF@index int@2
static inner_statement_t filter_s18 = {
    .operation = INNER_MOVE,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index",
        },
    .first_op =
        {
            .type = INNER_SYM_CONST,
            .data.constant =
                {
                    .datatype = INNER_INT,
                    .value.inner_int = 2,
                },
        },
    .next_statement = &filter_s19,
    .is_static = true,
};

// NEWARRAY LF@list_copied_with_cond LF@size
static inner_statement_t filter_s17 = {
    .operation = INNER_NEWARRAY,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_copied_with_cond",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@size",
        },
    .next_statement = &filter_s18,
    .is_static = true,
};

// ARRAYGET LF@size LF@list int@1
static inner_statement_t filter_s16 = {
    .operation = INNER_ARRAYGET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@size",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
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
    .next_statement = &filter_s17,
    .is_static = true,
};

// ARRAYGET LF@list_type LF@list int@0
static inner_statement_t filter_s15 = {
    .operation = INNER_ARRAYGET,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_type",
        },
    .first_op =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
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
    .next_statement = &filter_s16,
    .is_static = true,
};

// POPS LF@list
static inner_statement_t filter_s14 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
        },
    .next_statement = &filter_s15,
    .is_static = true,
};

// POPS LF@condition
static inner_statement_t filter_s13 = {
    .operation = INNER_POPS,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@condition",
        },
    .next_statement = &filter_s14,
    .is_static = true,
};

// DEFVAR LF@return_list
static inner_statement_t filter_s12 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@return_list",
        },
    .next_statement = &filter_s13,
    .is_static = true,
};

// DEFVAR LF@list_item_filtered
static inner_statement_t filter_s11 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_item_filtered",
        },
    .next_statement = &filter_s12,
    .is_static = true,
};

// DEFVAR LF@list_item
static inner_statement_t filter_s10 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_item",
        },
    .next_statement = &filter_s11,
    .is_static = true,
};

// DEFVAR LF@out_size
static inner_statement_t filter_s9 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@out_size",
        },
    .next_statement = &filter_s10,
    .is_static = true,
};

// DEFVAR LF@index
static inner_statement_t filter_s8 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@index",
        },
    .next_statement = &filter_s9,
    .is_static = true,
};

// DEFVAR LF@list_copied_with_cond
static inner_statement_t filter_s7 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_copied_with_cond",
        },
    .next_statement = &filter_s8,
    .is_static = true,
};

// DEFVAR LF@size
static inner_statement_t filter_s6 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@size",
        },
    .next_statement = &filter_s7,
    .is_static = true,
};

// DEFVAR LF@list_type
static inner_statement_t filter_s5 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list_type",
        },
    .next_statement = &filter_s6,
    .is_static = true,
};

// DEFVAR LF@condition
static inner_statement_t filter_s4 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@condition",
        },
    .next_statement = &filter_s5,
    .is_static = true,
};

// DEFVAR LF@list
static inner_statement_t filter_s3 = {
    .operation = INNER_DEFVAR,
    .destination =
        {
            .type = INNER_SYM_VAR,
            .data.identificator = "LF@list",
        },
    .next_statement = &filter_s4,
    .is_static = true,
};

// PUSHFRAME
static inner_statement_t filter_s2 = {
    .operation = INNER_PUSHFRAME,
    .next_statement = &filter_s3,
    .is_static = true,
};

// CREATEFRAME
static inner_statement_t filter_s1 = {
    .operation = INNER_CREATEFRAME,
    .next_statement = &filter_s2,
    .is_static = true,
};

// LABEL filter
inner_statement_t filter_code = {
    .operation = INNER_LABEL,
    .destination =
        {
            .type = INNER_SYM_LABEL,
            .data.identificator = "filter",
        },
    .next_statement = &filter_s1,
    .is_static = true,
};
