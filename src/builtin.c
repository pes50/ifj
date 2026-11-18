/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file builtin.c
 *
 * @authors     Roman Skoupý      (xskoupr00)
 * @authors     Simon Šmíd        (xsmidsi00)
 *
 * @brief Implementation of create_builtin_symtable function
 *
\*****************************************************************************/

#include "builtin.h"
#include "ast_node.h"
#include "builtin_code.h"

#define builtin_table_size 200

#pragma region Types

static ast_node_t int_type_declaration = {
    .type = AST_TYPE_DECLARATION,
    .data.type_declaration =
        {
            .name = "Int",
            .data_type = &node_int,
        },
    .data_type = &node_int,
    .next = NULL,
};

static ast_node_t double_type_declaration = {
    .type = AST_TYPE_DECLARATION,
    .data.type_declaration =
        {
            .name = "Double",
            .data_type = &node_double,
        },
    .data_type = &node_double,
    .next = NULL,
};

static ast_node_t string_type_declaration = {
    .type = AST_TYPE_DECLARATION,
    .data.type_declaration =
        {
            .name = "String",
            .data_type = &node_string,
        },
    .data_type = &node_string,
    .next = NULL,
};

static ast_node_t unit_type_declaration = {
    .type = AST_TYPE_DECLARATION,
    .data.type_declaration =
        {
            .name = "Unit",
            .data_type = &node_unit,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static ast_node_t boolean_type_declaration = {
    .type = AST_TYPE_DECLARATION,
    .data.type_declaration =
        {
            .name = "Boolean",
            .data_type = &node_boolean,
        },
    .data_type = &node_boolean,
    .next = NULL,
};

static ast_node_t any_type_declaration = {
    .type = AST_TYPE_DECLARATION,
    .data.type_declaration =
        {
            .name = "Any",
            .data_type = &node_any,
        },
    .data_type = &node_any,
    .next = NULL,
};

#pragma endregion

#pragma region Functions

static ast_node_t parameter_list_empty = {
    .type = AST_PARAMETER_LIST,
    .data =
        {
            .parameter_list = {NULL},
        },
    .data_type = &node_unit,
    .next = NULL,
};

#pragma region print(value: Any): Unit

static ast_node_t print_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &print_code},
    .next = NULL,
};

static ast_node_t value_any_parameter = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "value",
            .data_type = &node_any,
        },
    .data_type = &node_any,
    .next = NULL,
};

static ast_node_t any_value_parameter_list = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &value_any_parameter,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static data_type_node_t any_unit_signature = {
    .type = DATA_TYPE_LAMBDA,
    .data.lambda_type =
        {
            .parameter_types = &node_any,
            .return_type = &node_unit,
        },
    .next = NULL,
};

static ast_node_t print_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &print_body,
            .is_main = false,
            .name = "print",
            .return_type = &node_unit,
            .parameter_lists = &any_value_parameter_list,
        },
    .data_type = &any_unit_signature,
    .next = NULL,
};

#pragma endregion

#pragma region readLine(): String

static ast_node_t readline_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &readline_code},
    .next = NULL,
};

static data_type_node_t empty_string_signature = {
    .type = DATA_TYPE_LAMBDA,
    .data.lambda_type =
        {
            .parameter_types = NULL,
            .return_type = &node_string,
        },
    .next = NULL,
};

static ast_node_t read_line_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &readline_body,
            .is_main = false,
            .name = "readLine",
            .return_type = &node_string,
            .parameter_lists = &parameter_list_empty,
        },
    .data_type = &empty_string_signature,
    .next = NULL,
};

#pragma endregion

#pragma region readInt(): Int

static ast_node_t readint_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &readint_code},
    .next = NULL,
};

static data_type_node_t empty_int_signature = {
    .type = DATA_TYPE_LAMBDA,
    .data.lambda_type =
        {
            .parameter_types = NULL,
            .return_type = &node_int,
        },
    .next = NULL,
};

static ast_node_t read_int_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &readint_body,
            .is_main = false,
            .name = "readInt",
            .return_type = &node_int,
            .parameter_lists = &parameter_list_empty,
        },
    .data_type = &empty_int_signature,
    .next = NULL,
};

#pragma endregion

#pragma region readDouble(): Double

static ast_node_t read_double_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &readdouble_code},
    .next = NULL,
};

