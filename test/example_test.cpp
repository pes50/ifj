#include <gtest/gtest.h>

TEST(ExampleSuite, Pass) { EXPECT_EQ(1, 1); }

TEST(ExampleSuite, Fail) { EXPECT_EQ(1, 2); }