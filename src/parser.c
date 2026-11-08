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
#include "lexer.h"
#include "token.h"
#include "token_stack.h"

#include <string.h> // strcmp

// NON terminals
err_code_t start(token_t *token, t_ast_node *node);
err_code_t prolog(token_t *token);
err_code_t prog(token_t *token, t_ast_node *node);
err_code_t expr(token_t *token, t_ast_node *node);

err_code_t parse() {
  token_t *token = token_create();
  if (token == NULL) {
    return INTERNAL_ERR;
  }

  t_ast_node *node;

  err_code_t ret = start(token, node);
  token_free(token);
  destroy_node(node); // should do smth with it first
  lexer_destroy();
  return ret;
}

err_code_t start(token_t *token, t_ast_node *node) {
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

err_code_t prog(token_t *token, t_ast_node *node) {
  switch (token->type) {
  case TOK_AT_MAIN:
  case TOK_KW_DEF:
    break;

  case TOK_KW_TYPE:
    break;

  case TOK_KW_CASE:
    break;

  case TOK_KW_EXTENSION:
    break;

  default:
    return SYNTAX_ERR;
  }
}

err_code_t expr(token_t *token, t_ast_node *node) {
  token_stack_t *stack = create_token_stack();
  if (stack == NULL) {
    return INTERNAL_ERR;
  }
  err_code_t err = NO_ERR;
  bool end = false;

  while (!end) {
    // TODO
    /* fisrt handle stupid stuff like funcall, if expr, INTERPOLATEDS,
     lamdas maybe?, List(), also unit maybe? ()
     */

    if (is_empty(stack)) {
      push_token(stack, token);
    }
    switch (get_action(token->type, get_top_token_type(stack))) {
    case SHIFT:
      push_token(stack, token);
      break;
    case REDUCE:
      reduce(stack);
      break;
    case CONSUME_PAREN:
      remove_paren(stack);
      break;
    case ERROR:
      end = true;
      err = SYNTAX_ERR;
      break;

    default:
      break;
    }
    lexer_get_token(token, ALLOWED); // err
    end = is_end(token);
  }
  node = return_final_node(stack);
  destroy_token_stack(stack);
  return err;
}
