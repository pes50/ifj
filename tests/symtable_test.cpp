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

#define HT_INITIAL_CAPACITY 4096

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
  const char *identificator0 = "identificator0";
  const char *identificator1 = "identificator1";
  const char *identificator2 = "identificator2";
  const char *identificator3 = "identificator3";
  const char *identificator4 = "identificator4";
  const char *identificator5 = "identificator5";

  virtual void SetUp() {
    ht_init(&hashtable, 10);

    ht_insert_item(hashtable, identificator0, (char *)identificator0);
    ht_insert_item(hashtable, identificator1, (char *)identificator1);
    ht_insert_item(hashtable, identificator2, (char *)identificator2);
    ht_insert_item(hashtable, identificator3, (char *)identificator3);
    ht_insert_item(hashtable, identificator4, (char *)identificator4);
    ht_insert_item(hashtable, identificator5, (char *)identificator5);
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

TEST_F(EmptyHashTableFixture, EmptyHashTableInsertion) {
  const char identificator[] = "identificator";

  ASSERT_EQ(ht_insert_item(hashtable, identificator, nullptr), NO_ERR);

  size_t index = ht_hash_function(identificator) % HT_INITIAL_CAPACITY;
  EXPECT_EQ(hashtable->data[index].state, FULL);
  EXPECT_EQ(hashtable->item_cnt, 1);
}

TEST_F(EmptyHashTableFixture, HtInvalidSymbolInsertion) {
  const char identificator[] = "";

  EXPECT_EQ(ht_insert_item(hashtable, identificator, nullptr), INTERNAL_ERR);
}

TEST_F(EmptyHashTableFixture, InvalidHashTableInsertion) {
  const char identificator[] = "identificator";

  hashtable->capacity = 0;
  EXPECT_EQ(ht_insert_item(hashtable, identificator, nullptr), INTERNAL_ERR);
  // no need testing the cap < item_cnt error - its from the same function which
  // is tested by itself. if one of its error will pass through the insert
  // function, the other one will too
}

TEST_F(SixtyPercentFullHashTableFixture, HashTableAutoResizing) {
  const char identificator6[] = "identificator6";

  EXPECT_EQ(ht_insert_item(hashtable, identificator6, nullptr), NO_ERR);

  ASSERT_EQ(hashtable->capacity, 20);
  ASSERT_EQ(hashtable->item_cnt, 7);

  const char identificator7[] = "identificator7";
  EXPECT_EQ(ht_insert_item(hashtable, identificator7, nullptr), NO_ERR);
  const char identificator8[] = "identificator8";
  EXPECT_EQ(ht_insert_item(hashtable, identificator8, nullptr), NO_ERR);
  const char identificator9[] = "identificator9";
  EXPECT_EQ(ht_insert_item(hashtable, identificator9, nullptr), NO_ERR);
  const char identificator10[] = "identificator10";
  EXPECT_EQ(ht_insert_item(hashtable, identificator10, nullptr), NO_ERR);
}

TEST_F(SixtyPercentFullHashTableFixture, HashTableGetExistingItem) {
  void *data;
  ASSERT_EQ(ht_get_item(hashtable, "identificator4", &data), NO_ERR);
  // in this fixture the data saved with the identificators as key are the
  // identificator strings themselves
  EXPECT_EQ(data, identificator4);
  EXPECT_STREQ((char *)data, identificator4);
}

TEST_F(SixtyPercentFullHashTableFixture, HashTableGetNonExistingItem) {
  void *data;
  EXPECT_EQ(ht_get_item(hashtable, "non-existing id example", &data),
            UNDEF_SYMBOL_ERR);
  EXPECT_EQ(data, nullptr);
}

TEST_F(EmptyHashTableFixture, HashTableGetInvalidId) {
  void *data;
  EXPECT_EQ(ht_get_item(hashtable, "", &data), INTERNAL_ERR);
  EXPECT_EQ(data, nullptr);
}

TEST_F(SixtyPercentFullHashTableFixture, HashTableDeleteExistingItem) {
  size_t orig_item_cnt = hashtable->item_cnt;

  void *data;
  EXPECT_EQ(ht_get_item(hashtable, "identificator4", &data), NO_ERR);

  EXPECT_EQ(ht_delete_item(hashtable, "identificator4"), NO_ERR);

  EXPECT_EQ(hashtable->item_cnt, orig_item_cnt - 1);
}

TEST_F(SixtyPercentFullHashTableFixture, HashTableDeleteEmptyIdItem) {
  size_t orig_item_cnt = hashtable->item_cnt;
  EXPECT_EQ(ht_delete_item(hashtable, ""), INTERNAL_ERR);
  EXPECT_EQ(hashtable->item_cnt, orig_item_cnt);
}

TEST_F(SixtyPercentFullHashTableFixture, HashTableGetDeletedItem) {
  void *data;
  ht_delete_item(hashtable, "identificator3");

  EXPECT_EQ(ht_get_item(hashtable, "identificator3", &data), UNDEF_SYMBOL_ERR);
}

TEST_F(SixtyPercentFullHashTableFixture, HashTableDeleteNonExistingItem) {
  ht_delete_item(hashtable, "identificator0");
  ht_delete_item(hashtable, "identificator1");
  ht_delete_item(hashtable, "identificator2");
  ht_delete_item(hashtable, "identificator3");

  EXPECT_EQ(ht_delete_item(hashtable, "identificator3"), INTERNAL_ERR);
}
