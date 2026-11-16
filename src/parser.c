/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file parser.c
 *
 * @author Matous Jahoda (xjahodm00)
 *
 * @brief Implementation of parser
 *
 \*****************************************************************************/

#include "parser.h"

#include "actions.h"
#include "data_type.h"
#include "lexer.h"
#include "string_duplicate.h"
#include "token.h"
#include "token_stack.h"

#include <stdlib.h> // free
#include <string.h> // strcmp

// NON terminals
err_code_t start(token_t *token, ast_node_t **node);
err_code_t prolog(token_t *token);
err_code_t prog(token_t *token, ast_node_t **node);
err_code_t expr(token_t *token, ast_node_t **node);
err_code_t fun_def(token_t *token, ast_node_t **node, bool is_main);
err_code_t type_alias(token_t *token, ast_node_t **node);
err_code_t class_def(token_t *token, ast_node_t **node);
err_code_t extension(token_t *token, ast_node_t **node);
err_code_t param_lists(token_t *token, ast_node_t **node);
err_code_t ret_type(token_t *token, data_type_node_t **node);
err_code_t type(token_t *token, data_type_node_t **node);
err_code_t types(token_t *token, data_type_node_t **node);
err_code_t params(token_t *token, ast_node_t **node);
err_code_t params_tail(token_t *token, ast_node_t **node);
err_code_t param(token_t *token, ast_node_t **node);
err_code_t fun_body(token_t *token, ast_node_t **node);
err_code_t block(token_t *token, ast_node_t **node);
err_code_t stmt_tail(token_t *token, ast_node_t **node);
err_code_t stmt(token_t *token, ast_node_t **node);
err_code_t id_stmt(token_t *token, ast_node_t **node);
err_code_t if_stmt(token_t *token, ast_node_t **node);
err_code_t if_stmt_tail(token_t *token, ast_node_t **true_branch,
                        ast_node_t **false_branch);
err_code_t else_stmt(token_t *token, ast_node_t **node);
err_code_t else_stmt_tail(token_t *token, ast_node_t **node);
err_code_t for_loop(token_t *token, ast_node_t **node);
err_code_t while_loop(token_t *token, ast_node_t **node);
err_code_t ext_list(token_t *token, ast_node_t **node);

err_code_t parse(ast_node_t **node, FILE *stream) {
  token_t *token = token_create();
  if (token == NULL) {
    return INTERNAL_ERR;
  }

  err_code_t ret = lexer_init(stream);
  if (ret != NO_ERR) {
    return ret;
  }
  ret = start(token, node);
  token_free(token);
  lexer_destroy();
  return ret;
}

err_code_t start(token_t *token, ast_node_t **node) {
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }
  ret = prolog(token);
  if (ret != NO_ERR) {
    return ret;
  }
  return prog(token, node);
}

err_code_t prolog(token_t *token) {
  if (token->type != TOK_KW_IMPORT) {
    return SYNTAX_ERR;
  }
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }
  if (!strcmp(token->value, "ifj26")) {
    return SYNTAX_ERR;
  }
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }
  if (token->type != TOK_DOT) {
    return SYNTAX_ERR;
  }
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }
  if (token->type != TOK_MUL) {
    return SYNTAX_ERR;
  }
  return lexer_get_token(token, REQUIRED);
}

