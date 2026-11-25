/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file ast_node.c
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Implementation of abstract syntax tree node
 *
\*****************************************************************************/

#include "ast_node.h"
#include "string_duplicate.h"
#include <stdlib.h>

ast_node_t *create_node() { return calloc(1, sizeof(ast_node_t)); }

void destroy_node(ast_node_t *node) {
  if (node == NULL) {
    return;
  }

  destroy_type_node(node->data_type);

  switch (node->type) {
  case AST_FUNCTION_DECLARATION:
    free(node->data.function_declaration.name);
    destroy_node(node->data.function_declaration.parameter_lists);
    destroy_node(node->data.function_declaration.body);
    destroy_type_node(node->data.function_declaration.return_type);
    break;
  case AST_FUNCTION_CALL:
    free(node->data.function_call.name);
    destroy_node(node->data.function_call.argument_lists);
    break;
  case AST_DOT_FUNCTION_CALL:
    free(node->data.dot_function_call.name);
    destroy_node(node->data.dot_function_call.target);
    destroy_node(node->data.dot_function_call.argument_lists);
    break;
  case AST_LAMBDA_DECLARATION:
    destroy_node(node->data.lambda_declaration.parameters);
    destroy_node(node->data.lambda_declaration.body);
    free(node->data.lambda_declaration.unique_id);
    break;
  case AST_PARAMETER_LIST:
    destroy_node(node->data.parameter_list.parameters);
    break;
  case AST_PARAMETER:
    free(node->data.parameter.name);
    destroy_type_node(node->data.parameter.data_type);
    break;
  case AST_ARGUMENT_LIST:
    destroy_node(node->data.argument_list.arguments);
    break;
  case AST_CLASS_DECLARATION:
    free(node->data.class_declaration.name);
    destroy_node(node->data.class_declaration.attribute_list);
    break;
  case AST_TYPE_DECLARATION:
    free(node->data.type_declaration.name);
    destroy_type_node(node->data.type_declaration.data_type);
    break;
  case AST_EXTENSION_DECLARATION:
    destroy_node(node->data.extension_declaration.parameter);
    destroy_node(node->data.extension_declaration.function_declarations);
    break;
  case AST_BLOCK:
    destroy_node(node->data.block.statement);
    break;
  case AST_VARIABLE_DECLARATION:
    free(node->data.variable_declaration.name);
    destroy_node(node->data.variable_declaration.expression);
    destroy_type_node(node->data.variable_declaration.data_type);
    free(node->data.variable_declaration.unique_id);
    break;
  case AST_ASSIGNMENT:
    free(node->data.assignment.name);
    destroy_node(node->data.assignment.expression);
    break;
  case AST_FOR_DO:
    destroy_node(node->data.for_do.iterator);
    destroy_node(node->data.for_do.generator);
    destroy_node(node->data.for_do.guards);
    destroy_node(node->data.for_do.body);
    break;
  case AST_WHILE_DO:
    destroy_node(node->data.while_do.condition);
    destroy_node(node->data.while_do.body);
    break;
  case AST_IF:
    destroy_node(node->data.if_statement.condition);
    destroy_node(node->data.if_statement.true_branch);
    destroy_node(node->data.if_statement.false_branch);
    break;
  case AST_RETURN:
    destroy_node(node->data.return_statement.expression);
    break;
  case AST_MATCH:
    destroy_node(node->data.match.cases);
    destroy_node(node->data.match.expression);
    break;
  case AST_MATCH_CASE:
    destroy_node(node->data.match_case.block);
    destroy_node(node->data.match_case.guard);
    destroy_node(node->data.match_case.pattern);
    break;
  case AST_BINARY_OPERATION:
    destroy_node(node->data.binary_operation.left);
    destroy_node(node->data.binary_operation.right);
    break;
  case AST_UNARY_OPERATION:
    destroy_node(node->data.unary_operation.expression);
    break;
  case AST_IDENTIFIER:
    free(node->data.identifier.name);
    break;
  case AST_LITERAL:
    free(node->data.literal.raw);
    break;
  case AST_INTERPOLATED_STRING:
    destroy_node(node->data.interpolated_string.expressions);
    break;
  case AST_LIST_CONSTRUCTOR:
  case AST_BUILTIN_BODY:
    break;
  }

  destroy_node(node->next);

  free(node);
}

ast_node_t *create_function_declaration_node(char *name,
                                             ast_node_t *parameter_lists,
                                             ast_node_t *body,
                                             data_type_node_t *return_type,
                                             bool is_main) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_FUNCTION_DECLARATION;
  node->data.function_declaration.name = string_duplicate(name);
  node->data.function_declaration.parameter_lists = parameter_lists;
  node->data.function_declaration.body = body;
  node->data.function_declaration.return_type = return_type;
  node->data.function_declaration.is_main = is_main;
  node->data.function_declaration.is_range_method = false;

  // Check if string_duplicate failed
  if (node->data.function_declaration.name == NULL) {
    destroy_node(node);
    return NULL;
  }

  return node;
}

