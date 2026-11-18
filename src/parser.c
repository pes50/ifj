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

#pragma region DECLARATIONS
// NON terminals
err_code_t start(token_t *token, ast_node_t **node);
err_code_t prolog(token_t *token);
err_code_t prog(token_t *token, ast_node_t **node);
err_code_t expr(token_t *token, ast_node_t **node);
err_code_t expr_pre_parse(token_t *token, ast_node_t **node);
err_code_t expr_tail(token_t *token, ast_node_t **node,
                     ast_node_t **expression);
err_code_t fun_def(token_t *token, ast_node_t **node, bool is_main);
err_code_t type_alias(token_t *token, ast_node_t **node);
err_code_t class_def(token_t *token, ast_node_t **node);
err_code_t extension(token_t *token, ast_node_t **node);
err_code_t param_lists(token_t *token, ast_node_t **node);
err_code_t ret_type(token_t *token, data_type_node_t **node);
err_code_t type(token_t *token, data_type_node_t **node);
err_code_t types(token_t *token, data_type_node_t **node);
err_code_t lambda_type(token_t *token, data_type_node_t **node);
err_code_t lambda_type_tail(token_t *token, data_type_node_t **node);
err_code_t params(token_t *token, ast_node_t **node);
err_code_t params_tail(token_t *token, ast_node_t **node);
err_code_t param(token_t *token, ast_node_t **node);
err_code_t fun_body(token_t *token, ast_node_t **node);
err_code_t block(token_t *token, ast_node_t **node);
err_code_t stmt_tail(token_t *token, ast_node_t **node);
err_code_t stmt(token_t *token, ast_node_t **node);
err_code_t var_dec(token_t *token, ast_node_t **node, bool is_const);
err_code_t id_stmt(token_t *token, ast_node_t **node);
err_code_t if_stmt(token_t *token, ast_node_t **node);
err_code_t if_stmt_tail(token_t *token, ast_node_t **true_branch,
                        ast_node_t **false_branch);
err_code_t else_stmt(token_t *token, ast_node_t **node);
err_code_t else_stmt_tail(token_t *token, ast_node_t **node);
err_code_t for_loop(token_t *token, ast_node_t **node);
err_code_t gen(token_t *token, ast_node_t **iterator, ast_node_t **generator);
err_code_t gen_tail(token_t *token, ast_node_t *expression,
                    ast_node_t **generator);
err_code_t guard_list(token_t *token, ast_node_t **node);
err_code_t for_tail(token_t *token, ast_node_t **node);
err_code_t while_loop(token_t *token, ast_node_t **node);
err_code_t ext_list(token_t *token, ast_node_t **node);
err_code_t case_list(token_t *token, ast_node_t **node);
err_code_t case_tail(token_t *token, ast_node_t **guard,
                     ast_node_t **expression);
err_code_t case_param(token_t *token, ast_node_t **node);
err_code_t if_guard(token_t *token, ast_node_t **node);
err_code_t fun_call(token_t *token, ast_node_t **node);
err_code_t dot_call(token_t *token, ast_node_t **node);
err_code_t dot_call_tail(token_t *token, ast_node_t **node, ast_node_t *target);
err_code_t arg_lists(token_t *token, ast_node_t **node);
err_code_t args(token_t *token, ast_node_t **node);
err_code_t args_tail(token_t *token, ast_node_t **node);
err_code_t list(token_t *token, ast_node_t **node);
err_code_t interpol(token_t *token, ast_node_t **node);
err_code_t interpol_part(token_t *token, ast_node_t **node);
err_code_t interpol_m(token_t *token, ast_node_t **node);
err_code_t interpol_part_m(token_t *token, ast_node_t **node);
err_code_t lambda(token_t *token, ast_node_t **node);

#pragma endregion

#pragma region PARSE

err_code_t parse(ast_node_t **node, FILE *stream) {
  token_t *token = token_create();
  if (token == NULL) {
    return INTERNAL_ERR;
  }

  lexer_init(stream);
  err_code_t ret = start(token, node);
  token_free(token);
  lexer_destroy();
  return ret;
}

#pragma endregion

#pragma region HIGH LEVEL

err_code_t start(token_t *token, ast_node_t **node) {
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }
  ret = prolog(token);
  if (ret != NO_ERR) {
    return ret;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }

  return prog(token, node);
}