err_code_t prog(token_t *token, ast_node_t **node) {
  // helper variable
  err_code_t ret;

  switch (token->type) {
  case TOK_AT_MAIN:
    token_clear(token);
    ret = lexer_get_token(token, ALLOWED);
    if (ret != NO_ERR) {
      return ret;
    }

    // shift one token if new line
    if (token->type == TOK_EOL) {
      token_clear(token);
      err_code_t ret = lexer_get_token(token, ALLOWED);
      if (ret != NO_ERR) {
        return ret;
      }
    }

    ret = fun_def(token, node, true);
    if (ret != NO_ERR) {
      return ret;
    }

    return prog(token, &(*node)->next);

  case TOK_KW_DEF:
    ret = fun_def(token, node, false);
    if (ret != NO_ERR) {
      return ret;
    }

    return prog(token, &(*node)->next);

  case TOK_KW_TYPE:
    ret = type_alias(token, node);
    if (ret != NO_ERR) {
      return ret;
    }

    return prog(token, &(*node)->next);

  case TOK_KW_CASE:
    ret = class_def(token, node);
    if (ret != NO_ERR) {
      return ret;
    }

    return prog(token, &(*node)->next);

  case TOK_KW_EXTENSION:
    ret = extension(token, node);
    if (ret != NO_ERR) {
      return ret;
    }

    return prog(token, &(*node)->next);

  case TOK_EOF:
    *node = NULL;
    return NO_ERR;

  default:
    return SYNTAX_ERR;
  }
}

err_code_t fun_def(token_t *token, ast_node_t **node, bool is_main) {
  if (token->type != TOK_KW_DEF) {
    return SYNTAX_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    return ret;
  }

  if (token->type != TOK_IDENTIFIER) {
    return SYNTAX_ERR;
  }

  char *name = string_duplicate(token->value);
  if (name == NULL) {
    return INTERNAL_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    free(name);
    return ret;
  }

  ast_node_t *parameter_lists = NULL;
  ret = param_lists(token, &parameter_lists);
  if (ret != NO_ERR) {
    free(name);
    return ret;
  }

  data_type_node_t *return_type = NULL;
  ret = ret_type(token, &return_type);
  if (ret != NO_ERR) {
    free(name);
    destroy_node(parameter_lists);
    return ret;
  }

  if (token->type != TOK_EQUAL) {
    free(name);
    destroy_node(parameter_lists);
    destroy_type_node(return_type);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    free(name);
    destroy_node(parameter_lists);
    destroy_type_node(return_type);
    return ret;
  }

  ast_node_t *body;
  ret = fun_body(token, &body);
  if (ret != NO_ERR) {
    free(name);
    destroy_node(parameter_lists);
    destroy_type_node(return_type);
    return ret;
  }

  *node = create_function_declaration_node(name, parameter_lists, body,
                                           return_type, is_main);

  if (*node == NULL) {
    free(name);
    destroy_node(parameter_lists);
    destroy_type_node(return_type);
    destroy_node(body);
    return INTERNAL_ERR;
  }
  return NO_ERR;
}

err_code_t type_alias(token_t *token, ast_node_t **node) {
  if (token->type != TOK_KW_TYPE) {
    return SYNTAX_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }

  if (token->type != TOK_IDENTIFIER) {
    return SYNTAX_ERR;
  }

  char *name = string_duplicate(token->value);
  if (name == NULL) {
    return INTERNAL_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    free(name);
    return ret;
  }

  if (token->type != TOK_EQUAL) {
    free(name);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    free(name);
    return ret;
  }

  data_type_node_t *data_type;
  ret = type(token, &data_type);
  if (ret != NO_ERR) {
    free(name);
    return ret;
  }

  ret = types(token, &data_type->next);
  if (ret != NO_ERR) {
    destroy_type_node(data_type);
    free(name);
    return ret;
  }

  if (token->type != TOK_EOL) {
    destroy_type_node(data_type);
    free(name);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    destroy_type_node(data_type);
    free(name);
    return ret;
  }

  *node = create_type_declaration_node(name, data_type);
  if (*node == NULL) {
    destroy_type_node(data_type);
    free(name);
    return INTERNAL_ERR;
  }
  return NO_ERR;
}

