/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file semantic_analyser.c
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Implementation of functions used in semantic analysis
 *
\*****************************************************************************/

#include "semantic_analyser.h"
#include "string_duplicate.h"
#include <stdlib.h>

t_ast_node *create_function_declaration_node(char *name,
                                             t_ast_node *parameter_lists,
                                             t_ast_node *body,
                                             t_data_type_node *return_type) {
  // Create node
  t_ast_node *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_FUNCTION_DECLARATION;
  node->data.function_declaration.name = string_duplicate(name);
  node->data.function_declaration.parameter_lists = parameter_lists;
  node->data.function_declaration.body = body;
  node->data.function_declaration.return_type = return_type;

  // Check if string_duplicate failed
  if (node->data.function_declaration.name == NULL) {
    destroy_node(node);
    return NULL;
  }

  return node;
}

t_ast_node *create_function_call_node(char *name, t_ast_node *argument_lists) {
  // Create node
  t_ast_node *node = create_node();
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

t_ast_node *create_dot_function_call_node(char *name, t_ast_node *target,
                                          t_ast_node *argument_lists) {
  // Create node
  t_ast_node *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_DOT_FUNCTION_CALL;
  node->data.dot_function_call.name = string_duplicate(name);
  node->data.dot_function_call.target = target;
  node->data.dot_function_call.argument_lists = argument_lists;

  // Check if string_duplicate failed
  if (node->data.dot_function_call.name == NULL) {
    destroy_node(node);
    return NULL;
  }

  return node;
}

t_ast_node *create_lambda_declaration_node(t_ast_node *parameters,
                                           t_ast_node *body,
                                           t_data_type_node *return_type) {
  // Create node
  t_ast_node *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_LAMBDA_DECLARATION;
  node->data.lambda_declaration.parameters = parameters;
  node->data.lambda_declaration.body = body;
  node->data.lambda_declaration.return_type = return_type;

  return node;
}

t_ast_node *create_parameter_list_node(t_ast_node *parameters) {
  // Create node
  t_ast_node *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_PARAMETER_LIST;
  node->data.parameter_list.parameters = parameters;

  return node;
}

t_ast_node *create_parameter_node(char *name, t_data_type_node *data_type) {
  // Create node
  t_ast_node *node = create_node();
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

t_ast_node *create_class_declaration_node(char *name, t_ast_node *attributes) {
  // Create node
  t_ast_node *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_CLASS_DECLARATION;
  node->data.class_declaration.name = string_duplicate(name);
  node->data.class_declaration.attributes = attributes;

  // Check if string_duplicate failed
  if (node->data.class_declaration.name == NULL) {
    destroy_node(node);
    return NULL;
  }

  return node;
}

t_ast_node *create_type_declaration_node(char *name,
                                         t_data_type_node *data_type) {
  // Create node
  t_ast_node *node = create_node();
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

t_ast_node *
create_extension_declaration_node(t_ast_node *parameter,
                                  t_ast_node *function_declarations) {
  // Create node
  t_ast_node *node = create_node();
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

t_ast_node *create_block_node(t_ast_node *statement) {
  // Create node
  t_ast_node *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_BLOCK;
  node->data.block.statement = statement;

  return node;
}

t_ast_node *create_variable_declaration_node(char *name,
                                             t_data_type_node *data_type,
                                             bool is_const,
                                             t_ast_node *expression) {
  // Create node
  t_ast_node *node = create_node();
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

t_ast_node *create_assignment_node(char *name, t_ast_node *expression) {
  // Create node
  t_ast_node *node = create_node();
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

t_ast_node *create_if_node(t_ast_node *condition, t_ast_node *true_branch,
                           t_ast_node *false_branch) {
  // Create node
  t_ast_node *node = create_node();
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

t_ast_node *create_for_do_node(t_ast_node *generator, t_ast_node *guards,
                               t_ast_node *body) {
  // Create node
  t_ast_node *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_FOR_DO;
  node->data.for_do.generator = generator;
  node->data.for_do.guards = guards;
  node->data.for_do.body = body;

  return node;
}

t_ast_node *create_while_do_node(t_ast_node *condition, t_ast_node *body) {
  // Create node
  t_ast_node *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_WHILE_DO;
  node->data.while_do.condition = condition;
  node->data.while_do.body = body;

  return node;
}

t_ast_node *create_return_node(t_ast_node *expression) {
  // Create node
  t_ast_node *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_RETURN;
  node->data.return_statement.expression = expression;

  return node;
}

t_ast_node *create_match_node(t_ast_node *expression, t_ast_node *cases) {
  // Create node
  t_ast_node *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_MATCH;
  node->data.match.expression = expression;
  node->data.match.cases = cases;

  return node;
}

t_ast_node *create_match_case_node(t_ast_node *pattern, t_ast_node *guard,
                                   t_ast_node *block) {
  // Create node
  t_ast_node *node = create_node();
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

t_ast_node *create_binary_operation_node(token_type_t operation,
                                         t_ast_node *left, t_ast_node *right) {
  // TODO validate operation
  // Create node
  t_ast_node *node = create_node();
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

t_ast_node *create_unary_operation_node(token_type_t operation,
                                        t_ast_node *expression) {
  // TODO validate operation
  // Create node
  t_ast_node *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_UNARY_OPERATION;
  node->data.unary_operation.op = operation;
  node->data.unary_operation.expression = expression;

  return node;
}

t_ast_node *create_leaf_node(token_t token) {
  // Create node
  t_ast_node *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  switch (token.type) {
  case TOK_IDENTIFIER:
    node->type = AST_IDENTIFIER;
    node->data.identifier.name = string_duplicate(token.value);
    break;
  case TOK_LIT_INT:
    node->type = AST_LITERAL;
    node->data.literal.primitive = PRIMITIVE_INT;
    node->data.literal.raw = string_duplicate(token.value);
    break;
  case TOK_LIT_DOUBLE:
    node->type = AST_LITERAL;
    node->data.literal.primitive = PRIMITIVE_DOUBLE;
    node->data.literal.raw = string_duplicate(token.value);
    break;
  case TOK_LIT_STRING:
    node->type = AST_LITERAL;
    node->data.literal.primitive = PRIMITIVE_STRING;
    node->data.literal.raw = string_duplicate(token.value);
    break;
  case TOK_LIT_TRUE:
    node->type = AST_LITERAL;
    node->data.literal.primitive = PRIMITIVE_BOOLEAN;
    node->data.literal.raw = string_duplicate(token.value);
    break;
  case TOK_LIT_FALSE:
    node->type = AST_LITERAL;
    node->data.literal.primitive = PRIMITIVE_BOOLEAN;
    node->data.literal.raw = string_duplicate(token.value);
    break;
  default:
    // Invalid token for leaf node
    destroy_node(node);
    return NULL;
  }

  // Check if string_duplicate failed
  if (token.type == TOK_IDENTIFIER) {
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

t_ast_node *create_interpolated_string_node(t_ast_node *expressions) {
  // Create node
  t_ast_node *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = AST_INTERPOLATED_STRING;
  node->data.interpolated_string.expressions = expressions;

  return node;
}