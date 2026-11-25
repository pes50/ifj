/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file main.c
 *
 * @authors Matouš Jahoda     (xjahodm00)
 * @authors Tomáš Peňás       (xpenast00)
 * @authors Roman Skoupý      (xskoupr00)
 * @authors Simon Šmíd        (xsmidsi00)
 *
 * @brief Entrypoint of the compiler
 *
\*****************************************************************************/

#include "ast_convert.h"
#include "builtin.h"
#include "code_gen.h"
#include "parser.h"
#include "semantic_analyser.h"
#include <stdio.h>

int main() {
  ast_node_t *ast_root = NULL;
  err_code_t err = parse(&ast_root, stdin);

  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  hashtable_t *symtable = create_builtin_symtable();
  if (symtable == NULL) {
    destroy_node(ast_root);
    RETURN_ERR(INTERNAL_ERR);
  }

  ast_node_t *main_node = NULL;
  err = analyse(ast_root, symtable, &main_node);
  if (err != NO_ERR) {
    destroy_node(ast_root);
    ht_destruct(&symtable);
    RETURN_ERR(err);
  }

  inner_statement_t *inner_code = NULL;
  err = ast_convert(main_node, &inner_code);
  if (err != NO_ERR) {
    destroy_node(ast_root);
    ht_destruct(&symtable);
    RETURN_ERR(err);
  }

  codegen_print_program(inner_code, stdout);
  ht_destruct(&symtable);
  destroy_node(ast_root);
  destroy_statement_list(inner_code);

  return 0;
}