err_code_t class_def(token_t *token, ast_node_t **node) {
  // case
  if (token->type != TOK_KW_CASE) {
    return SYNTAX_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }

  // case class
  if (token->type != TOK_KW_CLASS) {
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }

  // case class id
  if (token->type != TOK_IDENTIFIER) {
    return SYNTAX_ERR;
  }

  char *name = string_duplicate(token->value);
  if (name == NULL) {
    return INTERNAL_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    free(name);
    return ret;
  }

  // case class id (
  if (token->type != TOK_L_PAREN) {
    free(name);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    free(name);
    return ret;
  }

  // case class id ( <params>
  ast_node_t *attributes = NULL;
  ret = params(token, &attributes);

  if (ret != NO_ERR) {
    free(name);
    return ret;
  }

  // case class id ( <params> )
  if (token->type != TOK_R_PAREN) {
    free(name);
    destroy_node(attributes);
    return SYNTAX_ERR;
  }

  // case class id ( <params> ) EOL
  token_clear(token);
  ret = lexer_get_token(token, REQUIRED);
  if (ret != NO_ERR) {
    free(name);
    destroy_node(attributes);
    return ret;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    free(name);
    destroy_node(attributes);
    return ret;
  }

  *node = create_class_declaration_node(name, attributes);
  if (*node == NULL) {
    free(name);
    destroy_node(attributes);
    return INTERNAL_ERR;
  }
  return NO_ERR;
}

err_code_t extension(token_t *token, ast_node_t **node) {
  if (token->type != TOK_KW_EXTENSION) {
    return SYNTAX_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }

  if (token->type != TOK_L_PAREN) {
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    return ret;
  }

  ast_node_t *parameter = NULL;
  ret = param(token, &parameter);
  if (ret != NO_ERR) {
    return ret;
  }

  if (token->type != TOK_R_PAREN) {
    destroy_node(parameter);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, REQUIRED);
  if (ret != NO_ERR) {
    destroy_node(parameter);
    return ret;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    destroy_node(parameter);
    return ret;
  }

  if (token->type != TOK_INDENT) {
    destroy_node(parameter);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    destroy_node(parameter);
    return ret;
  }

  ast_node_t *function_declarations;
  ret = ext_list(token, &function_declarations);

  if (ret != NO_ERR) {
    destroy_node(parameter);
    return ret;
  }

  if (token->type != TOK_DEDENT) {
    destroy_node(parameter);
    destroy_node(function_declarations);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    destroy_node(parameter);
    destroy_node(function_declarations);
    return ret;
  }

  *node = create_extension_declaration_node(parameter, function_declarations);
  if (*node == NULL) {
    destroy_node(parameter);
    destroy_node(function_declarations);
    return INTERNAL_ERR;
  }
  return NO_ERR;
}

err_code_t param_lists(token_t *token, ast_node_t **node) {
  if (token->type != TOK_L_PAREN) {
    return NO_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }

  ast_node_t *parameters;
  ret = params(token, &parameters);

  if (ret != NO_ERR) {
    return ret;
  }

  if (token->type != TOK_R_PAREN) {
    destroy_node(parameters);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    destroy_node(parameters);
    return ret;
  }

  *node = create_parameter_list_node(parameters);
  if (*node == NULL) {
    destroy_node(parameters);
    return INTERNAL_ERR;
  }

  ret = param_lists(token, &(*node)->next);

  if (ret != NO_ERR) {
    destroy_node(*node);
    return ret;
  }
  return NO_ERR;
}

err_code_t ret_type(token_t *token, data_type_node_t **node) {
  if (token->type != TOK_COLON) {
    return NO_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }
  ret = type(token, node);

  if (ret != NO_ERR) {
    destroy_type_node(*node);
    return ret;
  }
  return NO_ERR;
}

