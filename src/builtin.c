/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file builtin.c
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Implementation of create_builtin_symtable function
 *
\*****************************************************************************/

#include "builtin.h"
#include "ast_node.h"

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

static ast_node_t print_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "print",
            .return_type = &node_unit,
            .parameter_lists = &any_value_parameter_list,
        },
    .data_type = &node_unit,
    .next = NULL,
};

#pragma endregion

#pragma region readLine(): String

static ast_node_t read_line_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "readLine",
            .return_type = &node_string,
            .parameter_lists = &parameter_list_empty,
        },
    .data_type = &node_string,
    .next = NULL,
};

#pragma endregion

#pragma region readInt(): Int

static ast_node_t read_int_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "readInt",
            .return_type = &node_int,
            .parameter_lists = &parameter_list_empty,
        },
    .data_type = &node_int,
    .next = NULL,
};

#pragma endregion

#pragma region readDouble(): Double

static ast_node_t read_double_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "readDouble",
            .return_type = &node_double,
            .parameter_lists = &parameter_list_empty,
        },
    .data_type = &node_double,
    .next = NULL,
};

#pragma endregion

#pragma region toStr(value: Any) String

static ast_node_t to_str_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "toStr",
            .return_type = &node_string,
            .parameter_lists = &any_value_parameter_list,
        },
    .data_type = &node_string,
    .next = NULL,
};

#pragma endregion

#pragma region toInt(value: Double): Int

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

static ast_node_t to_int_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "toInt",
            .return_type = &node_int,
            .parameter_lists = &double_value_parameter_list,
        },
    .data_type = &node_int,
    .next = NULL,
};

#pragma endregion

#pragma region strLength(value : String) : Int

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

static ast_node_t str_length_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "strLength",
            .return_type = &node_int,
            .parameter_lists = &str_length_parameter_list,
        },
    .data_type = &node_int,
    .next = NULL,
};

#pragma endregion

#pragma region subStr(value: String, start: Int, end: Int): String

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

static ast_node_t sub_str_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "subStr",
            .return_type = &node_string,
            .parameter_lists = &sub_str_parameter_list,
        },
    .data_type = &node_string,
    .next = NULL,
};

#pragma endregion

#pragma region ord(value: String, index: Int): Int

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

static ast_node_t ord_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "ord",
            .return_type = &node_int,
            .parameter_lists = &ord_parameter_list,
        },
    .data_type = &node_int,
    .next = NULL,
};

#pragma endregion

#pragma region chr(code: Int): String

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

static ast_node_t chr_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "chr",
            .return_type = &node_string,
            .parameter_lists = &chr_parameter_list,
        },
    .data_type = &node_string,
    .next = NULL,
};

#pragma endregion

#pragma region readBoolean(): Boolean

static ast_node_t read_boolean_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "readBoolean",
            .return_type = &node_boolean,
            .parameter_lists = &parameter_list_empty,
        },
    .data_type = &node_boolean,
    .next = NULL,
};

#pragma endregion

#pragma region Any.toString(value: Any): String

static ast_node_t any_to_string_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "Any.toString",
            .return_type = &node_string,
            .parameter_lists = &any_value_parameter_list,
        },
    .data_type = &node_string,
    .next = NULL,
};

#pragma endregion

#pragma region Double.toInt(value: Double): Int

static ast_node_t double_to_int_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "Double.toInt",
            .return_type = &node_int,
            .parameter_lists = &double_value_parameter_list,
        },
    .data_type = &node_int,
    .next = NULL,
};

#pragma endregion

#pragma region Double.floor(value: Double): Double

static ast_node_t double_floor_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "Double.floor",
            .return_type = &node_double,
            .parameter_lists = &double_value_parameter_list,
        },
    .data_type = &node_double,
    .next = NULL,
};

#pragma endregion

#pragma region String.length(value: String): Int

static ast_node_t string_length_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "String.length",
            .return_type = &node_int,
            .parameter_lists = &str_length_parameter_list,
        },
    .data_type = &node_int,
    .next = NULL,
};

#pragma endregion

static data_type_node_t union_unit = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_UNIT,
    .next = NULL,
};

#pragma region readIntMaybe() : Int | Unit

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

static ast_node_t read_int_maybe_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "readIntMaybe",
            .return_type = &int_unit_node,
            .parameter_lists = &parameter_list_empty,
        },
    .data_type = &int_unit_node,
    .next = NULL,
};

#pragma endregion

#pragma region readDoubleMaybe() : Double | Unit

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

static ast_node_t read_double_maybe_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "readDoubleMaybe",
            .return_type = &double_unit_node,
            .parameter_lists = &parameter_list_empty,
        },
    .data_type = &double_unit_node,
    .next = NULL,
};

#pragma endregion

#pragma region List(v1, v2, ...) : List[T]

static data_type_node_t generic_list_node = {
    .type = DATA_TYPE_LIST,
    .data.list_type =
        {
            .elements_type = NULL,
        },
    .next = NULL,
};

static ast_node_t list_constructor_parameter_list = {
    .type = AST_PARAMETER_LIST,
    .data.parameter_list =
        {
            .parameters = NULL,
        },
    .data_type = &node_unit,
    .next = NULL,
};
static ast_node_t list_constructor_function_declaration = {
    .type = AST_FUNCTION_DECLARATION,
    .data.function_declaration =
        {
            .body = NULL,
            .is_main = false,
            .name = "List",
            .return_type = &generic_list_node,
            .parameter_lists = &list_constructor_parameter_list,
        },
    .data_type = &generic_list_node,
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
      &list_constructor_function_declaration,
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

  return table;
}