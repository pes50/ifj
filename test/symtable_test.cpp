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

class SixtyPercentFullHashTableFixture : public ::testing::Test {
protected:
  hashtable_t *hashtable;

  virtual void SetUp() {
    ht_init(&hashtable, 10);

    token_t token;
    token.type = TOK_IDENTIFIER;

    // TODO dont know how to write nicer (token.value is const char*)

    token.value = "identificator0";
    token.length = 14;
    ht_insert_item(hashtable, token);
    token.value = "identificator1";
    ht_insert_item(hashtable, token);
    token.value = "identificator2";
    ht_insert_item(hashtable, token);
    token.value = "identificator3";
    ht_insert_item(hashtable, token);
    token.value = "identificator4";
    ht_insert_item(hashtable, token);
    token.value = "identificator5";
    ht_insert_item(hashtable, token);
  }

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

TEST_F(EmptyHashTableFixture, EmptyHashTableInsertion) {
  token_t token;
  token.value = "identificator";
  token.type = TOK_IDENTIFIER;
  token.length = 13;

  ASSERT_EQ(ht_insert_item(hashtable, token), NO_ERR);

  size_t index = ht_hash_function(token.value) % HT_INITIAL_CAPACITY;
  EXPECT_EQ(hashtable->data[index].state, FULL);
  EXPECT_EQ(hashtable->item_cnt, 1);
}

TEST_F(EmptyHashTableFixture, HtInvalidSymbolInsertion) {
  token_t token;
  token.value = "";
  token.type = TOK_IDENTIFIER;
  token.length = 0;

  EXPECT_EQ(ht_insert_item(hashtable, token), INTERNAL_ERR);
}

TEST_F(EmptyHashTableFixture, InvalidHashTableInsertion) {
  token_t token;
  token.value = "identificator";
  token.type = TOK_IDENTIFIER;
  token.length = 13;

  hashtable->capacity = 0;
  EXPECT_EQ(ht_insert_item(hashtable, token), INTERNAL_ERR);
  // no need testing the cap < item_cnt error - its from the same function which
  // is tested by itself. if one of its error will pass through the insert
  // function, the other one will too
}

TEST_F(SixtyPercentFullHashTableFixture, HashTableResize) {
  ASSERT_EQ(ht_resize(hashtable, 20), NO_ERR);

  EXPECT_EQ(hashtable->capacity, 20);

  double load_factor;
  ht_load_factor(hashtable, &load_factor);
  EXPECT_EQ(load_factor, 6.0 / 20.0);
}

TEST_F(SixtyPercentFullHashTableFixture, HashTableAutoResizing) {
  token_t token;
  token.type = TOK_IDENTIFIER;
  token.value = "identificator6";
  token.length = 14;

  EXPECT_EQ(ht_insert_item(hashtable, token), NO_ERR);
}

TEST_F(SixtyPercentFullHashTableFixture, HashTableFindExistingItem) {
  size_t item_idx;
  ASSERT_EQ(ht_find_item(hashtable, "identificator4", &item_idx), NO_ERR);
  EXPECT_EQ(hashtable->data[item_idx].identificator, "identificator4");
  EXPECT_EQ(hashtable->data[item_idx].state, FULL);
}

TEST_F(SixtyPercentFullHashTableFixture, HashTableFindNonExistingItem) {
  size_t item_idx;
  EXPECT_EQ(ht_find_item(hashtable, "non-existing id example", &item_idx),
            UNDEF_SYMBOL_ERR);
}

TEST_F(EmptyHashTableFixture, HashTableFindInvalidId) {
  size_t item_idx;
  EXPECT_EQ(ht_find_item(hashtable, "", &item_idx), INTERNAL_ERR);
}

TEST_F(SixtyPercentFullHashTableFixture, HashTableDeleteExistingItem) {
  size_t orig_item_cnt = hashtable->item_cnt;

  size_t item_idx;
  ht_find_item(hashtable, "identificator4", &item_idx);

  EXPECT_EQ(ht_delete_item(hashtable, "identificator4"), NO_ERR);

  EXPECT_EQ(hashtable->item_cnt, orig_item_cnt - 1);
  EXPECT_EQ(hashtable->data[item_idx].state, DELETED);
}

TEST_F(SixtyPercentFullHashTableFixture, HashTableDeleteEmptyIdItem) {
  size_t orig_item_cnt = hashtable->item_cnt;
  EXPECT_EQ(ht_delete_item(hashtable, ""), INTERNAL_ERR);
  EXPECT_EQ(hashtable->item_cnt, orig_item_cnt);
}

TEST_F(SixtyPercentFullHashTableFixture, HashTableFindDeletedItem) {
  size_t item_idx;
  ht_delete_item(hashtable, "identificator3");

  EXPECT_EQ(ht_find_item(hashtable, "identificator3", &item_idx),
            UNDEF_SYMBOL_ERR);
}

TEST_F(SixtyPercentFullHashTableFixture, HashTableDeleteNonExistingItem) {
  ht_delete_item(hashtable, "identificator0");
  ht_delete_item(hashtable, "identificator1");
  ht_delete_item(hashtable, "identificator2");
  ht_delete_item(hashtable, "identificator3");

  // TODO not decided yet on the error code
  EXPECT_EQ(ht_delete_item(hashtable, "identificator3"), INTERNAL_ERR);
}
