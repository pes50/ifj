/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file token_stack.c
 *
 * @author Matouš Jahoda (xjahodm00)
 *
 * @brief Implementation for token stack
 *
\*****************************************************************************/

#include "token_stack.h"

#include "semantic_analyser.h"
#include "stack.h"

#include <stdio.h>
#include <stdlib.h>

// Element that is pushed into stack
typedef struct {
  token_stack_element_type_t type;

  union {
    struct {
      token_t *token;
    } token;

    struct {
      ast_node_t *node;
    } nonterminal;
  };
} token_stack_element_t;

// Internal token stack representation
struct token_stack_t {
  t_stack *top;
  uint32_t token_count;
};

token_t *token_copy(token_t *token) {
  token_t *tmp_token = token_create();
  if (tmp_token == NULL) {
    return NULL;
  }

  token_set(tmp_token, token->type, token->value, token->length);
  return tmp_token;
}

bool push_nonterm(token_stack_t *stack, ast_node_t *node) {
  if (stack == NULL || node == NULL) {
    return false;
  }
  if (stack->top == NULL) {
    return false;
  }

  token_stack_element_t *elem = malloc(sizeof(token_stack_element_t));
  if (elem == NULL) {
    return false;
  }

  elem->type = NONTERMINAL;
  elem->nonterminal.node = node;

  if (!stack_push(stack->top, elem)) {
    free(elem);
    return false;
  }
  return true;
}

token_stack_t *create_token_stack() {
  token_stack_t *stack = malloc(sizeof(token_stack_t));
  if (stack == NULL) {
    return NULL;
  }

  stack->top = create_stack();
  if (stack->top == NULL) {
    free(stack);
    return NULL;
  }

  stack->token_count = 0;
  return stack;
}

void destroy_token_stack(token_stack_t *stack) {
  if (stack == NULL) {
    return;
  }
  // Pop and free all elements and their internal data
  while (stack_get_size(stack->top) > 0) {
    token_stack_element_t *elem = stack_pop(stack->top);
    switch (elem->type) {
    case NONTERMINAL:
      destroy_node(elem->nonterminal.node);
      break;

    case TOKEN:
      token_free(elem->token.token);
      break;
    default:
      break;
    }
    free(elem);
  }

  free(stack->top);
  free(stack);
}

bool push_token(token_stack_t *stack, token_t *token) {
  if (stack == NULL || token == NULL) {
    return false;
  }
  if (stack->top == NULL) {
    return false;
  }

  token_stack_element_t *elem = malloc(sizeof(token_stack_element_t));
  if (elem == NULL) {
    return false;
  }

  elem->type = TOKEN;
  elem->token.token = token_copy(token);
  if (elem->token.token == NULL) {
    free(elem);
    return false;
  }

  if (!stack_push(stack->top, elem)) {
    free(elem);
    return false;
  }

  stack->token_count++;
  return true;
}

bool remove_paren(token_stack_t *stack) {
  if (stack == NULL) {
    return false;
  }
  if (stack->top == NULL || is_token_empty(stack)) {
    return false;
  }

  t_stack_element *elem = stack_get_top(stack->top);
  if (elem == NULL) {
    return false;
  }

  token_stack_element_t *data = (token_stack_element_t *)elem->data;
  if (data == NULL) {
    return false;
  }

  if (data->type == TOKEN) {
    return false;
  }

  elem = elem->next;
  if (elem == NULL) {
    return false;
  }

  data = (token_stack_element_t *)elem->data;
  if (data == NULL) {
    return false;
  }

  // second from top should be opening paren
  if (data->type == NONTERMINAL) {
    return false;
  }
  if (data->token.token->type != TOK_L_PAREN) {
    return false;
  }

  // If second from top is opening paren, we can succesfully remove it
  data = stack_pop(stack->top);
  ast_node_t *node = data->nonterminal.node;
  free(data);

  data = stack_pop(stack->top);

  token_free(data->token.token);
  free(data);

  stack->token_count--;

  // push the AST node back on stack
  if (!push_nonterm(stack, node)) {
    destroy_node(node);
    return false;
  }
  return true;
}