static data_type_node_t empty_double_signature = {
    .type = DATA_TYPE_LAMBDA,
    .data.lambda_type =
        {
            .parameter_types = NULL,
            .return_type = &node_double,
        },
    .next = NULL,
};

static ast_node_t read_double_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &read_double_body,
            .is_main = false,
            .name = "readDouble",
            .return_type = &node_double,
            .parameter_lists = &parameter_list_empty,
        },
    .data_type = &empty_double_signature,
    .next = NULL,
};

#pragma endregion

#pragma region toStr(value: Any) String

static ast_node_t to_str_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &to_str_code},
    .next = NULL,
};

static data_type_node_t any_string_signature = {
    .type = DATA_TYPE_LAMBDA,
    .data.lambda_type =
        {
            .parameter_types = &node_any,
            .return_type = &node_string,
        },
    .next = NULL,
};

static ast_node_t to_str_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &to_str_body,
            .is_main = false,
            .name = "toStr",
            .return_type = &node_string,
            .parameter_lists = &any_value_parameter_list,
        },
    .data_type = &any_string_signature,
    .next = NULL,
};

#pragma endregion

#pragma region toInt(value: Double): Int

static ast_node_t to_int_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &to_int_code},
    .next = NULL,
};

static ast_node_t double_value_parameter = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "value",
            .data_type = &node_double,
        },
    .data_type = &node_double,
    .next = NULL,
};

static ast_node_t double_value_parameter_list = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &double_value_parameter,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static data_type_node_t double_int_signature = {
    .type = DATA_TYPE_LAMBDA,
    .data.lambda_type =
        {
            .parameter_types = &node_double,
            .return_type = &node_int,
        },
    .next = NULL,
};

static ast_node_t to_int_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &to_int_body,
            .is_main = false,
            .name = "toInt",
            .return_type = &node_int,
            .parameter_lists = &double_value_parameter_list,
        },
    .data_type = &double_int_signature,
    .next = NULL,
};

#pragma endregion

#pragma region strLength(value : String) : Int

static ast_node_t str_length_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &str_length_code},
    .next = NULL,
};

static ast_node_t str_length_parameter = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "value",
            .data_type = &node_string,
        },
    .data_type = &node_string,
    .next = NULL,
};

static ast_node_t str_length_parameter_list = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &str_length_parameter,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static data_type_node_t string_int_signature = {
    .type = DATA_TYPE_LAMBDA,
    .data.lambda_type =
        {
            .parameter_types = &node_string,
            .return_type = &node_int,
        },
    .next = NULL,
};

static ast_node_t str_length_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &str_length_body,
            .is_main = false,
            .name = "strLength",
            .return_type = &node_int,
            .parameter_lists = &str_length_parameter_list,
        },
    .data_type = &string_int_signature,
    .next = NULL,
};

#pragma endregion

#pragma region subStr(value: String, start: Int, end: Int): String

static ast_node_t sub_str_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &sub_str_code},
    .next = NULL,
};

static ast_node_t sub_str_end_parameter = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "end",
            .data_type = &node_int,
        },
    .data_type = &node_int,
    .next = NULL,
};

static ast_node_t sub_str_start_parameter = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "start",
            .data_type = &node_int,
        },
    .data_type = &node_int,
    .next = &sub_str_end_parameter,
};

static ast_node_t sub_str_value_parameter = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "value",
            .data_type = &node_string,
        },
    .data_type = &node_string,
    .next = &sub_str_start_parameter,
};

static ast_node_t sub_str_parameter_list = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &sub_str_value_parameter,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static data_type_node_t end_int = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_INT,
    .next = NULL,
};

static data_type_node_t start_int = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_INT,
    .next = &end_int,
};

static data_type_node_t value_string = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_STRING,
    .next = &start_int,
};

static data_type_node_t string_int_int_string_signature = {
    .type = DATA_TYPE_LAMBDA,
    .data.lambda_type =
        {
            .parameter_types = &value_string,
            .return_type = &node_string,
        },
    .next = NULL,
};

static ast_node_t sub_str_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &sub_str_body,
            .is_main = false,
            .name = "subStr",
            .return_type = &node_string,
            .parameter_lists = &sub_str_parameter_list,
        },
    .data_type = &string_int_int_string_signature,
    .next = NULL,
};

#pragma endregion