err_code_t prolog(token_t *token) {
  if (token->type != TOK_KW_IMPORT) {
    return SYNTAX_ERR;
  }
  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }
  if (strcmp(token->value, "ifj26")) {
    return SYNTAX_ERR;
  }
  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }
  if (token->type != TOK_DOT) {
    return SYNTAX_ERR;
  }
  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }
  if (token->type != TOK_MUL) {
    return SYNTAX_ERR;
  }
  token_clear(token);
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

  if (token->type != TOK_ASSIGN) {
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

  free(name);
  if (*node == NULL) {
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

  if (token->type != TOK_ASSIGN) {
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
  free(name);
  if (*node == NULL) {
    destroy_type_node(data_type);
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
  free(name);
  if (*node == NULL) {
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

  ast_node_t *function_declarations = NULL;
  ret = ext_list(token, &function_declarations);

  if (ret != NO_ERR) {
    destroy_node(parameter);
    destroy_node(function_declarations);
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

err_code_t ext_list(token_t *token, ast_node_t **node) {
  if (token->type == TOK_DEDENT) {
    return NO_ERR;
  }

  err_code_t ret = fun_def(token, node, false);
  if (ret != NO_ERR) {
    return ret;
  }

  return ext_list(token, &(*node)->next);
}

#pragma endregion

#pragma region TYPES PARAMS

err_code_t param_lists(token_t *token, ast_node_t **node) {
  if (token->type != TOK_L_PAREN) {
    return NO_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }

  ast_node_t *parameters = NULL;
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
    *node = NULL;
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
    *node = NULL;
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
  case TOK_IDENTIFIER:
    *node = create_class_type_node(token->value);
    break;
  case TOK_KW_LIST:
    token_clear(token);
    err_code_t ret = lexer_get_token(token, FORBIDDEN);
    if (ret != NO_ERR) {
      return ret;
    }

    if (token->type != TOK_L_BRACKET) {
      return SYNTAX_ERR;
    }
    token_clear(token);
    ret = lexer_get_token(token, FORBIDDEN);
    if (ret != NO_ERR) {
      return ret;
    }

    data_type_node_t *element_type = NULL;
    ret = type(token, &element_type);
    if (ret != NO_ERR) {
      return ret;
    }

    if (token->type != TOK_R_BRACKET) {
      destroy_type_node(element_type);
      return SYNTAX_ERR;
    }

    *node = create_list_type_node(element_type);
    if (*node == NULL) {
      destroy_type_node(element_type);
    }
    break;

  case TOK_L_PAREN:
    return lambda_type(token, node);

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
    *node = NULL;
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
    *node = NULL;
    return ret;
  }

  ret = types(token, &(*node)->next);

  if (ret != NO_ERR) {
    destroy_type_node(*node);
    *node = NULL;
  }
  return ret;
}

err_code_t lambda_type(token_t *token, data_type_node_t **node) {
  if (token->type != TOK_L_PAREN) {
    return SYNTAX_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    return ret;
  }

  data_type_node_t *parameter_types = NULL;
  ret = type(token, &parameter_types);
  if (ret != NO_ERR) {
    return ret;
  }

  ret = lambda_type_tail(token, &parameter_types->next);
  if (ret != NO_ERR) {
    destroy_type_node(parameter_types);
    return ret;
  }

  if (token->type != TOK_ARROW_RIGHT) {
    destroy_type_node(parameter_types);
    return NO_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_type_node(parameter_types);
    return ret;
  }

  data_type_node_t *return_type = NULL;
  ret = type(token, &return_type);
  if (ret != NO_ERR) {
    destroy_type_node(parameter_types);
    return ret;
  }

  *node = create_lambda_type_node(parameter_types, return_type);
  if (*node == NULL) {
    destroy_type_node(parameter_types);
    destroy_type_node(return_type);
    return INTERNAL_ERR;
  }
  return NO_ERR;
}

err_code_t lambda_type_tail(token_t *token, data_type_node_t **node) {
  if (token->type != TOK_COMMA) {
    return NO_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    return ret;
  }

  data_type_node_t *parameter_types = NULL;
  ret = type(token, &parameter_types);
  if (ret != NO_ERR) {
    return ret;
  }

  return lambda_type_tail(token, &parameter_types->next);
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
    *node = NULL;
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
  ret = type(token, &data_type);
  if (ret != NO_ERR) {
    free(name);
    return ret;
  }

  ret = types(token, &data_type->next);
  if (ret != NO_ERR) {
    free(name);
    destroy_type_node(data_type);
    return ret;
  }

  *node = create_parameter_node(name, data_type);
  free(name);
  if (*node == NULL) {
    destroy_type_node(data_type);
    return INTERNAL_ERR;
  }
  return NO_ERR;
}

#pragma endregion

#pragma region BLOCK BODY

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
      *node = NULL;
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
    *node = NULL;
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_node(*node);
    *node = NULL;
  }
  return ret;
}

err_code_t block(token_t *token, ast_node_t **node) {
  if (token->type == TOK_DEDENT) {
    return NO_ERR;
  }
  err_code_t ret = stmt(token, node);
  if (ret != NO_ERR) {
    return ret;
  }

  ast_node_t *statement = NULL;
  ret = stmt_tail(token, &statement);
  if (ret != NO_ERR) {
    destroy_node(*node);
    *node = NULL;
    return ret;
  }
  node_append(*node, statement);

  ast_node_t *statement2 = NULL;
  ret = block(token, &statement2);
  if (ret != NO_ERR) {
    destroy_node(*node);
    *node = NULL;
    return ret;
  }
  node_append(*node, statement2);

  return NO_ERR;
}

#pragma endregion

#pragma region STATEMENTS

err_code_t stmt_tail(token_t *token, ast_node_t **node) {
  if (token->type == TOK_EOL) {
    token_clear(token);
    return lexer_get_token(token, FORBIDDEN);
  }
  if (token->type != TOK_SEMICOLON) {
    return NO_ERR;
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
    *node = NULL;
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

  case TOK_KW_VAL:
    return var_dec(token, node, true);

  case TOK_KW_VAR:
    return var_dec(token, node, false);

  case TOK_KW_RETURN:
    token_clear(token);
    ret = lexer_get_token(token, FORBIDDEN);
    if (ret != NO_ERR) {
      return ret;
    }

    ast_node_t *expression = NULL;
    ret = expr(token, &expression);
    if (ret != NO_ERR) {
      return ret;
    }

    *node = create_return_node(expression);
    if (*node == NULL) {
      destroy_node(expression);
      return INTERNAL_ERR;
    }
    return NO_ERR;

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
  if (token->type == TOK_EOL) {
    token_clear(token);
    ret = lexer_get_token(token, FORBIDDEN);
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
  if (token->type != TOK_EOL) {
    return stmt(token, node);
  }

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
    *node = NULL;
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_node(*node);
    *node = NULL;
  }
  return ret;
}

err_code_t id_stmt(token_t *token, ast_node_t **node) {
  token_t *tmp_token = token_create();
  err_code_t ret = lexer_peek_token(tmp_token, ALLOWED, 0);
  if (ret != NO_ERR) {
    token_free(tmp_token);
    return ret;
  }

  if (tmp_token->type == TOK_ASSIGN) {
    token_free(tmp_token);
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
    token_clear(token);
    ret = lexer_get_token(token, FORBIDDEN);
    if (ret != NO_ERR) {
      free(name);
      return ret;
    }

    ast_node_t *expression = NULL;
    ret = expr(token, &expression);
    if (ret != NO_ERR) {
      free(name);
      return ret;
    }

    *node = create_assignment_node(name, expression);
    free(name);
    if (*node == NULL) {
      destroy_node(expression);
      return INTERNAL_ERR;
    }
    return NO_ERR;
  }
  token_free(tmp_token);
  ast_node_t *expression = NULL;
  ret = expr(token, &expression);
  if (ret != NO_ERR) {
    return ret;
  }

  ret = expr_tail(token, node, &expression);
  if (ret != NO_ERR) {
    destroy_node(expression);
  }
  return ret;
}

err_code_t var_dec(token_t *token, ast_node_t **node, bool is_const) {
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

  data_type_node_t *data_type = NULL;
  ret = ret_type(token, &data_type);
  if (ret != NO_ERR) {
    free(name);
    return ret;
  }

  if (token->type != TOK_ASSIGN) {
    free(name);
    destroy_type_node(data_type);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    free(name);
    destroy_type_node(data_type);
    return ret;
  }

  ast_node_t *expression = NULL;
  ret = expr(token, &expression);
  if (ret != NO_ERR) {
    free(name);
    destroy_type_node(data_type);
    return ret;
  }

  *node =
      create_variable_declaration_node(name, data_type, is_const, expression);

  free(name);
  if (*node == NULL) {
    destroy_type_node(data_type);
    destroy_node(expression);
    return SYNTAX_ERR;
  }
  return NO_ERR;
}

#pragma endregion

#pragma region LOOPS

err_code_t for_loop(token_t *token, ast_node_t **node) {
  ast_node_t *iterator = NULL;
  ast_node_t *generator = NULL;
  ast_node_t *guards = NULL;
  ast_node_t *body = NULL;
  err_code_t ret;
  if (token->type != TOK_EOL) {
    ret = gen(token, &iterator, &generator);
    if (ret != NO_ERR) {
      return ret;
    }

    ret = for_tail(token, &body);
    if (ret != NO_ERR) {
      destroy_node(iterator);
      destroy_node(generator);
      return ret;
    }

    *node = create_for_do_node(iterator, generator, guards, body);
    if (*node == NULL) {
      destroy_node(iterator);
      destroy_node(generator);
      destroy_node(body);
      return INTERNAL_ERR;
    }
    return NO_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
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

  ret = gen(token, &iterator, &generator);
  if (ret != NO_ERR) {
    return ret;
  }

  ret = guard_list(token, &guards);
  if (ret != NO_ERR) {
    destroy_node(iterator);
    destroy_node(generator);
    return ret;
  }

  ret = for_tail(token, &body);
  if (ret != NO_ERR) {
    destroy_node(iterator);
    destroy_node(generator);
    destroy_node(guards);
    return ret;
  }

  *node = create_for_do_node(iterator, generator, guards, body);
  if (*node == NULL) {
    destroy_node(iterator);
    destroy_node(generator);
    destroy_node(body);
    destroy_node(guards);
    return INTERNAL_ERR;
  }

  if (token->type != TOK_DEDENT) {
    destroy_node(*node);
    *node = NULL;
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_node(*node);
    *node = NULL;
  }

  return ret;
}

err_code_t gen(token_t *token, ast_node_t **iterator, ast_node_t **generator) {
  if (token->type != TOK_IDENTIFIER) {
    return SYNTAX_ERR;
  }

  char *name = string_duplicate(token->value);
  if (name == NULL) {
    return INTERNAL_ERR;
  }

  *iterator = create_parameter_node(name, NULL);
  free(name);
  if (*iterator == NULL) {
    return INTERNAL_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    destroy_node(*iterator);
    return ret;
  }

  if (token->type != TOK_ARROW_LEFT) {
    destroy_node(*iterator);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    destroy_node(*iterator);
    return ret;
  }

  ast_node_t *expression = NULL;
  ret = expr(token, &expression);
  if (ret != NO_ERR) {
    destroy_node(*iterator);
    return ret;
  }

  ret = gen_tail(token, expression, generator);
  if (ret != NO_ERR) {
    destroy_node(*iterator);
    return ret;
  }

  return NO_ERR;
}

err_code_t gen_tail(token_t *token, ast_node_t *expression,
                    ast_node_t **generator) {

  if (token->type == TOK_KW_DO) {
    *generator = expression;
    return NO_ERR;
  }

  if (token->type == TOK_IDENTIFIER && strcmp(token->value, "to") == 0) {

    token_clear(token);
    err_code_t ret = lexer_get_token(token, FORBIDDEN);
    if (ret != NO_ERR) {
      destroy_node(expression);
      return ret;
    }

    ast_node_t *to = NULL;
    ret = expr(token, &to);
    if (ret != NO_ERR) {
      destroy_node(expression);
      return ret;
    }

    node_append(expression, to);

    if (token->type == TOK_IDENTIFIER && strcmp(token->value, "by") == 0) {
      token_clear(token);
      ret = lexer_get_token(token, FORBIDDEN);
      if (ret != NO_ERR) {
        destroy_node(expression);
        return ret;
      }

      ast_node_t *step = NULL;
      ret = expr(token, &step);
      if (ret != NO_ERR) {
        destroy_node(expression);
        return ret;
      }
      node_append(expression, step);
    }

    ast_node_t *arguments = create_argument_list_node(expression);
    if (arguments == NULL) {
      destroy_node(expression);
      return INTERNAL_ERR;
    }

    *generator = create_function_call_node("to", arguments);
    if (*generator == NULL) {
      destroy_node(arguments);
      return INTERNAL_ERR;
    }
    return NO_ERR;
  }

  if (token->type == TOK_IDENTIFIER && strcmp(token->value, "until") == 0) {
    token_clear(token);
    err_code_t ret = lexer_get_token(token, FORBIDDEN);
    if (ret != NO_ERR) {
      destroy_node(expression);
      return ret;
    }

    ast_node_t *until = NULL;
    ret = expr(token, &until);
    if (ret != NO_ERR) {
      destroy_node(expression);
      return ret;
    }
    node_append(expression, until);

    ast_node_t *arguments = create_argument_list_node(expression);
    if (arguments == NULL) {
      destroy_node(expression);
      return INTERNAL_ERR;
    }

    *generator = create_function_call_node("until", arguments);
    if (*generator == NULL) {
      destroy_node(arguments);
      return INTERNAL_ERR;
    }
    return NO_ERR;
  }

  destroy_node(expression);
  return SYNTAX_ERR;
}

err_code_t guard_list(token_t *token, ast_node_t **node) {
  if (token->type == TOK_KW_DO) {
    return NO_ERR;
  }

  err_code_t ret = if_guard(token, node);
  if (ret != NO_ERR) {
    return ret;
  }

  if (token->type != TOK_EOL) {
    destroy_node(*node);
    *node = NULL;
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    destroy_node(*node);
    *node = NULL;
    return ret;
  }

  ret = guard_list(token, &(*node)->next);
  if (ret != NO_ERR) {
    destroy_node(*node);
    *node = NULL;
  }
  return ret;
}

err_code_t for_tail(token_t *token, ast_node_t **node) {
  if (token->type != TOK_KW_DO) {
    return SYNTAX_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    return ret;
  }

  ast_node_t *statement = NULL;

  if (token->type != TOK_EOL) {
    ret = stmt(token, &statement);
    if (ret != NO_ERR) {
      return ret;
    }

    *node = create_block_node(statement);
    if (*node == NULL) {
      destroy_node(statement);
      return INTERNAL_ERR;
    }
    return NO_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
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
    *node = NULL;
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_node(*node);
    *node = NULL;
  }

  return ret;
}

err_code_t while_loop(token_t *token, ast_node_t **node) {
  ast_node_t *condition = NULL;
  err_code_t ret = expr(token, &condition);
  if (ret != NO_ERR) {
    return ret;
  }

  if (token->type != TOK_KW_DO) {
    destroy_node(condition);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, REQUIRED);
  if (ret != NO_ERR) {
    destroy_node(condition);
    return ret;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    destroy_node(condition);
    return ret;
  }

  if (token->type != TOK_INDENT) {
    destroy_node(condition);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    destroy_node(condition);
    return ret;
  }

  ast_node_t *statement = NULL;
  ret = block(token, &statement);
  if (ret != NO_ERR) {
    destroy_node(condition);
    return ret;
  }

  ast_node_t *body = create_block_node(statement);
  if (body == NULL) {
    destroy_node(condition);
    destroy_node(statement);
    return INTERNAL_ERR;
  }

  *node = create_while_do_node(condition, body);
  if (*node == NULL) {
    destroy_node(condition);
    destroy_node(body);
    return INTERNAL_ERR;
  }

  if (token->type != TOK_DEDENT) {
    destroy_node(*node);
    *node = NULL;
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_node(*node);
    *node = NULL;
  }
  return ret;
}

#pragma endregion

#pragma region CASE

err_code_t case_list(token_t *token, ast_node_t **node) {
  if (token->type != TOK_KW_CASE) {
    return NO_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }

  ast_node_t *pattern = NULL;
  ret = case_param(token, &pattern);
  if (ret != NO_ERR) {
    return ret;
  }

  ast_node_t *guard = NULL;
  ast_node_t *expression = NULL;
  ret = case_tail(token, &guard, &expression);
  if (ret != NO_ERR) {
    destroy_node(pattern);
    return ret;
  }

  *node = create_match_case_node(pattern, guard, expression);
  if (*node == NULL) {
    destroy_node(pattern);
    destroy_node(guard);
    destroy_node(expression);
  }

  ret = case_list(token, &(*node)->next);
  if (ret != NO_ERR) {
    destroy_node(*node);
    *node = NULL;
  }
  return ret;
}

err_code_t case_tail(token_t *token, ast_node_t **guard,
                     ast_node_t **expression) {
  err_code_t ret;
  if (token->type == TOK_KW_IF) {
    ret = if_guard(token, guard);
    if (ret != NO_ERR) {
      return ret;
    }
  }

  if (token->type != TOK_ARROW_RIGHT) {
    destroy_node(*guard);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_node(*guard);
    return ret;
  }

  ret = fun_body(token, expression);
  if (ret != NO_ERR) {
    destroy_node(*guard);
  }
  return ret;
}

err_code_t case_param(token_t *token, ast_node_t **node) {
  token_t *tmp_token;
  switch (token->type) {
  case TOK_LIT_INT:
  case TOK_LIT_DOUBLE:
  case TOK_LIT_STRING:
  case TOK_LIT_M_STRING:
  case TOK_LIT_TRUE:
  case TOK_LIT_FALSE:
  case TOK_UNDERSCORE:
    *node = create_leaf_node(token);
    if (*node == NULL) {
      return INTERNAL_ERR;
    }

    token_clear(token);
    err_code_t ret = lexer_get_token(token, FORBIDDEN);
    if (ret != NO_ERR) {
      destroy_node(*node);
      *node = NULL;
    }
    return ret;

  case TOK_IDENTIFIER:
    tmp_token = token_create();
    if (tmp_token == NULL) {
      return INTERNAL_ERR;
    }

    ret = lexer_peek_token(tmp_token, FORBIDDEN, 0);
    if (ret != NO_ERR) {
      token_free(tmp_token);
      return ret;
    }

    if (tmp_token->type != TOK_COLON) {
      token_free(tmp_token);
      return fun_call(token, node);
    }
    token_free(tmp_token);

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

    token_clear(token);
    ret = lexer_get_token(token, FORBIDDEN);
    if (ret != NO_ERR) {
      free(name);
      return ret;
    }

    data_type_node_t *data_type = NULL;
    ret = type(token, &data_type);
    if (ret != NO_ERR) {
      free(name);
      return ret;
    }

    *node = create_parameter_node(name, data_type);
    free(name);
    if (*node == NULL) {
      destroy_type_node(data_type);
      return INTERNAL_ERR;
    }
    return NO_ERR;

  default:
    return SYNTAX_ERR;
  }
}

err_code_t if_guard(token_t *token, ast_node_t **node) {
  if (token->type != TOK_KW_IF) {
    return SYNTAX_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }

  return expr(token, node);
}

#pragma endregion

#pragma region ARGUMENTS

err_code_t arg_lists(token_t *token, ast_node_t **node) {
  if (token->type != TOK_L_PAREN) {
    return NO_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    return ret;
  }

  ast_node_t *arguments = NULL;
  ret = args(token, &arguments);
  if (ret != NO_ERR) {
    return ret;
  }

  if (token->type != TOK_R_PAREN) {
    destroy_node(arguments);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_node(arguments);
    return ret;
  }

  *node = create_argument_list_node(arguments);
  if (*node == NULL) {
    destroy_node(arguments);
    return INTERNAL_ERR;
  }

  ret = arg_lists(token, &(*node)->next);
  if (ret != NO_ERR) {
    destroy_node(*node);
    *node = NULL;
  }
  return ret;
}

err_code_t args(token_t *token, ast_node_t **node) {
  if (token->type == TOK_R_PAREN) {
    return NO_ERR;
  }

  err_code_t ret = expr(token, node);
  if (ret != NO_ERR) {
    return ret;
  }

  ret = args_tail(token, &(*node)->next);
  if (ret != NO_ERR) {
    destroy_node(*node);
    *node = NULL;
  }
  return ret;
}

err_code_t args_tail(token_t *token, ast_node_t **node) {
  if (token->type == TOK_R_PAREN) {
    return NO_ERR;
  }

  if (token->type != TOK_COMMA) {
    return SYNTAX_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    return ret;
  }

  ret = expr(token, node);
  if (ret != NO_ERR) {
    return ret;
  }

  ret = args_tail(token, &(*node)->next);
  if (ret != NO_ERR) {
    destroy_node(*node);
    *node = NULL;
  }
  return ret;
}

#pragma endregion

#pragma region EXPRESSION

// helper
bool is_end(token_t *token) {
  switch (token->type) {
  case TOK_IDENTIFIER:
  case TOK_LIT_INT:
  case TOK_LIT_DOUBLE:
  case TOK_LIT_STRING:
  case TOK_LIT_M_STRING:
  case TOK_LIT_TRUE:
  case TOK_LIT_FALSE:
  case TOK_L_PAREN:
  case TOK_UNARY_MINUS:
  case TOK_UNARY_PLUS:
  case TOK_NOT:
  case TOK_MUL:
  case TOK_DIV:
  case TOK_BINARY_PLUS:
  case TOK_BINARY_MINUS:
  case TOK_CONCAT_LIST:
  case TOK_LOWER_T:
  case TOK_LOWER_EQ:
  case TOK_GREATER_T:
  case TOK_GREATER_EQ:
  case TOK_EQUAL:
  case TOK_NOT_EQ:
  case TOK_AND:
  case TOK_OR:
  case TOK_R_PAREN:
    return false;

  default:
    return true;
  }
}

err_code_t expr(token_t *token, ast_node_t **node) {
  token_stack_t *stack = create_token_stack();
  if (stack == NULL) {
    return INTERNAL_ERR;
  }
  err_code_t ret = NO_ERR;
  bool end = false;

  while (!end) {
    ast_node_t *nonterm = NULL;

    ret = expr_pre_parse(token, &nonterm);

    if (ret != NO_ERR) {
      destroy_token_stack(stack);
      return ret;
    }

    if (nonterm != NULL) {
      if (!push_nonterm(stack, nonterm)) {
        destroy_node(nonterm);
        destroy_token_stack(stack);
        return INTERNAL_ERR;
      }
      end = is_end(token);
      continue;
    }

    switch (get_action(token->type, get_top_token_type(stack))) {
    case SHIFT:
      if (!push_token(stack, token)) {
        destroy_token_stack(stack);
        return INTERNAL_ERR;
      }

      token_clear(token);
      ret = lexer_get_token(token, ALLOWED);
      if (ret != NO_ERR) {
        destroy_token_stack(stack);
        return ret;
      }
      break;

    case REDUCE:
      if (!reduce(stack)) {
        destroy_token_stack(stack);
        return SYNTAX_ERR;
      }
      break;

    case CONSUME_PAREN:
      if (!remove_paren(stack)) {
        destroy_token_stack(stack);
        return SYNTAX_ERR;
      }

      token_clear(token);
      ret = lexer_get_token(token, ALLOWED);
      if (ret != NO_ERR) {
        destroy_token_stack(stack);
        return ret;
      }
      break;

    case ERROR:
      destroy_token_stack(stack);
      return SYNTAX_ERR;

    case END:
      end = true;
      continue;

    default:
      break;
    }
    end = is_end(token);
  }

  while (!is_token_empty(stack)) {
    if (!reduce(stack)) {
      destroy_token_stack(stack);
      return SYNTAX_ERR;
    }
  }

  *node = return_final_node(stack);
  destroy_token_stack(stack);
  if (*node == NULL) {
    return INTERNAL_ERR;
  }
  return NO_ERR;
}

err_code_t expr_tail(token_t *token, ast_node_t **node,
                     ast_node_t **expression) {
  if (token->type != TOK_KW_MATCH) {
    *node = *expression;
    return NO_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, REQUIRED);
  if (ret != NO_ERR) {
    return ret;
  }
  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
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

  ast_node_t *cases = NULL;
  ret = case_list(token, &cases);
  if (ret != NO_ERR) {
    return ret;
  }

  if (token->type != TOK_DEDENT) {
    destroy_node(cases);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_node(cases);
    return ret;
  }

  *node = create_match_node(*expression, cases);
  if (*node == NULL) {
    destroy_node(cases);
    return INTERNAL_ERR;
  }
  return NO_ERR;
}

// this handles funcall, dotcall, if stmt, unit
err_code_t expr_pre_parse(token_t *token, ast_node_t **node) {
  token_t *tmp_token;
  err_code_t ret;
  tmp_token = token_create();
  if (tmp_token == NULL) {
    return INTERNAL_ERR;
  }
  ret = lexer_peek_token(tmp_token, ALLOWED, 0);
  if (ret != NO_ERR) {
    token_free(tmp_token);
    return ret;
  }

  if (tmp_token->type == TOK_DOT) {
    token_free(tmp_token);
    return dot_call(token, node);
  }

  switch (token->type) {
  case TOK_LIT_INTERP_STRING_PART:
    token_free(tmp_token);
    return interpol(token, node);
  case TOK_LIT_INTERP_M_STRING_PART:
    token_free(tmp_token);
    return interpol_m(token, node);
    break;

  case TOK_IDENTIFIER:
    tmp_token = token_create();
    if (tmp_token == NULL) {
      return INTERNAL_ERR;
    }
    ret = lexer_peek_token(tmp_token, ALLOWED, 0);
    if (ret != NO_ERR) {
      token_free(tmp_token);
      return ret;
    }
    if (tmp_token->type == TOK_L_PAREN) {
      token_free(tmp_token);
      return fun_call(token, node);
    }
    token_free(tmp_token);
    return NO_ERR;

  case TOK_KW_IF:
    token_free(tmp_token);
    token_clear(token);
    ret = lexer_get_token(token, FORBIDDEN);
    if (ret != NO_ERR) {
      return ret;
    }
    return if_stmt(token, node);

  case TOK_L_PAREN:
    // () gets converted to unit
    if (tmp_token->type == TOK_R_PAREN) {
      token_free(tmp_token);
      token_clear(token);
      ret = lexer_get_token(token, FORBIDDEN);
      if (ret != NO_ERR) {
        return ret;
      }
      token_clear(token);
      ret = lexer_get_token(token, ALLOWED);
      if (ret != NO_ERR) {
        return ret;
      }

      *node = create_unit_literal_node();
      if (*node == NULL) {
        return INTERNAL_ERR;
      }
      return NO_ERR;
    }

    if (tmp_token->type != TOK_IDENTIFIER) {
      token_free(tmp_token);
      return NO_ERR;
    }

    token_clear(tmp_token);
    ret = lexer_peek_token(tmp_token, ALLOWED, 1);
    if (ret != NO_ERR) {
      token_free(tmp_token);
      return ret;
    }

    if (tmp_token->type != TOK_COLON) {
      token_free(tmp_token);
      return NO_ERR;
    }
    token_free(tmp_token);
    return lambda(token, node);

  case TOK_KW_LIST:
    if (tmp_token->type == TOK_DOT) {
      token_free(tmp_token);
      return dot_call(token, node);
    }
    token_free(tmp_token);
    return list(token, node);

  default:
    token_free(tmp_token);
    return NO_ERR;
  }
}

err_code_t fun_call(token_t *token, ast_node_t **node) {
  if (token->type != TOK_IDENTIFIER) {
    return SYNTAX_ERR;
  }

  char *name = string_duplicate(token->value);
  if (name == NULL) {
    return INTERNAL_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }

  ast_node_t *argument_lists = NULL;
  ret = arg_lists(token, &argument_lists);
  if (ret != NO_ERR) {
    free(name);
    return ret;
  }

  *node = create_function_call_node(name, argument_lists);

  free(name);
  if (*node == NULL) {
    destroy_node(argument_lists);
    return INTERNAL_ERR;
  }
  return NO_ERR;
}

err_code_t dot_call(token_t *token, ast_node_t **node) {
  ast_node_t *target = create_leaf_node(token);
  if (target == NULL) {
    return INTERNAL_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }

  if (token->type != TOK_DOT) {
    destroy_node(target);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }

  if (token->type != TOK_IDENTIFIER) {
    destroy_node(target);
    return SYNTAX_ERR;
  }

  char *name = string_duplicate(token->value);
  if (name == NULL) {
    destroy_node(target);
    return INTERNAL_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_node(target);
    return ret;
  }

  ast_node_t *argument_lists = NULL;
  ret = arg_lists(token, &argument_lists);
  if (ret != NO_ERR) {
    destroy_node(target);
    free(name);
    return ret;
  }

  ast_node_t *new_target =
      create_dot_function_call_node(name, target, argument_lists);

  if (new_target == NULL) {
    destroy_node(target);
    destroy_node(argument_lists);
    free(name);
    return INTERNAL_ERR;
  }

  return dot_call_tail(token, node, new_target);
}

err_code_t dot_call_tail(token_t *token, ast_node_t **node,
                         ast_node_t *target) {
  if (token->type != TOK_DOT) {
    *node = target;
    return NO_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }

  if (token->type != TOK_IDENTIFIER) {
    destroy_node(target);
    return SYNTAX_ERR;
  }

  char *name = string_duplicate(token->value);
  if (name == NULL) {
    destroy_node(target);
    return INTERNAL_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_node(target);
    return ret;
  }

  ast_node_t *argument_lists = NULL;
  ret = arg_lists(token, &argument_lists);
  if (ret != NO_ERR) {
    destroy_node(target);
    free(name);
    return ret;
  }

  ast_node_t *new_target =
      create_dot_function_call_node(name, target, argument_lists);

  if (new_target == NULL) {
    destroy_node(target);
    destroy_node(argument_lists);
    free(name);
    return INTERNAL_ERR;
  }

  ret = dot_call_tail(token, node, new_target);
  if (ret != NO_ERR) {
    destroy_node(new_target);
  }
  return NO_ERR;
}

err_code_t list(token_t *token, ast_node_t **node) {
  token_clear(token);
  err_code_t ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    return ret;
  }

  if (token->type != TOK_L_PAREN) {
    return SYNTAX_ERR;
  }

  ast_node_t *argument_lists = NULL;
  ret = arg_lists(token, &argument_lists);
  if (ret != NO_ERR) {
    return ret;
  }

  *node = create_function_call_node("List", argument_lists);
  if (*node == NULL) {
    destroy_node(argument_lists);
    return INTERNAL_ERR;
  }
  return NO_ERR;
}

err_code_t interpol(token_t *token, ast_node_t **node) {
  ast_node_t *expressions = NULL;

  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }

  ret = interpol_part(token, &expressions);
  if (ret != NO_ERR) {
    destroy_node(expressions);
  }

  if (token->type != TOK_LIT_INTERP_STRING_END) {
    destroy_node(expressions);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_node(expressions);
    return ret;
  }

  *node = create_interpolated_string_node(expressions);
  if (*node == NULL) {
    destroy_node(expressions);
    return INTERNAL_ERR;
  }
  return NO_ERR;
}

err_code_t interpol_part(token_t *token, ast_node_t **node) {
  if (token->type == TOK_LIT_INTERP_STRING_END) {
    return NO_ERR;
  }
  if (token->type == TOK_LIT_INTERP_STRING_PART) {
    *node = create_leaf_node(token);
    if (*node == NULL) {
      return INTERNAL_ERR;
    }

    token_clear(token);
    err_code_t ret = lexer_get_token(token, FORBIDDEN);
    if (ret != NO_ERR) {
      destroy_node(*node);
      *node = NULL;
      return ret;
    }

    return interpol_part(token, &(*node)->next);
  }

  if (token->type != TOK_EXPRESSION_START) {
    return SYNTAX_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    return ret;
  }

  ret = expr(token, node);
  if (ret != NO_ERR) {
    return ret;
  }

  if (token->type != TOK_EXPRESSION_END) {
    destroy_node(*node);
    *node = NULL;
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, FORBIDDEN);
  if (ret != NO_ERR) {
    destroy_node(*node);
    *node = NULL;
    return ret;
  }

  return interpol_part(token, &(*node)->next);
}

err_code_t interpol_m(token_t *token, ast_node_t **node) {
  ast_node_t *expressions = NULL;

  token_clear(token);
  err_code_t ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    return ret;
  }

  ret = interpol_part_m(token, &expressions);
  if (ret != NO_ERR) {
    destroy_node(expressions);
  }

  if (token->type != TOK_LIT_INTERP_M_STRING_END) {
    destroy_node(expressions);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_node(expressions);
    return ret;
  }

  *node = create_interpolated_string_node(expressions);
  if (*node == NULL) {
    destroy_node(expressions);
    return INTERNAL_ERR;
  }
  return NO_ERR;
}

err_code_t interpol_part_m(token_t *token, ast_node_t **node) {
  if (token->type == TOK_LIT_INTERP_STRING_END) {
    return NO_ERR;
  }
  if (token->type == TOK_LIT_INTERP_M_STRING_PART) {
    *node = create_leaf_node(token);
    if (*node == NULL) {
      return INTERNAL_ERR;
    }

    token_clear(token);
    err_code_t ret = lexer_get_token(token, ALLOWED);
    if (ret != NO_ERR) {
      destroy_node(*node);
      *node = NULL;
      return ret;
    }

    return interpol_part(token, &(*node)->next);
  }

  if (token->type != TOK_EXPRESSION_START) {
    return SYNTAX_ERR;
  }

  token_clear(token);
  err_code_t ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    return ret;
  }

  ret = expr(token, node);
  if (ret != NO_ERR) {
    return ret;
  }

  if (token->type != TOK_EXPRESSION_END) {
    destroy_node(*node);
    *node = NULL;
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_node(*node);
    *node = NULL;
    return ret;
  }

  return interpol_part(token, &(*node)->next);
}

err_code_t lambda(token_t *token, ast_node_t **node) {
  token_clear(token);
  err_code_t ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    return ret;
  }

  ast_node_t *parameters = NULL;
  ret = params(token, &parameters);
  if (ret != NO_ERR) {
    return ret;
  }

  if (token->type != TOK_R_PAREN) {
    destroy_node(parameters);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_node(parameters);
    return ret;
  }

  if (token->type != TOK_ARROW_RIGHT) {
    destroy_node(parameters);
    return SYNTAX_ERR;
  }

  token_clear(token);
  ret = lexer_get_token(token, ALLOWED);
  if (ret != NO_ERR) {
    destroy_node(parameters);
    return ret;
  }

  ast_node_t *body = NULL;
  ret = fun_body(token, &body);
  if (ret != NO_ERR) {
    destroy_node(parameters);
    return ret;
  }

  *node = create_lambda_declaration_node(parameters, body);
  if (*node == NULL) {
    destroy_node(parameters);
    destroy_node(body);
    return INTERNAL_ERR;
  }
  return NO_ERR;
}