ast_node_t *create_function_call_node(char *name, ast_node_t *argument_lists) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_FUNCTION_CALL;
  node->data.function_call.name = string_duplicate(name);
  node->data.function_call.argument_lists = argument_lists;

  // Check if string_duplicate failed
  if (node->data.function_call.name == NULL) {
    destroy_node(node);
    return NULL;
  }

  return node;
}

ast_node_t *create_dot_function_call_node(char *name, ast_node_t *target,
                                          ast_node_t *argument_lists) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_DOT_FUNCTION_CALL;
  node->data.dot_function_call.name = string_duplicate(name);
  node->data.dot_function_call.target = target;
  node->data.dot_function_call.argument_lists = argument_lists;
  node->data.dot_function_call.attribute_index = 0;

  // Check if string_duplicate failed
  if (node->data.dot_function_call.name == NULL) {
    destroy_node(node);
    return NULL;
  }

  return node;
}

ast_node_t *create_lambda_declaration_node(ast_node_t *parameters,
                                           ast_node_t *body) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_LAMBDA_DECLARATION;
  node->data.lambda_declaration.parameters = parameters;
  node->data.lambda_declaration.body = body;
  node->data.lambda_declaration.unique_id = NULL;

  return node;
}

ast_node_t *create_parameter_list_node(ast_node_t *parameters) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_PARAMETER_LIST;
  node->data.parameter_list.parameters = parameters;

  return node;
}

ast_node_t *create_parameter_node(char *name, data_type_node_t *data_type) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_PARAMETER;
  node->data.parameter.name = string_duplicate(name);
  node->data.parameter.data_type = data_type;

  // Check if string_duplicate failed
  if (node->data.parameter.name == NULL) {
    destroy_node(node);
    return NULL;
  }

  return node;
}

ast_node_t *create_argument_list_node(ast_node_t *arguments) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_ARGUMENT_LIST;
  node->data.argument_list.arguments = arguments;

  return node;
}

ast_node_t *create_class_declaration_node(char *name, ast_node_t *attributes) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_CLASS_DECLARATION;
  node->data.class_declaration.name = string_duplicate(name);
  node->data.class_declaration.attribute_list = attributes;
  node->data.class_declaration.unique_class_id = 0;
  node->data.class_declaration.attribute_count = 0;

  // Check if string_duplicate failed
  if (node->data.class_declaration.name == NULL) {
    destroy_node(node);
    return NULL;
  }

  return node;
}

ast_node_t *create_type_declaration_node(char *name,
                                         data_type_node_t *data_type) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_TYPE_DECLARATION;
  node->data.type_declaration.name = string_duplicate(name);
  node->data.type_declaration.data_type = data_type;

  // Check if string_duplicate failed
  if (node->data.type_declaration.name == NULL) {
    destroy_node(node);
    return NULL;
  }

  return node;
}

ast_node_t *
create_extension_declaration_node(ast_node_t *parameter,
                                  ast_node_t *function_declarations) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_EXTENSION_DECLARATION;
  node->data.extension_declaration.parameter = parameter;
  node->data.extension_declaration.function_declarations =
      function_declarations;

  return node;
}

ast_node_t *create_block_node(ast_node_t *statement) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_BLOCK;
  node->data.block.statement = statement;

  return node;
}

ast_node_t *create_variable_declaration_node(char *name,
                                             data_type_node_t *data_type,
                                             bool is_const,
                                             ast_node_t *expression) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_VARIABLE_DECLARATION;
  node->data.variable_declaration.name = string_duplicate(name);
  node->data.variable_declaration.data_type = data_type;
  node->data.variable_declaration.is_const = is_const;
  node->data.variable_declaration.expression = expression;

  // Check if string_duplicate failed
  if (node->data.variable_declaration.name == NULL) {
    destroy_node(node);
    return NULL;
  }

  return node;
}

ast_node_t *create_assignment_node(char *name, ast_node_t *expression) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_ASSIGNMENT;
  node->data.assignment.name = string_duplicate(name);
  node->data.assignment.expression = expression;

  // Check if string_duplicate failed
  if (node->data.assignment.name == NULL) {
    destroy_node(node);
    return NULL;
  }

  return node;
}

ast_node_t *create_if_node(ast_node_t *condition, ast_node_t *true_branch,
                           ast_node_t *false_branch) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_IF;
  node->data.if_statement.condition = condition;
  node->data.if_statement.true_branch = true_branch;
  node->data.if_statement.false_branch = false_branch;

  return node;
}

