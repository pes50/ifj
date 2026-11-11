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
    hashtable->data[index].data = NULL;
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

  hashtable_t new_table = *hashtable;
  new_table.data = NULL;
  new_table.capacity = item_cnt;
  new_table.item_cnt = 0;
  if (ht_init_data(&new_table, item_cnt) == INTERNAL_ERR) {
    return INTERNAL_ERR;
  }

  // goes through the old array and re-inserts all saved items to the new table
  for (size_t index = 0; index < old_table.capacity; index++) {
    // checks if there is any saved item left
    if (old_table.item_cnt <= 0) {
      break;
    }
    // inserts item from old table into the new one
    if (old_table.data[index].state == FULL) {
      if (ht_insert_item(&new_table, old_table.data[index].identificator,
                         old_table.data[index].data) == INTERNAL_ERR) {
        ht_data_destruct(&new_table);
        return INTERNAL_ERR;
      }
      // notes that the item has been copied
      old_table.item_cnt--;
    }
  }
  ht_data_destruct(&old_table);
  *hashtable = new_table;

  return NO_ERR;
}

/**
 * @brief Finds symbol in the hash table by it's identificator
 *
 * @param hashtable Hash table to look into
 * @param identificator Identificator of the given symbol
 * @param [out] item_idx Return pointer with the found item index
 *
 * @return INTERNAL_ERR if id of the symbol is empty string
 *                      or the hashtable has capacity = 0
 *         UNDEF_SYMBOL_ERR if symbol of that id is not in hashtable,
 *         NO_ERR otherwise
 */
static err_code_t ht_find_item(hashtable_t *hashtable,
                               const char *identificator, size_t *item_idx) {
  *item_idx = 0;
  // checks for invalid input
  if (identificator == NULL || strlen(identificator) == 0 ||
      hashtable->capacity == 0) {
    return INTERNAL_ERR;
  }

  // project onto <0, table capacity>
  size_t hash_value = ht_hash_function(identificator) % hashtable->capacity;

  // helper variable for readability
  ht_item_t *ht_data = hashtable->data;

  size_t examined_items = 0;
  // until item is found in the table that has the same id as passed
  // in parameter and is marked as FULL (not EMPTY/DELETED)
  // or all the items have been examined
  while (ht_data[hash_value].state != FULL ||
         strcmp(ht_data[hash_value].identificator, identificator) != 0) {

    if (ht_data[hash_value].state == EMPTY ||
        examined_items > hashtable->capacity) {
      return UNDEF_SYMBOL_ERR;
    }
    hash_value++;
    hash_value = hash_value % hashtable->capacity;
    examined_items++;
  }
  // returns index of the item in hasthable by this pointer
  *item_idx = hash_value;
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
    free(*hashtable);
    *hashtable = NULL;
    return INTERNAL_ERR;
  }

  // initialize hashtable helper data
  (*hashtable)->capacity = item_cnt;
  (*hashtable)->item_cnt = 0;
  (*hashtable)->parent = NULL;
  return NO_ERR;
}

void ht_destruct(hashtable_t **hashtable) {
  // if a address of a hash table is passed, the table is freed
  if (*hashtable != NULL) {
    ht_data_destruct(*hashtable);

    free(*hashtable);
    *hashtable = NULL;
  }
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

err_code_t ht_insert_item(hashtable_t *hashtable, const char *identificator,
                          void *data) {
  // checks for invalid input
  if (identificator == NULL || strlen(identificator) == 0 ||
      hashtable->capacity == 0) {
    return INTERNAL_ERR;
  }
  size_t item_idx_dumpster;
  // if symbol of same id is already in the table, then redefinition happened
  if (ht_find_item(hashtable, identificator, &item_idx_dumpster) == NO_ERR) {
    return REDEF_SYMBOL_ERR;
  }

  // project onto <0, table capacity>
  size_t hash_value = ht_hash_function(identificator) % hashtable->capacity;

  // while there are occupied spaces keep going further in the table
  while (hashtable->data[hash_value].state == FULL) {
    hash_value++;
    hash_value = hash_value % hashtable->capacity;
  }
  // after empty (or deleted) space was found, save the data there
  hashtable->data[hash_value].identificator = (char *)identificator;
  hashtable->data[hash_value].data = data;
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

err_code_t ht_get_item(hashtable_t *hashtable, const char *identificator,
                       void **data) {
  err_code_t error_code;
  size_t item_idx;

  error_code = ht_find_item(hashtable, identificator, &item_idx);

  // can't proceed if error in the lookup occured
  if (error_code != NO_ERR) {
    if (hashtable->parent != NULL) {
      return ht_get_item(hashtable->parent, identificator, data);
    }

    *data = NULL;
    return error_code;
  }

  // can't proceed if the item at the found index or the index itself is invalid
  if (hashtable->capacity - 1 < item_idx ||
      hashtable->data[item_idx].state != FULL) {
    *data = NULL;
    return INTERNAL_ERR;
  }

  // reads data
  *data = hashtable->data[item_idx].data;
  return error_code;
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
  hashtable->data[item_idx].identificator = NULL;
  hashtable->data[item_idx].data = NULL;
  hashtable->item_cnt--;
  return NO_ERR;
}

hashtable_t *ht_get_root(hashtable_t *hashtable) {
  if (hashtable == NULL) {
    return NULL;
  }

  hashtable_t *current = hashtable;
  while (current->parent != NULL) {
    current = current->parent;
  }

  return current;
}