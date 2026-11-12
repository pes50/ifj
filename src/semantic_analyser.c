/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file semantic_analyser.c
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Implementation of analyse function used in semantic analysis
 *
\*****************************************************************************/

#include "semantic_analyser.h"
#include "analyse_callable_declarations.h"
#include "analyse_types.h"
#include "collect_symbols.h"
#include <stdlib.h>

err_code_t analyse(ast_node_t *root, hashtable_t *symtable,
                   char **main_identifier) {
  if (root == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  err_code_t err = collect_symbols(root, symtable, main_identifier);
  if (err != NO_ERR) {
    return err;
  }

  if (*main_identifier == NULL) {
    return UNDEF_SYMBOL_ERR;
  }

  ast_node_t *current = root->data.block.statement;
  ast_node_t *node = NULL;
  while (current != NULL) {
    switch (current->type) {
    case AST_FUNCTION_DECLARATION:
      err = analyse_function_declaration(current, symtable);
      if (err != NO_ERR) {
        return err;
      }
      break;
    case AST_EXTENSION_DECLARATION:
      node = current->data.extension_declaration.function_declarations;
      while (node != NULL) {
        err = analyse_function_declaration(node, symtable);
        if (err == NO_ERR) {
          node = node->next;
        } else {
          return err;
        }
      }
      break;
    case AST_TYPE_DECLARATION:
      err = analyse_type_declaration(current, symtable);
      break;
    case AST_CLASS_DECLARATION:
      err = analyse_class_declaration(current, symtable);
      break;
    default:
      return OTHER_SEMANTIC_ERR;
    }
    if (err == NO_ERR) {
      current = current->next;
    } else {
      return err;
    }
  }

  return NO_ERR;
}
