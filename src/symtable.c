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
  // allocates memory for items of the hash table
  hashtable->data = (ht_item_t *)malloc(item_cnt * sizeof(ht_item_t));
  // checks whether allocation have failed
  if (hashtable->data == NULL) {
    return INTERNAL_ERR;
  }

  // initializes all hash table items as empty
  for (size_t index = 0; index < item_cnt; index++) {
    hashtable->data[index].state = EMPTY;
    hashtable->data[index].identificator = NULL;
  }

  return NO_ERR;
}

err_code_t ht_init(hashtable_t **hashtable, size_t item_cnt) {
  // allocates memory for the hashtable stucture
  *hashtable = (hashtable_t *)malloc(sizeof(hashtable_t));
  // check whether allocation have failed
  if (*hashtable == NULL) {
    return INTERNAL_ERR;
  }

  // allocate memory for hash table items and initialize them as empty
  if (ht_init_data(*hashtable, item_cnt) == INTERNAL_ERR) {
    return INTERNAL_ERR;
  }

  // initialize hashtable helper data
  (*hashtable)->capacity = item_cnt;
  (*hashtable)->item_cnt = 0;
  return NO_ERR;
}

err_code_t ht_resize(hashtable_t *hashtable, size_t item_cnt) {
  (void)hashtable;
  (void)item_cnt;
  return INTERNAL_ERR;
}

void ht_data_destruct(hashtable_t *hashtable) {
  // if hashtable has a initialized data array it is freed
  if ((hashtable)->data != NULL) {
    free((hashtable)->data);
    (hashtable)->data = NULL;
  }
  return;
}

void ht_destruct(hashtable_t **hashtable) {
  // if a address of a hash table is passed, the table is freed
  if (*hashtable != NULL) {
    ht_data_destruct(*hashtable);

    free(*hashtable);
    *hashtable = NULL;
  }
  return;
}

size_t ht_hash_function(const char *identificator) {
  size_t hash_value = HASH_FUNCTION_OFFSET_BASIS;
  size_t character = 0;

  // TODO will it have ending character or do we need to save id length
  while (identificator[character] != '\0') {
    // fnv1-a - firstly xor current character with the hash value, then multiply
    // by the prime
    hash_value ^= (unsigned char)identificator[character];
    hash_value *= HASH_FUNCTION_PRIME;

    // keep only 32 bits even if size_t was larger on given architecture
    hash_value = hash_value & HASH_FUNCTION_32_BIT_MASK;

    // iterate
    character++;
  }

  return hash_value;
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
  if (hashtable->capacity == 0 || hashtable->capacity < hashtable->item_cnt) {
    return INTERNAL_ERR;
  }

  *load_factor = (double)hashtable->item_cnt / hashtable->capacity;

  return NO_ERR;
}
