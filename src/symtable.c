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
#include <string.h>

/********************\
 * Helper functions *
\********************/

/**
 * @brief Allocates memory for the hashtable data array and
 * initializes items as empty
 *
 * @param hashtable Pointer to table in which data array is to be initialized
 * @param item_cnt Wanted capacity
 *
 * @return INTERNAL_ERR if memory allocation failed,
 *         NO_ERR otherwise
 */
static err_code_t ht_init_data(hashtable_t *hashtable, size_t item_cnt) {
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

/**
 * @brief Deallocates all memory held by the hash table's data array
 *
 * @param hashtable Pointer to the hash table whose data should be deallocated
 */
static void ht_data_destruct(hashtable_t *hashtable) {
  // if hashtable has a initialized data array it is freed
  if (hashtable->data != NULL) {
    free(hashtable->data);
    hashtable->data = NULL;
  }
  return;
}

/**
 * @brief Resizes current hash table to set number of items
 *
 * @param hashtable Hash table to be resized
 * @param item_cnt Wanted number of hash table items
 *
 * @return INTERNAL_ERR if memory allocation failed
 *         NO_ERR otherwise
 */
static err_code_t ht_resize(hashtable_t *hashtable, size_t item_cnt) {
  // saves old hash table to a temporary variable and initializes new array of
  // the wanted size
  hashtable_t old_table = *hashtable;

  hashtable->data = NULL;
  hashtable->capacity = item_cnt;
  hashtable->item_cnt = 0;
  if (ht_init_data(hashtable, item_cnt) == INTERNAL_ERR) {
    return INTERNAL_ERR;
  }

  // helper variable to unify the type in insertion function
  token_t temp_token;

  // goes through the old array and re-inserts all saved items to the new table
  for (size_t index = 0; index < old_table.capacity; index++) {
    // checks if there is any saved item left
    if (old_table.item_cnt <= 0) {
      break;
    }
    // inserts item from old table into the new one
    if (old_table.data[index].state == FULL) {
      temp_token.value = old_table.data[index].identificator;
      temp_token.type = TOK_IDENTIFIER;

      if (ht_insert_item(hashtable, temp_token) == INTERNAL_ERR) {
        return INTERNAL_ERR;
      }
      // notes that the item has been copied
      old_table.item_cnt--;
    }
  }
  ht_data_destruct(&old_table);
  return NO_ERR;
}

/**
 * @brief Calculates the load factor (items/capacity) of a hash table
 *
 * @param hashtable Hash table to calculate load factor of
 * @param [out] load_factor Return pointer with the table's load factor
 *
 * @return INTERNAL_ERR if hashtable->capacity = 0 (division by zero)
 *         or if hashtable->capacity < hashtable->item_cnt
 *         NO_ERR otherwise
 */
static err_code_t ht_load_factor(hashtable_t *hashtable, double *load_factor) {
  if (hashtable->capacity == 0 || hashtable->capacity < hashtable->item_cnt) {
    return INTERNAL_ERR;
  }

  *load_factor = (double)hashtable->item_cnt / hashtable->capacity;

  return NO_ERR;
}

/***************************************\
 * Hash table interface implementation *
\***************************************/

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

  while (identificator[character] != '\0') {
    // fnv1-a - firstly xor current character with the hash value, then multiply
    // by the prime
    hash_value = hash_value ^ (unsigned char)identificator[character];
    hash_value = hash_value * HASH_FUNCTION_PRIME;

    // keep only 32 bits even if size_t was larger on given architecture
    hash_value = hash_value & HASH_FUNCTION_32_BIT_MASK;

    // iterate
    character++;
  }

  return hash_value;
}

err_code_t ht_insert_item(hashtable_t *hashtable, token_t token) {
  if (token.length < 1) {
    return INTERNAL_ERR;
  }
  if (hashtable->capacity == 0) {
    return INTERNAL_ERR;
  }

  // project onto <0, table capacity>
  size_t hash_value = ht_hash_function(token.value) % hashtable->capacity;

  // while there are occupied spaces keep going further in the table
  while (hashtable->data[hash_value].state == FULL) {
    hash_value++;
    hash_value = hash_value % hashtable->capacity;
  }
  // after empty (or deleted) space was found, save the data there
  hashtable->data[hash_value].identificator = (char *)token.value;
  // TODO: bude potreba udelat jinak - dohodnout se s Romanem

  hashtable->data[hash_value].state = FULL;

  hashtable->item_cnt++;
  double load_factor;
  if (ht_load_factor(hashtable, &load_factor) == INTERNAL_ERR) {
    return INTERNAL_ERR;
  }

  if (load_factor > HT_MAX_ALLOWED_LOAD_FACTOR) {
    if (ht_resize(hashtable, hashtable->capacity * 2) == INTERNAL_ERR) {
      return INTERNAL_ERR;
    }
  }
  return NO_ERR;
}

err_code_t ht_find_item(hashtable_t *hashtable, const char *identificator,
                        size_t *item_idx) {
  *item_idx = 0;

  // checks for invalid input
  if (strlen(identificator) == 0) {
    return INTERNAL_ERR;
  }
  if (hashtable->capacity == 0) {
    return INTERNAL_ERR;
  }

  // project onto <0, table capacity>
  size_t hash_value = ht_hash_function(identificator) % hashtable->capacity;

  // until item is found in the table that has the same id as passed
  // in parameter and is marked as FULL (not EMPTY/DELETED)
  while (hashtable->data[hash_value].state != FULL ||
         hashtable->data[hash_value].identificator != identificator) {
    if (hashtable->data[hash_value].state == EMPTY) {
      return UNDEF_SYMBOL_ERR;
    }
    hash_value++;
  }
  // returns index of the item in hasthable by this pointer
  *item_idx = hash_value;
  return NO_ERR;
}

err_code_t ht_delete_item(hashtable_t *hashtable, const char *identificator) {
  size_t item_idx;
  err_code_t err_code = ht_find_item(hashtable, identificator, &item_idx);

  // checks ht_find_item error codes
  if (err_code == INTERNAL_ERR) {
    return INTERNAL_ERR;
  }
  if (err_code == UNDEF_SYMBOL_ERR) {
    return INTERNAL_ERR;
  }

  // sets the item state as DELETED and corrects hashtable item count
  hashtable->data[item_idx].state = DELETED;
  (hashtable->item_cnt)--;
  return NO_ERR;
}
