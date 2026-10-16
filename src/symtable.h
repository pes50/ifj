/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file symtable.h
 *
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Specification of symbol table interface and declaration
 * of symbol table interface functions
 *
 \*****************************************************************************/

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "err_codes.h"
#include "token.h"
#include <stdlib.h>

/*********************************\
 * Constants and types definitons *
\*********************************/

// 2^12 (to save memory because of scopes)
#define HT_INITIAL_CAPACITY 4096

#define HT_MAX_ALLOWED_LOAD_FACTOR 0.65

// fnv-1a offset basis for 32 bit hashing
#define HASH_FUNCTION_OFFSET_BASIS 2166136261
// prime number used at the fnv-1a hashing function
#define HASH_FUNCTION_PRIME 16777619
// bit mask for shaving anything over 32 bits in fnv-1a algorithm
#define HASH_FUNCTION_32_BIT_MASK 0xFFFFFFFF

/**
 * @brief Possible states of hashtable item
 */
typedef enum { EMPTY, FULL, DELETED } hashtable_item_state;

/**
 * @brief Structure containing symbol data
 */
typedef struct {
  hashtable_item_state state;
  char *identificator;
  // TODO rest of values
} ht_item_t;

/**
 * @brief Structure containing data of a hashtable
 */
typedef struct {
  ht_item_t *data; // dynamic array of hashtable items
  size_t item_cnt;
  size_t capacity;
} hashtable_t;

/*************************************\
 * Interface functions specification *
\*************************************/

/**
 * @brief Allocates memory for the hash table and initializes items
 * values
 *
 * @param [out] hashtable Address of a pointer to return the hash tables address
 * @param item_cnt Wanted capacity
 *
 * @return INTERNAL_ERR if memory allocation failed,
 *         NO_ERR otherwise
 */
err_code_t ht_init(hashtable_t **hashtable, size_t item_cnt);

/**
 * @brief Deallocates all memory held by the hash table
 *
 * @param hashtable Address of the pointer to the hash table to be destructed
 */
void ht_destruct(hashtable_t **hashtable);

/**
 * @brief Calculates 32 bit fnv-1a hash of the identificator
 *
 * @param identificator Identificator of a symbol (>=1 characters)
 *
 * @return Calculated hash
 */
size_t ht_hash_function(const char *identificator);

/**
 * @brief Inserts symbol into the hash table
 *
 * @param hashtable Hash table to insert into
 * @param token Token with the symbol to insert
 *
 * @return INTERNAL_ERR if id of the symbol is empty string
 *                      or memory allocation during resizing failed
 *                      or the capacity is 0
 *                      or the capacity is smaller then item_cnt
 *         REDEF_SYMBOL_ERR if symbol with the same id
 *                          is already in the hash table
 *         NO_ERR otherwise
 */
err_code_t ht_insert_item(hashtable_t *hashtable, token_t token);

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
err_code_t ht_find_item(hashtable_t *hashtable, const char *identificator,
                        size_t *item_idx);

/**
 * @brief Deletes symbol from hash table
 *
 * @param hashtable Hash table delete from
 * @param identificator Identificator of the symbol to be removed
 *
 * @return INTERNAL_ERR if id of the symbol is empty string
 *                      or the hashtable has capacity = 0
 *                      or symbol of that id is not in hashtable,
 *         NO_ERR otherwise
 */
err_code_t ht_delete_item(hashtable_t *hashtable, const char *identificator);

#endif
