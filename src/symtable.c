/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file symtable.c
 *
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Implementation of symbol table
 *
 \*****************************************************************************/

#include "symtable.h"

err_code_t ht_init_data(hashtable_t *hashtable, size_t item_cnt) {
  (void)hashtable;
  (void)item_cnt;
  return INTERNAL_ERR;
}

err_code_t ht_init(hashtable_t **hashtable, size_t item_cnt) {
  (void)hashtable;
  (void)item_cnt;
  return INTERNAL_ERR;
}

err_code_t ht_resize(hashtable_t *hashtable, size_t item_cnt) {
  (void)hashtable;
  (void)item_cnt;
  return INTERNAL_ERR;
}

void ht_data_destruct(hashtable_t *hashtable) { (void)hashtable; }

void ht_destruct(hashtable_t **hashtable) { (void)hashtable; }

size_t ht_hash_function(const char *identificator) {
  (void)identificator;
  return 0;
}

err_code_t ht_insert_item(hashtable_t *hashtable, token_t token) {
  (void)hashtable;
  (void)token;
  return INTERNAL_ERR;
}

err_code_t ht_find_item(hashtable_t *hashtable, const char *identificator,
                        size_t *item_idx) {
  (void)hashtable;
  (void)identificator;
  (void)item_idx;
  return INTERNAL_ERR;
}

err_code_t ht_delete_item(hashtable_t *hashtable, const char *identificator) {
  (void)hashtable;
  (void)identificator;
  return INTERNAL_ERR;
}

err_code_t ht_load_factor(hashtable_t *hashtable, double *load_factor) {
  (void)hashtable;
  (void)load_factor;
  return INTERNAL_ERR;
}