#pragma region ord(value: String, index: Int): Int

static ast_node_t ord_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &ord_code},
    .next = NULL,
};

static ast_node_t ord_index_parameter = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "index",
            .data_type = &node_int,
        },
    .data_type = &node_int,
    .next = NULL,
};

static ast_node_t ord_value_parameter = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "value",
            .data_type = &node_string,
        },
    .data_type = &node_string,
    .next = &ord_index_parameter,
};

static ast_node_t ord_parameter_list = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &ord_value_parameter,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static data_type_node_t ord_index_int = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_INT,
    .next = NULL,
};

static data_type_node_t ord_value_string = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_STRING,
    .next = &ord_index_int,
};

static data_type_node_t string_int_int_signature = {
    .type = DATA_TYPE_LAMBDA,
    .data.lambda_type =
        {
            .parameter_types = &ord_value_string,
            .return_type = &node_int,
        },
    .next = NULL,
};

static ast_node_t ord_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &ord_body,
            .is_main = false,
            .name = "ord",
            .return_type = &node_int,
            .parameter_lists = &ord_parameter_list,
        },
    .data_type = &string_int_int_signature,
    .next = NULL,
};

#pragma endregion

#pragma region chr(code: Int): String

static ast_node_t chr_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &chr_code},
    .next = NULL,
};

static ast_node_t chr_parameter = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "code",
            .data_type = &node_int,
        },
    .data_type = &node_int,
    .next = NULL,
};

static ast_node_t chr_parameter_list = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &chr_parameter,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static data_type_node_t int_string_signature = {
    .type = DATA_TYPE_LAMBDA,
    .data.lambda_type =
        {
            .parameter_types = &node_int,
            .return_type = &node_string,
        },
    .next = NULL,
};

static ast_node_t chr_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &chr_body,
            .is_main = false,
            .name = "chr",
            .return_type = &node_string,
            .parameter_lists = &chr_parameter_list,
        },
    .data_type = &int_string_signature,
    .next = NULL,
};

#pragma endregion

#pragma region readBoolean(): Boolean

static ast_node_t readboolean_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &readboolean_code},
    .next = NULL,
};

static data_type_node_t empty_boolean_signature = {
    .type = DATA_TYPE_LAMBDA,
    .data.lambda_type =
        {
            .parameter_types = NULL,
            .return_type = &node_boolean,
        },
    .next = NULL,
};

static ast_node_t read_boolean_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &readboolean_body,
            .is_main = false,
            .name = "readBoolean",
            .return_type = &node_boolean,
            .parameter_lists = &parameter_list_empty,
        },
    .data_type = &empty_boolean_signature,
    .next = NULL,
};

#pragma endregion

#pragma region Any.toString: String

static ast_node_t target_any_parameter_node = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "$target",
            .data_type = &node_any,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static ast_node_t target_any_parameter_list_node = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &target_any_parameter_node,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static ast_node_t any_to_string_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &to_str_body,
            .is_main = false,
            .name = "Any.toString",
            .return_type = &node_string,
            .parameter_lists = &target_any_parameter_list_node,
        },
    .data_type = &node_curried_function,
    .next = NULL,
};

#pragma endregion

#pragma region Double.toInt: Int

static ast_node_t target_double_parameter_node = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "$target",
            .data_type = &node_double,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static ast_node_t target_double_parameter_list_node = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &target_double_parameter_node,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static ast_node_t double_to_int_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &to_int_body,
            .is_main = false,
            .name = "Double.toInt",
            .return_type = &node_int,
            .parameter_lists = &target_double_parameter_list_node,
        },
    .data_type = &node_curried_function,
    .next = NULL,
};

#pragma endregion

#pragma region Double.floor: Double

static ast_node_t floor_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &floor_code},
    .next = NULL,
};

static ast_node_t double_floor_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &floor_body,
            .is_main = false,
            .name = "Double.floor",
            .return_type = &node_double,
            .parameter_lists = &target_double_parameter_list_node,
        },
    .data_type = &node_curried_function,
    .next = NULL,
};

#pragma endregion

#pragma region String.length: Int

static ast_node_t target_string_parameter_node = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "$target",
            .data_type = &node_string,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static ast_node_t target_string_parameter_list_node = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &target_string_parameter_node,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static ast_node_t string_length_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &str_length_body,
            .is_main = false,
            .name = "String.length",
            .return_type = &node_int,
            .parameter_lists = &target_string_parameter_list_node,
        },
    .data_type = &node_curried_function,
    .next = NULL,
};

