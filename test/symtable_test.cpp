/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file symtable_test.cpp
 *
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Unit tests for symbol table functionality
 *
 \*****************************************************************************/

extern "C" {
#include "symtable.h"
}
#include <gtest/gtest.h>

/****************************\
 * Hash table test fixtures *
\****************************/

class NullHashTablePointerFixture : public ::testing::Test {
protected:
  hashtable_t *hashtable;
  char *identificator;

  virtual void SetUp() { hashtable = nullptr; }

  virtual void TearDown() {
    if (hashtable != nullptr) {
      if (hashtable->data != nullptr) {
        free(hashtable->data);
      }
      free(hashtable);
    }
  }
};

class EmptyHashTableFixture : public ::testing::Test {
protected:
  hashtable_t *hashtable;

  virtual void SetUp() { ht_init(&hashtable, HT_INITIAL_CAPACITY); }

  virtual void TearDown() { ht_destruct(&hashtable); }
};

/********************\
 * Hash table tests *
\********************/

TEST_F(NullHashTablePointerFixture, HashTableInitialization) {

  ASSERT_EQ(ht_init(&hashtable, HT_INITIAL_CAPACITY), NO_ERR);
  ASSERT_NE(hashtable, nullptr);
  ASSERT_NE(hashtable->data, nullptr);

  EXPECT_EQ(hashtable->capacity, HT_INITIAL_CAPACITY);
  EXPECT_EQ(hashtable->item_cnt, 0);

  for (size_t item = 0; item < HT_INITIAL_CAPACITY; item++) {
    EXPECT_EQ(hashtable->data[item].state, EMPTY);
    EXPECT_EQ(hashtable->data[item].identificator, nullptr);
  }
}

TEST_F(NullHashTablePointerFixture, HashTableDestruction) {
  ASSERT_EQ(ht_init(&hashtable, HT_INITIAL_CAPACITY), NO_ERR);
  ASSERT_NE(hashtable, nullptr);
  EXPECT_NE(hashtable->data, nullptr);

  ht_destruct(&hashtable);
  EXPECT_EQ(hashtable, nullptr);
}

TEST_F(NullHashTablePointerFixture, HashFunction) {
  const char *identificator_placeholder = "identificator";
  size_t hash;
  hash = ht_hash_function(identificator_placeholder);
  EXPECT_EQ(hash, 0x8be5a7a6);
}

TEST_F(EmptyHashTableFixture, EmptyHashTableLoadFactor) {
  double load_factor;
  EXPECT_EQ(ht_load_factor(hashtable, &load_factor), NO_ERR);
  EXPECT_EQ(load_factor, 0.0);
}

TEST_F(EmptyHashTableFixture, HashTableInvalidLoadFactor) {
  double load_factor;
  hashtable->item_cnt = HT_INITIAL_CAPACITY + 1;
  EXPECT_EQ(ht_load_factor(hashtable, &load_factor), INTERNAL_ERR);

  hashtable->item_cnt = 0;
  hashtable->capacity = 0;
  EXPECT_EQ(ht_load_factor(hashtable, &load_factor), INTERNAL_ERR);
}