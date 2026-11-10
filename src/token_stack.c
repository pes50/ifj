/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file token_stack.c
 *
 * @author Matous Jahoda (xjahodm00)
 *
 * @brief Implementation for token stack
 *
\*****************************************************************************/

#include "token_stack.h"

#include "semantic_analyser.h"
#include "stack.h"

#include <stdio.h>
#include <stdlib.h>

typedef enum { TOKEN, NONTERMINAL } element_token_type_t;

// Element that is pushed into stack
typedef struct {
  element_token_type_t type;
  union {
    struct {
      token_t *token;
    } token;
    struct {
      t_ast_node *node;
    } nonterminal;
  };
} element_token_t;

// Internal token stack representation
struct token_stack_t {
  t_stack *top;
  uint32_t token_count;
};

bool push_nonterm(token_stack_t *stack, t_ast_node *node) {
  if (stack == NULL) {
    return false;
  }
  element_token_t *elem = malloc(sizeof(element_token_t));
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
  token_stack_t *stack;
  stack = malloc(sizeof(token_stack_t));
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
  while (stack_get_size(stack->top)) {
    element_token_t *elem = stack_pop(stack->top);
    switch (elem->type) {
    case NONTERMINAL:
      destroy_node(elem->nonterminal.node);
      break;

    case TOKEN:
      token_free(elem->token.token);
      break;
    }
    free(elem);
  }
  free(stack->top);
  free(stack);
}

bool push_token(token_stack_t *stack, token_t *token) {
  if (stack == NULL) {
    return false;
  }
  element_token_t *elem = malloc(sizeof(element_token_t));
  elem->type = TOKEN;

  elem->token.token = token;
  if (elem->token.token == NULL) {
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
  if (stack->top == NULL) {
    return false;
  }
  if (is_empty(stack)) {
    return true;
  }
  t_stack_element *elem = stack_get_top(stack->top);
  if (elem == NULL) {
    return false;
  }
  element_token_t *data = (element_token_t *)elem->data;
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
  data = (element_token_t *)elem->data;
  if (data == NULL) {
    return false;
  }
  if (data->type == NONTERMINAL) {
    return false;
  }
  if (data->token.token->type != TOK_L_PAREN) {
    return false;
  }
  // If second from top is opening paren, we can succesfully remove it
  data = stack_pop(stack->top);
  t_ast_node *node = data->nonterminal.node;
  free(data);
  data = stack_pop(stack->top);
  token_free(data->token.token);
  free(data);
  stack->token_count--;

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
  if (stack->top == NULL) {
    return false;
  }
  if (is_empty(stack)) {
    return true;
  }
  t_stack_element *elem = stack_get_top(stack->top);
  if (elem == NULL) {
    return false;
  }
  element_token_t *data = (element_token_t *)elem->data;
  if (data == NULL) {
    return false;
  }
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
      bool ret = push_nonterm(stack, create_leaf_node(data->token.token));
      token_free(data->token.token);
      free(data);
      stack->token_count--;
      return ret;

    default:
      return false;
    }
  }
  elem = elem->next;

  if (elem == NULL) {
    return false;
  }
  data = (element_token_t *)elem->data;
  if (data == NULL) {
    return false;
  }
  if (data->type == NONTERMINAL) {
    return false;
  }
  t_ast_node *node;
  bool ret;
  switch (data->token.token->type) {
  // E -> unop E
  case TOK_UNARY_MINUS:
  case TOK_UNARY_PLUS:
  case TOK_NOT:
    data = stack_pop(stack->top);
    node = data->nonterminal.node;
    free(data);
    data = stack_pop(stack->top);

    ret = push_nonterm(
        stack, create_unary_operation_node(data->token.token->type, node));

    token_free(data->token.token);
    free(data);
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
    elem = elem->next;

    if (elem == NULL) {
      return false;
    }
    data = (element_token_t *)elem->data;
    if (data == NULL) {
      return false;
    }
    if (data->type == TOKEN) {
      return false;
    }
    data = stack_pop(stack->top);
    node = data->nonterminal.node;
    free(data);

    data = stack_pop(stack->top);
    token_type_t type = data->token.token->type;
    token_free(data->token.token);
    free(data);

    data = stack_pop(stack->top);
    ret = push_nonterm(stack, create_binary_operation_node(
                                  type, data->nonterminal.node, node));
    free(data);
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
  if (stack->top == NULL || is_empty(stack)) {
    return TOK_ERR;
  }

  t_stack_element *elem = stack_get_top(stack->top);

  while (elem != NULL) {
    element_token_t *data = (element_token_t *)elem->data;
    if (data->type == TOKEN) {
      return data->token.token->type;
    }
    elem = elem->next;
  }
  return TOK_ERR;
}

bool is_empty(token_stack_t *stack) {
  if (stack == NULL) {
    return true;
  }
  return (stack->token_count == 0);
}

t_ast_node *return_final_node(token_stack_t *stack) {
  if (stack == NULL) {
    return NULL;
  }
  if (!is_empty(stack) || stack->top == NULL) {
    return NULL;
  }
  t_stack_element *elem = stack_get_top(stack->top);
  if (elem == NULL) {
    return NULL;
  }
  element_token_t *data = (element_token_t *)elem->data;
  return data->nonterminal.node;
}