#pragma endregion

static data_type_node_t union_unit = {
    .type = DATA_TYPE_UNIT,
    .next = NULL,
};

#pragma region readIntMaybe() : Int | Unit

static ast_node_t readint_maybe_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &readint_maybe_code},
    .next = NULL,
};

static data_type_node_t union_int = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_INT,
    .next = &union_unit,
};

static data_type_node_t int_unit_node = {
    .type = DATA_TYPE_UNION,
    .data.union_type =
        {
            .types = &union_int,
        },
    .next = NULL,
};

static data_type_node_t empty_int_unit_signature = {
    .type = DATA_TYPE_LAMBDA,
    .data.lambda_type =
        {
            .parameter_types = NULL,
            .return_type = &int_unit_node,
        },
    .next = NULL,
};

static ast_node_t read_int_maybe_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &readint_maybe_body,
            .is_main = false,
            .name = "readIntMaybe",
            .return_type = &int_unit_node,
            .parameter_lists = &parameter_list_empty,
        },
    .data_type = &empty_int_unit_signature,
    .next = NULL,
};

#pragma endregion

#pragma region readDoubleMaybe() : Double | Unit

static ast_node_t readdouble_maybe_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &readdouble_maybe_code},
    .next = NULL,
};

static data_type_node_t union_double = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_DOUBLE,
    .next = &union_unit,
};

static data_type_node_t double_unit_node = {
    .type = DATA_TYPE_UNION,
    .data.union_type =
        {
            .types = &union_double,
        },
    .next = NULL,
};

static data_type_node_t empty_double_unit_signature = {
    .type = DATA_TYPE_LAMBDA,
    .data.lambda_type =
        {
            .parameter_types = NULL,
            .return_type = &double_unit_node,
        },
    .next = NULL,
};

static ast_node_t read_double_maybe_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &readdouble_maybe_body,
            .is_main = false,
            .name = "readDoubleMaybe",
            .return_type = &double_unit_node,
            .parameter_lists = &parameter_list_empty,
        },
    .data_type = &empty_double_unit_signature,
    .next = NULL,
};

#pragma endregion

static ast_node_t n_int_parameter_node = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "n",
            .data_type = &node_int,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static ast_node_t n_int_parameter_list_node = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &n_int_parameter_node,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static ast_node_t target_int_parameter_node = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "$target",
            .data_type = &node_int,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static ast_node_t target_int_parameter_list_node = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &target_int_parameter_node,
        },
    .data_type = &node_unit,
    .next = &n_int_parameter_list_node,
};

#pragma region to(n: Int) : $Range

static ast_node_t to_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "Int.to",
            .return_type = &node_range,
            .parameter_lists = &target_int_parameter_list_node,
        },
    .data_type = &node_curried_function,
    .next = NULL,
};

#pragma endregion

#pragma region until(n: Int) : $Range

static ast_node_t until_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "Int.until",
            .return_type = &node_range,
            .parameter_lists = &target_int_parameter_list_node,
        },
    .data_type = &node_curried_function,
    .next = NULL,
};

#pragma endregion

static ast_node_t target_range_parameter_node = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "$target",
            .data_type = &node_range,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static ast_node_t target_range_parameter_list_node = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &target_range_parameter_node,
        },
    .data_type = &node_unit,
    .next = &n_int_parameter_list_node,
};

#pragma region by(n: Int) : $Range

static ast_node_t by_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "$Range.by",
            .return_type = &node_range,
            .parameter_lists = &target_range_parameter_list_node,
        },
    .data_type = &node_curried_function,
    .next = NULL,
};

#pragma endregion

#pragma region List(v1, v2, ...) : List[T]

static ast_node_t list_constructor = {
    .type = AST_LIST_CONSTRUCTOR,
    .data_type = &generic_list_node,
    .next = NULL,
};

#pragma endregion

static ast_node_t target_list_parameter_node = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "$target",
            .data_type = &generic_list_node,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static ast_node_t target_list_n_parameter_list_node = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &target_list_parameter_node,
        },
    .data_type = &node_unit,
    .next = &n_int_parameter_list_node,
};

#pragma region drop(n): List[T]