ast_node_t *create_for_do_node(ast_node_t *iterator, ast_node_t *generator,
                               ast_node_t *guards, ast_node_t *body) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_FOR_DO;
  node->data.for_do.iterator = iterator;
  node->data.for_do.generator = generator;
  node->data.for_do.guards = guards;
  node->data.for_do.body = body;

  return node;
}

ast_node_t *create_while_do_node(ast_node_t *condition, ast_node_t *body) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_WHILE_DO;
  node->data.while_do.condition = condition;
  node->data.while_do.body = body;

  return node;
}

ast_node_t *create_return_node(ast_node_t *expression) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_RETURN;
  node->data.return_statement.expression = expression;
  node->data.return_statement.discard = true;

  return node;
}

ast_node_t *create_match_node(ast_node_t *expression, ast_node_t *cases) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_MATCH;
  node->data.match.expression = expression;
  node->data.match.cases = cases;

  return node;
}

ast_node_t *create_match_case_node(ast_node_t *pattern, ast_node_t *guard,
                                   ast_node_t *block) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_MATCH_CASE;
  node->data.match_case.block = block;
  node->data.match_case.guard = guard;
  node->data.match_case.pattern = pattern;

  return node;
}

void node_append(ast_node_t *entry_node, ast_node_t *node_to_append) {
  if (entry_node == NULL) {
    return;
  }

  ast_node_t *current = entry_node;
  while (current->next != NULL) {
    current = current->next;
  }

  current->next = node_to_append;
}

ast_node_t *create_binary_operation_node(token_type_t operation,
                                         ast_node_t *left, ast_node_t *right) {
  // TODO validate operation
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_BINARY_OPERATION;
  node->data.binary_operation.op = operation;
  node->data.binary_operation.left = left;
  node->data.binary_operation.right = right;

  return node;
}

ast_node_t *create_unary_operation_node(token_type_t operation,
                                        ast_node_t *expression) {
  // TODO validate operation
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_UNARY_OPERATION;
  node->data.unary_operation.op = operation;
  node->data.unary_operation.expression = expression;

  return node;
}

ast_node_t *create_leaf_node(token_t *token) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  switch (token->type) {
  case TOK_IDENTIFIER:
    node->type = AST_IDENTIFIER;
    node->data.identifier.name = string_duplicate(token->value);
    break;
  case TOK_LIT_INT:
    node->type = AST_LITERAL;
    node->data.literal.data_type = &node_int;
    node->data.literal.raw = string_duplicate(token->value);
    break;
  case TOK_LIT_DOUBLE:
    node->type = AST_LITERAL;
    node->data.literal.data_type = &node_double;
    node->data.literal.raw = string_duplicate(token->value);
    break;
  case TOK_LIT_STRING:
  case TOK_LIT_INTERP_STRING_PART:
  case TOK_LIT_INTERP_M_STRING_PART:
  case TOK_LIT_INTERP_STRING_END:
  case TOK_LIT_INTERP_M_STRING_END:
    node->type = AST_LITERAL;
    node->data.literal.data_type = &node_string;
    node->data.literal.raw = string_duplicate(token->value);
    break;
  case TOK_LIT_TRUE:
    node->type = AST_LITERAL;
    node->data.literal.data_type = &node_boolean;
    node->data.literal.raw = string_duplicate("true");
    break;
  case TOK_LIT_FALSE:
    node->type = AST_LITERAL;
    node->data.literal.data_type = &node_boolean;
    node->data.literal.raw = string_duplicate("false");
    break;
  case TOK_UNDERSCORE:
    node->type = AST_LITERAL;
    node->data.literal.data_type = &node_any;
    node->data.literal.raw = string_duplicate("_");
    break;
  case TOK_KW_LIST:
    node->type = AST_IDENTIFIER;
    node->data.identifier.name = string_duplicate("List");
    break;
  default:
    // Invalid token for leaf node
    destroy_node(node);
    return NULL;
  }

  // Check if string_duplicate failed
  if (token->type == TOK_IDENTIFIER) {
    if (node->data.identifier.name == NULL) {
      destroy_node(node);
      return NULL;
    }
  } else {
    if (node->data.literal.raw == NULL) {
      destroy_node(node);
      return NULL;
    }
  }

  return node;
}

ast_node_t *create_interpolated_string_node(ast_node_t *expressions) {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_INTERPOLATED_STRING;
  node->data.interpolated_string.expressions = expressions;

  return node;
}

ast_node_t *create_unit_literal_node() {
  // Create node
  ast_node_t *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_LITERAL;
  node->data.literal.raw = string_duplicate("()");
  node->data.literal.data_type = &node_unit;

  // Check if string_duplicate failed
  if (node->data.literal.raw == NULL) {
    destroy_node(node);
    return NULL;
  }

  return node;
}
