/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file convert_node.c
 *
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Implementation of AST node to 3AC statement conversion
 *        (1 node -> 1 statement)
 *
 \*****************************************************************************/

#include "convert_node.h"
#include "string.h"
#include "token.h"
#include <stdio.h>

/*********************************\
 * Helper functions declarations *
\*********************************/

err_code_t convert_function_call(context_t *context, ast_node_t *node);

err_code_t convert_dot_function_call(context_t *context, ast_node_t *node);

err_code_t convert_lambda_declaration(context_t *context, ast_node_t *node);

err_code_t convert_extension_declaration(context_t *context, ast_node_t *node);

err_code_t convert_block(context_t *context, ast_node_t *node);

err_code_t convert_var_declaration(context_t *context, ast_node_t *node);

err_code_t convert_assignment(context_t *context, ast_node_t *node);

err_code_t convert_if(context_t *context, ast_node_t *node);

err_code_t convert_while_do(context_t *context, ast_node_t *node);

err_code_t convert_for_do(context_t *context, ast_node_t *node);

err_code_t convert_return(context_t *context, ast_node_t *node);

err_code_t convert_match(context_t *context, ast_node_t *node);

err_code_t convert_match_case(context_t *context, ast_node_t *node);

err_code_t convert_binary_op(context_t *context, ast_node_t *node);

err_code_t convert_unary_op(context_t *context, ast_node_t *node);

err_code_t list_ctor(context_t *context, ast_node_t *node);

/***********************************\
 * Helper functions implementation *
\***********************************/

// For frames and unique labels and variables
char *add_prefix_to_id(char *identifier, const char *prefix) {
  size_t final_len = strlen(prefix) + strlen(identifier);
  char *result = malloc((sizeof(char) * final_len) + 1 + 1);

  if (result == NULL) {
    return NULL;
  }

  snprintf(result, final_len, "%s%s", prefix, identifier);
  // TODO: if identifer will be overwritten it has to be deallocated here
  return result;
}

char *join_strings_with_char(char *prefix, char middle_char, char *identifier) {

  size_t final_len = strlen(prefix) + strlen(identifier);
  // + 1 for '\0' and + 1 for the character between prefix and identifier
  char *result = malloc((sizeof(char) * final_len) + 1 + 1);

  if (result == NULL) {
    free(prefix);
    // TODO: if identifer will be overwritten it has to be deallocated here
    return NULL;
  }

  snprintf(result, final_len, "%s%c%s", prefix, middle_char, identifier);
  free(prefix);
  // TODO: if identifer will be overwritten it has to be deallocated here

  return result;
}

char *number_to_string(size_t number) {
  // should return the number of characters needed to express number as string
  size_t length = snprintf(NULL, 0, "%llu", (unsigned long long)number);
  char *result = malloc((sizeof(char) * length) + 1);

  snprintf(result, length, "%llu", (unsigned long long)number);

  return result;
}

char *create_unique_variable(context_t *context, char *identifier) {
  context->unique_var_counter++;
  return join_strings_with_char(
      number_to_string(context->unique_var_counter - 1), '$', identifier);
}

// To traverse all nodes inside block
void block_body_list_traversal(context_t *context, ast_node_t *node) {
  while (node != NULL) {
    convert_node(context, node);
    node = node->next;
  }
}

/**
 * @brief Converts expression using stack instruction and
 *        puts result on top of data stack (in ifjcode26)
 *
 * @param context Context structure
 * @param node Node with the root of the expression subtree
 */
err_code_t convert_expression_on_stack(context_t *context, ast_node_t *node) {
  if (node == NULL) {
    return INTERNAL_ERR;
  }

  err_code_t err = NO_ERR;
  inner_symbol_t empty_sym = create_empty_symbol(),
                 operand = create_empty_symbol();

  // if recursion end in this calling
  if (node->type != AST_BINARY_OPERATION) {

    if (node->type == AST_LITERAL) {
      // TODO: (pain)
    }

    if (node->type == AST_IDENTIFIER) {
      operand = create_var_symbol(
          create_unique_variable(context, node->data.identifier.name));
    }

    // pushs
    context->tail->next_statement =
        create_statement(INNER_PUSHS, empty_sym, operand, empty_sym);
    if (context->tail->next_statement == NULL) {
      return INTERNAL_ERR;
    }
    context->tail = context->tail->next_statement;

  } else { // recursion continues

    // check invalid pointers
    if (node->data.binary_operation.right == NULL ||
        node->data.binary_operation.left == NULL) {
      return INTERNAL_ERR;
    }

    // right child recursion
    err =
        convert_expression_on_stack(context, node->data.binary_operation.right);
    if (err != NO_ERR) {
      return err;
    }

    // left child recursion
    err =
        convert_expression_on_stack(context, node->data.binary_operation.left);
    if (err != NO_ERR) {
      return err;
    }

    // choose what operation to convert
    inner_statement_operation_t operation = INNER_NOP_CUSTOM;

    switch (node->data.binary_operation.op) {
    case TOK_BINARY_PLUS:
      // adds
      operation = INNER_ADDS;
      break;
    case TOK_BINARY_MINUS:
      // subs
      operation = INNER_SUBS;
      break;
    case TOK_MUL:
      // muls
      operation = INNER_MULS;
      break;
    case TOK_DIV:
      // TODO: rozdelit podle typu na IDIV a DIV (celociselne / realne deleni)
      break;

      // TODO others

    default:
      break;
    }

    // convert the operation to statement
    create_statement(operation, empty_sym, empty_sym, empty_sym);
    if (context->tail->next_statement == NULL) {
      return INTERNAL_ERR;
    }
    context->tail = context->tail->next_statement;
  }

  return err;
}