static ast_node_t drop_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &drop_code},
    .next = NULL,
};

static ast_node_t drop_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &drop_body,
            .is_main = false,
            .name = "List[T].drop",
            .return_type = &generic_list_node,
            .parameter_lists = &target_list_n_parameter_list_node,
        },
    .data_type = &node_curried_function,
    .next = NULL,
};

#pragma endregion

#pragma region take(n): List[T]

static ast_node_t take_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &take_code},
    .next = NULL,
};

static ast_node_t take_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &take_body,
            .is_main = false,
            .name = "List[T].take",
            .return_type = &generic_list_node,
            .parameter_lists = &target_list_n_parameter_list_node,
        },
    .data_type = &node_curried_function,
    .next = NULL,
};

#pragma endregion

#pragma region slice(from, until): List[T]

static ast_node_t slice_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &slice_code},
    .next = NULL,
};

static ast_node_t until_int_parameter_node = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "until",
            .data_type = &node_int,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static ast_node_t from_int_parameter_node = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "from",
            .data_type = &node_int,
        },
    .data_type = &node_unit,
    .next = &until_int_parameter_node,
};

static ast_node_t from_until_parameter_list_node = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &from_int_parameter_node,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static ast_node_t from_until_target_list_parameter_list_node = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &target_list_parameter_node,
        },
    .data_type = &node_unit,
    .next = &from_until_parameter_list_node,
};

static ast_node_t slice_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &slice_body,
            .is_main = false,
            .name = "List[T].slice",
            .return_type = &generic_list_node,
            .parameter_lists = &from_until_target_list_parameter_list_node,
        },
    .data_type = &node_curried_function,
    .next = NULL,
};

#pragma endregion

#pragma region head: T

static ast_node_t head_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &head_code},
    .next = NULL,
};

static ast_node_t target_list_parameter_list_node = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &target_list_parameter_node,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static ast_node_t head_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &head_body,
            .is_main = false,
            .name = "List[T].head",
            .return_type = &node_generic_t,
            .parameter_lists = &target_list_parameter_list_node,
        },
    .data_type = &node_curried_function,
    .next = NULL,
};

#pragma endregion

#pragma region tail: List[T]

static ast_node_t tail_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &tail_code},
    .next = NULL,
};

static ast_node_t tail_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &tail_body,
            .is_main = false,
            .name = "List[T].tail",
            .return_type = &generic_list_node,
            .parameter_lists = &target_list_parameter_list_node,
        },
    .data_type = &node_curried_function,
    .next = NULL,
};

#pragma endregion

#pragma region List.range(start, end, [step]) : List[Int]

static ast_node_t step_int_parameter_node = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "step",
            .data_type = &node_int,
            .optional = true,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static ast_node_t end_int_parameter_node = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "end",
            .data_type = &node_int,
        },
    .data_type = &node_unit,
    .next = &step_int_parameter_node,
};

static ast_node_t start_int_parameter_node = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "start",
            .data_type = &node_int,
        },
    .data_type = &node_unit,
    .next = &end_int_parameter_node,
};

static ast_node_t start_end_step_parameter_list_node = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &start_int_parameter_node,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static data_type_node_t list_int_node = {
    .type = DATA_TYPE_LIST,
    .data.list_type =
        {
            .elements_type = &node_int,
        },
    .next = NULL,
};

static data_type_node_t range_step_int = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_INT,
    .next = NULL,
};

static data_type_node_t range_end_int = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_INT,
    .next = &range_step_int,
};

static data_type_node_t range_start_int = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_INT,
    .next = &range_end_int,
};

static data_type_node_t int_int_int_list_int_signature = {
    .type = DATA_TYPE_LAMBDA,
    .data.lambda_type =
        {
            .parameter_types = &range_start_int,
            .return_type = &list_int_node,
        },
    .next = NULL,
};

static ast_node_t range_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "List[T].range",
            .return_type = &list_int_node,
            .parameter_lists = &start_end_step_parameter_list_node,
            .is_static = true,
        },
    .data_type = &int_int_int_list_int_signature,
    .next = NULL,
};

#pragma endregion

static data_type_node_t lambda_boolean_type_node = {
    .type = DATA_TYPE_LAMBDA,
    .data.lambda_type =
        {
            .parameter_types = &node_generic_t,
            .return_type = &node_boolean,
        },
    .next = NULL,
};