err_code_t type(token_t *token, data_type_node_t **node) {
  switch (token->type) {
  case TOK_KW_UNIT:
    *node = create_unit_type_node();
    break;
  case TOK_KW_BOOLEAN:
    *node = create_primitive_type_node(PRIMITIVE_BOOLEAN);
    break;
  case TOK_KW_DOUBLE:
    *node = create_primitive_type_node(PRIMITIVE_DOUBLE);
    break;
  case TOK_KW_INT:
    *node = create_primitive_type_node(PRIMITIVE_INT);
    break;
  case TOK_KW_STRING:
    *node = create_primitive_type_node(PRIMITIVE_STRING);
    break;

  default:
    return SYNTAX_ERR;
  }
  if (*node == NULL) {
    return INTERNAL_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_type_node(*node);
  }
  return ret;
}

err_code_t types(token_t *token, data_type_node_t **node) {
  if (token->type != TOK_PIPE) {
    return NO_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }
  ret = type(token, node);

  if (ret != NO_ERR) {
    destroy_type_node(*node);
    return ret;
  }

  ret = types(token, &(*node)->next);

  if (ret != NO_ERR) {
    destroy_type_node(*node);
  }
  return ret;
}

err_code_t params(token_t *token, ast_node_t **node) {
  if (token->type != TOK_IDENTIFIER) {
    return NO_ERR;
  }

  err_code_t ret = param(token, node);
  if (ret != NO_ERR) {
    return ret;
  }

  ret = params_tail(token, &(*node)->next);

  if (ret != NO_ERR) {
    destroy_node(*node);
  }
  return ret;
}

err_code_t params_tail(token_t *token, ast_node_t **node) {
  if (token->type != TOK_COMMA) {
    return NO_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    return ret;
  }

  return params(token, node);
}

err_code_t param(token_t *token, ast_node_t **node) {
  if (token->type != TOK_IDENTIFIER) {
    return NO_ERR;
  }

  char *name = string_duplicate(token->value);
  if (name == NULL) {
    return INTERNAL_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    free(name);
    return ret;
  }

  if (token->type != TOK_COLON) {
    free(name);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    free(name);
    return ret;
  }

  data_type_node_t *data_type = NULL;
  ret = types(token, &data_type);
  if (ret != NO_ERR) {
    free(name);
    return ret;
  }

  *node = create_parameter_node(name, data_type);
  if (*node == NULL) {
    free(name);
    destroy_type_node(data_type);
    return INTERNAL_ERR;
  }
  return NO_ERR;
}

err_code_t fun_body(token_t *token, ast_node_t **node) {
  // same line definition
  if (token->type != TOK_EOL) {
    err_code_t ret = expr(token, node);
    if (ret != NO_ERR) {
      return ret;
    }

    token_clear(token);
    ret = lexer_get_token(token, FORBIDDEN);
    if (ret != NO_ERR) {
      destroy_node(*node);
    }
    return ret;
  }
  // block definiton

  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }

  if (token->type != TOK_INDENT) {
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }

  ast_node_t *statement = NULL;
  ret = block(token, &statement);
  if (ret != NO_ERR) {
    return ret;
  }

  *node = create_block_node(statement);
  if (*node == NULL) {
    destroy_node(statement);
    return INTERNAL_ERR;
  }

  if (token->type != TOK_DEDENT) {
    destroy_node(*node);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_node(*node);
  }
  return ret;
}

err_code_t block(token_t *token, ast_node_t **node) {
  err_code_t ret = stmt(token, node);
  if (ret != NO_ERR) {
    return ret;
  }

  ast_node_t *statement = NULL;
  ret = stmt_tail(token, &statement);
  if (ret != NO_ERR) {
    destroy_node(*node);
    return ret;
  }
  node_append(*node, statement);
  return NO_ERR;
}

err_code_t stmt_tail(token_t *token, ast_node_t **node) {
  if (token->type == TOK_EOL) {
    token_clear(token);
    return lexer_get_token(token, FORBIDDEN);
  }
  if (token->type != TOK_SEMICOLON) {
    return SYNTAX_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }

  ret = stmt(token, node);
  if (ret != NO_ERR) {
    return ret;
  }

  ret = stmt_tail(token, &(*node)->next);
  if (ret != NO_ERR) {
    destroy_node(*node);
  }
  return ret;
}