// conversion functions
err_code_t convert_function_call(context_t *context, ast_node_t *node) {
  err_code_t err = NO_ERR;
  inner_symbol_t empty_sym = create_empty_symbol();
  inner_symbol_t label = create_var_symbol(node->data.function_call.name);

  // createframe
  context->tail->next_statement =
      create_statement(INNER_CREATEFRAME, empty_sym, empty_sym, empty_sym);
  if (context->tail->next_statement == NULL) {
    return INTERNAL_ERR;
  }
  context->tail = context->tail->next_statement;

  // TODO params

  // pushframe
  context->tail->next_statement =
      create_statement(INNER_PUSHFRAME, empty_sym, empty_sym, empty_sym);
  if (context->tail->next_statement == NULL) {
    return INTERNAL_ERR;
  }
  context->tail = context->tail->next_statement;

  // label
  context->tail->next_statement =
      create_statement(INNER_CALL, empty_sym, label, empty_sym);
  if (context->tail->next_statement == NULL) {
    return INTERNAL_ERR;
  }
  context->tail = context->tail->next_statement;

  return err;
}

err_code_t convert_block(context_t *context, ast_node_t *node) {
  // body of the block
  block_body_list_traversal(context, node->data.block.statement);

  return NO_ERR;
}

err_code_t convert_var_declaration(context_t *context, ast_node_t *node) {
  err_code_t err = NO_ERR;
  inner_symbol_t empty_sym = create_empty_symbol();

  char *name = create_unique_variable(context, node->data.assignment.name);
  if (name == NULL) {
    return INTERNAL_ERR;
  }

  node->data.variable_declaration.unique_id = name; // TODO: spravne?

  name = add_prefix_to_id(name, "TF@");
  if (name == NULL) {
    return INTERNAL_ERR;
  }

  inner_symbol_t variable = create_var_symbol(name);

  // defvar
  context->tail->next_statement =
      create_statement(INNER_DEFVAR, empty_sym, variable, empty_sym);
  if (context->tail->next_statement == NULL) {
    return INTERNAL_ERR;
  }
  context->tail = context->tail->next_statement;

  // calculates the expression and leaves the result on top of the stack
  err = convert_expression_on_stack(context,
                                    node->data.variable_declaration.expression);
  if (err != NO_ERR) {
    return err;
  }

  // pops
  context->tail->next_statement =
      create_statement(INNER_POPS, variable, empty_sym, empty_sym);
  if (context->tail->next_statement == NULL) {
    return INTERNAL_ERR;
  }
  context->tail = context->tail->next_statement;

  return err;
}

err_code_t convert_assignment(context_t *context, ast_node_t *node) {
  err_code_t err = NO_ERR;
  inner_symbol_t empty_sym = create_empty_symbol();

  char *name = node->data.assignment.name; // TODO: potrebuju dostat unique name
  name = add_prefix_to_id(name, "TF@");
  if (name == NULL) {
    return INTERNAL_ERR;
  }

  inner_symbol_t variable = create_var_symbol(name);

  // calculates the expression and leaves the result on top of the stack
  err = convert_expression_on_stack(context, node->data.assignment.expression);
  if (err != NO_ERR) {
    return err;
  }

  // pops
  context->tail->next_statement =
      create_statement(INNER_POPS, variable, empty_sym, empty_sym);
  if (context->tail->next_statement == NULL) {
    return INTERNAL_ERR;
  }
  context->tail = context->tail->next_statement;

  return err;
}

err_code_t convert_return(context_t *context, ast_node_t *node) {
  err_code_t err = NO_ERR;
  inner_symbol_t empty_sym = create_empty_symbol();

  // calculates the expression and leaves the result on top of the stack
  err = convert_expression_on_stack(context,
                                    node->data.return_statement.expression);
  if (err != NO_ERR) {
    return err;
  }

  // return
  context->tail->next_statement =
      create_statement(INNER_RETURN, empty_sym, empty_sym, empty_sym);
  if (context->tail->next_statement == NULL) {
    return INTERNAL_ERR;
  }
  context->tail = context->tail->next_statement;

  return err;
}

/****************************\
 * Interface implementation *
\****************************/

err_code_t convert_node(context_t *context, ast_node_t *node) {

  err_code_t err = NO_ERR;

  switch (node->type) {
  case AST_FUNCTION_CALL:
    err = enqueue_function(context, node->data.function_call.calling);
    if (err != NO_ERR) {
      return err;
    }
    err = convert_function_call(context, node);
    break;
  case AST_DOT_FUNCTION_CALL:
    break;
  case AST_LAMBDA_DECLARATION:
    break;
  case AST_EXTENSION_DECLARATION:
    break;
  case AST_BLOCK:
    err = convert_block(context, node);
    break;
  case AST_VARIABLE_DECLARATION:
    err = convert_var_declaration(context, node);
    break;
  case AST_ASSIGNMENT:
    err = convert_assignment(context, node);
    break;
  case AST_IF:
    break;
  case AST_WHILE_DO:
    break;
  case AST_FOR_DO:
    break;
  case AST_RETURN:
    convert_return(context, node);
    break;
  case AST_MATCH:
    break;
  case AST_MATCH_CASE:
    break;
  case AST_BINARY_OPERATION:
    break;
  case AST_UNARY_OPERATION:
    break;
  case AST_LIST_CONSTRUCTOR:
    break;
  default:
    err = INTERNAL_ERR;
    break;
  }

  return err;
}
