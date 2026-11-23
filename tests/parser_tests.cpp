// LLM GENERATED
// Gemini 3.1 Pro
#include <cstdio>
#include <gtest/gtest.h>
#include <string>

extern "C" {
#include "../src/parser.h"
}

class ParserTest : public ::testing::Test {
protected:
  FILE *stream = nullptr;

  // Helper to create a FILE* from a string
  void SetUpStream(const std::string &input) {
    stream = std::tmpfile();
    ASSERT_NE(stream, nullptr) << "Failed to create temporary file";
    std::fputs(input.c_str(), stream);
    std::rewind(stream); // Reset file pointer to the beginning for the parser
  }

  void TearDown() override {
    if (stream) {
      std::fclose(stream); // tmpfile() automatically deletes the file on close
    }
  }
};

TEST_F(ParserTest, Prolog) {
  SetUpStream("import ifj26.*");

  ast_node_t *node = nullptr;
  err_code_t result = parse(&node, stream);

  ASSERT_EQ(result, NO_ERR);
  destroy_node(node);
}