static ast_node_t func_parameter_node = {
    .type = AST_PARAMETER,
    .data.parameter =
        {
            .name = "func",
            .data_type = &lambda_boolean_type_node,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static ast_node_t func_parameter_list_node = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &func_parameter_node,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static ast_node_t target_list_func_parameter_list_node = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &target_list_parameter_node,
        },
    .data_type = &node_unit,
    .next = &func_parameter_list_node,
};

#pragma region filter((T) => Boolean) : List[T]

static ast_node_t filter_body = {
    .type = AST_BUILTIN_BODY,
    .data.builtin_body = {.first_instruction = &filter_code},
    .next = NULL,
};

static ast_node_t filter_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = &filter_body,
            .is_main = false,
            .name = "List[T].filter",
            .return_type = &generic_list_node,
            .parameter_lists = &target_list_func_parameter_list_node,
        },
    .data_type = &node_curried_function,
    .next = NULL,
};

#pragma endregion

#pragma region takeWhile((T) => Boolean) : List[T]

static ast_node_t take_while_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "List[T].takeWhile",
            .return_type = &generic_list_node,
            .parameter_lists = &target_list_func_parameter_list_node,
        },
    .data_type = &node_curried_function,
    .next = NULL,
};

#pragma endregion

#pragma region dropWhile((T) => Boolean) : List[T]

static ast_node_t drop_while_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "List[T].dropWhile",
            .return_type = &generic_list_node,
            .parameter_lists = &target_list_func_parameter_list_node,
        },
    .data_type = &node_curried_function,
    .next = NULL,
};

#pragma endregion

#pragma region toList : List[Int]

static ast_node_t target_range_empty_parameter_list_node = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = &target_range_parameter_node,
        },
    .data_type = &node_unit,
    .next = NULL,
};

static ast_node_t to_list_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "$Range.toList",
            .return_type = &list_int_node,
            .parameter_lists = &target_range_empty_parameter_list_node,
        },
    .data_type = &node_curried_function,
    .next = NULL,
};

#pragma endregion

#pragma endregion

hashtable_t *create_builtin_symtable() {
  // Create the table
  hashtable_t *table = NULL;
  if (ht_init(&table, builtin_table_size) != NO_ERR) {
    return NULL;
  }

  ast_node_t *functions[] = {
      &print_function_declaration,
      &read_line_function_declaration,
      &read_int_function_declaration,
      &read_double_function_declaration,
      &to_str_function_declaration,
      &to_int_function_declaration,
      &str_length_function_declaration,
      &sub_str_function_declaration,
      &ord_function_declaration,
      &chr_function_declaration,
      &read_boolean_function_declaration,
      &any_to_string_function_declaration,
      &double_to_int_function_declaration,
      &double_floor_function_declaration,
      &string_length_function_declaration,
      &read_int_maybe_function_declaration,
      &read_double_maybe_function_declaration,
      &to_function_declaration,
      &until_function_declaration,
      &by_function_declaration,
      &drop_function_declaration,
      &take_function_declaration,
      &slice_function_declaration,
      &head_function_declaration,
      &tail_function_declaration,
      &range_function_declaration,
      &filter_function_declaration,
      &take_while_function_declaration,
      &drop_while_function_declaration,
      &to_list_function_declaration,
  };

  int functions_length = sizeof(functions) / sizeof(ast_node_t *);

  // Register built-in functions
  for (int i = 0; i < functions_length; i++) {
    if (ht_insert_item(table, functions[i]->data.function_declaration.name,
                       functions[i]) != NO_ERR) {
      ht_destruct(&table);
      return NULL;
    }
  }

  ast_node_t *types[] = {
      &int_type_declaration,     &double_type_declaration,
      &string_type_declaration,  &unit_type_declaration,
      &boolean_type_declaration, &any_type_declaration,
  };

  int types_length = sizeof(types) / sizeof(ast_node_t *);

  // Register built-in types
  for (int i = 0; i < types_length; i++) {
    if (ht_insert_item(table, types[i]->data.type_declaration.name, types[i]) !=
        NO_ERR) {
      ht_destruct(&table);
      return NULL;
    }
  }

  // Register List constructor
  if (ht_insert_item(table, "List", &list_constructor) != NO_ERR) {
    ht_destruct(&table);
    return NULL;
  }

  return table;
}