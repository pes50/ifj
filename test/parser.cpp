#include <gtest/gtest.h>
#include <vector>

extern "C" {
#include "parser.h"
#include "token.h"
}

class ParserFixture : public ::testing::Test {
public:
  static thread_local ParserFixture *active_fixture;

  token_t GetNextToken() {
    if (current < tokens.size()) {
      return tokens[current++];
    }
    return token_t{TOK_ERR};
  }

protected:
  std::vector<token_t> tokens;
  size_t current = 0;

  void SetUp() override { active_fixture = this; }

  void TearDown() override { active_fixture = nullptr; }
};

thread_local ParserFixture *ParserFixture::active_fixture = nullptr;

extern "C" {
token_t __real_get_token(void);

token_t __wrap_get_token(void) {
  if (ParserFixture::active_fixture != nullptr) {
    return ParserFixture::active_fixture->GetNextToken();
  }
  return __real_get_token();
}
}

TEST_F(ParserFixture, ReadAllTokens) {
  tokens = {
      {TOK_L_PAREN},
      {TOK_LIT_INT, "1", 1},
      {TOK_PLUS},
      {TOK_LIT_INT, "1", 1},
      {TOK_R_PAREN},
      {TOK_MUL},
      {TOK_LIT_INT, "23", 2},
      {TOK_ERR},
  };

  parse();

  EXPECT_EQ(current, 8);
}

TEST_F(ParserFixture, StopOnError) {
  tokens = {
      {TOK_LIT_INT, "1", 1}, {TOK_PLUS}, {TOK_ERR},
      {TOK_LIT_INT, "1", 1}, {TOK_ERR},
  };

  parse();

  EXPECT_EQ(current, 3);
}

TEST(Parser, RealGetToken) {
  int result = parse();
  EXPECT_EQ(result, 0);
}