bool reduce(token_stack_t *stack) {
  if (stack == NULL) {
    return false;
  }
  if (stack->top == NULL || is_token_empty(stack)) {
    return false;
  }

  t_stack_element *elem = stack_get_top(stack->top);
  if (elem == NULL) {
    return false;
  }

  token_stack_element_t *data = (token_stack_element_t *)elem->data;
  if (data == NULL) {
    return false;
  }

  // if top is token, create a lef node
  if (data->type == TOKEN) {
    switch (data->token.token->type) {
    // E -> i
    case TOK_IDENTIFIER:
    case TOK_LIT_INT:
    case TOK_LIT_DOUBLE:
    case TOK_LIT_STRING:
    case TOK_LIT_M_STRING:
    case TOK_LIT_TRUE:
    case TOK_LIT_FALSE:
      stack_pop(stack->top);
      ast_node_t *node = create_leaf_node(data->token.token);

      token_free(data->token.token);
      free(data);

      if (node == NULL) {
        return false;
      }

      bool ret = push_nonterm(stack, node);
      if (!ret) {
        destroy_node(node);
      }

      stack->token_count--;
      return ret;

    default:
      return false;
    }
  }

  // top is NONTERM, check next
  elem = elem->next;
  if (elem == NULL) {
    return false;
  }

  data = (token_stack_element_t *)elem->data;
  if (data == NULL) {
    return false;
  }

  // cant have two NONTERMS in a row
  if (data->type == NONTERMINAL) {
    return false;
  }
  // helper variables
  ast_node_t *right;
  ast_node_t *left;
  ast_node_t *node;
  bool ret;
  token_type_t type;

  // decide if it is binary op or unary op
  switch (data->token.token->type) {
  // E -> unop E
  case TOK_UNARY_MINUS:
  case TOK_UNARY_PLUS:
  case TOK_NOT:
    // save AST node
    data = stack_pop(stack->top);
    right = data->nonterminal.node;

    free(data);

    if (right == NULL) {
      return false;
    }

    // save operation type
    data = stack_pop(stack->top);
    type = data->token.token->type;

    token_free(data->token.token);
    free(data);

    // create new AST node and push it onto stack
    node = create_unary_operation_node(type, right);
    if (node == NULL) {
      destroy_node(right);
      return false;
    }

    ret = push_nonterm(stack, node);

    if (!ret) {
      destroy_node(node);
    }

    stack->token_count--;
    return ret;

  // E -> E binop E
  case TOK_BINARY_PLUS:
  case TOK_BINARY_MINUS:
  case TOK_MUL:
  case TOK_DIV:
  case TOK_LOWER_T:
  case TOK_LOWER_EQ:
  case TOK_GREATER_T:
  case TOK_GREATER_EQ:
  case TOK_EQUAL:
  case TOK_NOT_EQ:
  case TOK_AND:
  case TOK_OR:
  case TOK_CONCAT_LIST:
    // check third element, must be NONTERM
    elem = elem->next;

    if (elem == NULL) {
      return false;
    }

    data = (token_stack_element_t *)elem->data;
    if (data == NULL) {
      return false;
    }

    if (data->type == TOKEN) {
      return false;
    }

    // save right node of AST
    data = stack_pop(stack->top);
    right = data->nonterminal.node;

    free(data);

    if (right == NULL) {
      return false;
    }

    // save operation type
    data = stack_pop(stack->top); // cant be NULL, already checked
    type = data->token.token->type;

    token_free(data->token.token);
    free(data);

    // save left node of AST
    data = stack_pop(stack->top); // cant be NULL, already checked
    left = data->nonterminal.node;

    free(data);

    if (left == NULL) {
      destroy_node(right);
      return false;
    }

    // create new AST node and push it onto stack
    node = create_binary_operation_node(type, left, right);
    if (node == NULL) {
      destroy_node(right);
      destroy_node(left);
      return false;
    }

    ret = push_nonterm(stack, node);
    if (!ret) {
      destroy_node(node);
      return false;
    }

    stack->token_count--;
    return ret;

  default:
    return false;
  }
}

token_type_t get_top_token_type(token_stack_t *stack) {
  if (stack == NULL) {
    return TOK_ERR;
  }
  if (stack->top == NULL || is_token_empty(stack)) {
    return TOK_ERR;
  }

  t_stack_element *elem = stack_get_top(stack->top);

  while (elem != NULL) {
    token_stack_element_t *data = (token_stack_element_t *)elem->data;
    if (data->type == TOKEN) {
      return data->token.token->type;
    }
    elem = elem->next;
  }
  return TOK_ERR;
}

bool is_token_empty(token_stack_t *stack) {
  if (stack == NULL) {
    return true;
  }
  return (stack->token_count == 0);
}

ast_node_t *return_final_node(token_stack_t *stack) {
  if (stack == NULL) {
    return NULL;
  }
  if (!is_token_empty(stack) || stack->top == NULL) {
    return NULL;
  }

  // only return node if it is the last element in stack
  if (stack_get_size(stack->top) != 1) {
    return NULL;
  }

  token_stack_element_t *data = stack_pop(stack->top);
  if (data == NULL) {
    return NULL;
  }

  ast_node_t *node = data->nonterminal.node;

  free(data);

  return node;
}

token_stack_element_type_t peek_top_element_type(token_stack_t *stack) {
  if (stack == NULL) {
    return ERROR_OR_EMPTY;
  }
  if (stack_get_size(stack->top) == 0) {
    return ERROR_OR_EMPTY;
  }

  t_stack_element *elem = stack_get_top(stack->top);
  if (elem == NULL) {
    return ERROR_OR_EMPTY;
  }

  token_stack_element_t *data = (token_stack_element_t *)elem->data;
  if (data == NULL) {
    return ERROR_OR_EMPTY;
  }
  return data->type;
}

bool get_top_element(token_stack_t *stack, ast_node_t **node, token_t **token) {
  token_stack_element_t *data;
  switch (peek_top_element_type(stack)) {
  case ERROR_OR_EMPTY:
    return false;

  case TOKEN:
    if (token == NULL) {
      return false;
    }
    data = stack_pop(stack->top);
    *token = data->token.token;
    free(data);
    stack->token_count--;
    return true;

  case NONTERMINAL:
    if (node == NULL) {
      return false;
    }
    data = stack_pop(stack->top);
    *node = data->nonterminal.node;
    free(data);
    return true;
  }
  return false;
}