err_code_t stmt(token_t *token, ast_node_t **node) {
  err_code_t ret;
  switch (token->type) {
  case TOK_KW_IF:
    token_clear(token);
    ret = lexer_get_token(token, FORBIDDEN);
    if (ret != NO_ERR) {
      return ret;
    }
    return if_stmt(token, node);

  case TOK_KW_FOR:
    token_clear(token);
    ret = lexer_get_token(token, ALLOWED);
    if (ret != NO_ERR) {
      return ret;
    }
    return for_loop(token, node);

  case TOK_KW_WHILE:
    token_clear(token);
    ret = lexer_get_token(token, FORBIDDEN);
    if (ret != NO_ERR) {
      return ret;
    }
    return while_loop(token, node);

  case TOK_IDENTIFIER:
    return id_stmt(token, node);

  default:
    return expr(token, node);
  }
}

err_code_t if_stmt(token_t *token, ast_node_t **node) {
  ast_node_t *condition = NULL;
  err_code_t ret = expr(token, &condition);
  if (ret != NO_ERR) {
    return ret;
  }

  if (token->type != TOK_KW_THEN) {
    destroy_node(condition);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_node(condition);
  }

  ast_node_t *true_branch = NULL;
  ast_node_t *false_branch = NULL;
  ret = if_stmt_tail(token, &true_branch, &false_branch);

  if (ret != NO_ERR) {
    destroy_node(condition);
    return ret;
  }

  *node = create_if_node(condition, true_branch, false_branch);
  if (*node == NULL) {
    destroy_node(condition);
    destroy_node(true_branch);
    destroy_node(false_branch);
    return INTERNAL_ERR;
  }
  return NO_ERR;
}

err_code_t if_stmt_tail(token_t *token, ast_node_t **true_branch,
                        ast_node_t **false_branch) {
  err_code_t ret;
  if (token->type == TOK_INDENT) {
    token_clear(token);
    ret = lexer_get_token(token, ALLOWED);
    if (ret != NO_ERR) {
      return ret;
    }

    ast_node_t *statement = NULL;
    ret = block(token, &statement);
    if (ret != NO_ERR) {
      return ret;
    }

    *true_branch = create_block_node(statement);
    if (*true_branch == NULL) {
      destroy_node(statement);
      return INTERNAL_ERR;
    }

    if (token->type != TOK_DEDENT) {
      destroy_node(*true_branch);
      return SYNTAX_ERR;
    }

    token_clear(token);
    ret = lexer_get_token(token, ALLOWED);
    if (ret != NO_ERR) {
      destroy_node(*true_branch);
      return ret;
    }

    ret = else_stmt(token, false_branch);

    if (ret != NO_ERR) {
      destroy_node(*true_branch);
    }
    return ret;
  }

  ret = stmt(token, true_branch);
  if (ret != NO_ERR) {
    return ret;
  }

  ret = else_stmt(token, false_branch);

  if (ret != NO_ERR) {
    destroy_node(*true_branch);
  }
  return ret;
}

err_code_t else_stmt(token_t *token, ast_node_t **node) {
  if (token->type != TOK_KW_ELSE) {
    return NO_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    return ret;
  }

  return else_stmt_tail(token, node);
}

err_code_t else_stmt_tail(token_t *token, ast_node_t **node) {
  if (token->type != TOK_INDENT) {
    return stmt(token, node);
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }

  ast_node_t *statement = NULL;
  ret = block(token, &statement);
  if (ret != NO_ERR) {
    return ret;
  }

  *node = create_block_node(statement);
  if (*node == NULL) {
    destroy_node(statement);
    return INTERNAL_ERR;
  }

  if (token->type != TOK_DEDENT) {
    destroy_node(*node);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_node(*node);
  }
  return ret;
}
