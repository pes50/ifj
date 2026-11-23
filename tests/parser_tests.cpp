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

TEST_F(ParserTest, PrologErr) {
  SetUpStream("import ifj.*");

  ast_node_t *node = nullptr;
  err_code_t result = parse(&node, stream);

  ASSERT_EQ(result, SYNTAX_ERR);
  destroy_node(node);
}

TEST_F(ParserTest, FunDef) {
  std::string program = "import ifj26.*\n"
                        "def fun : Int = x * y";

  SetUpStream(program);

  ast_node_t *node = nullptr;
  err_code_t result = parse(&node, stream);

  ASSERT_EQ(result, NO_ERR);
  destroy_node(node);
}

TEST_F(ParserTest, boolOperators) {
  const char *program = R"(import ifj26.*

@main def main(): Unit =
    val a: Boolean = true
    var b = false
    val c = !a || b && true
    val d = (a == b) != false
)";

  SetUpStream(program);

  ast_node_t *node = nullptr;
  err_code_t result = parse(&node, stream);

  ASSERT_EQ(result, NO_ERR);
  destroy_node(node);
}

TEST_F(ParserTest, noElse) {
  const char *program = R"(import ifj26.*

@main def main(): Unit =
    val flag = true
    if flag then
        print("Pravda")
)";

  SetUpStream(program);

  ast_node_t *node = nullptr;
  err_code_t result = parse(&node, stream);

  ASSERT_EQ(result, NO_ERR);
  destroy_node(node);
}

TEST_F(ParserTest, elseIfChain) {
  const char *program = R"(import ifj26.*

@main def main(): Unit =
    val x = 5
    if x < 0 then
        print("Zaporne")
    else if x == 0 then
        print("Nula")
    else if x == 1 then
        print("Jedna")
    else
        print("Jine")
)";

  SetUpStream(program);

  ast_node_t *node = nullptr;
  err_code_t result = parse(&node, stream);

  ASSERT_EQ(result, NO_ERR);
  destroy_node(node);
}

TEST_F(ParserTest, readBoolean) {
  const char *program = R"(import ifj26.*

@main def main(): Unit =
    val input = readBoolean()
    val isTrue = input == true
    
    while !isTrue || false do
        print("Cyklus")
)";

  SetUpStream(program);

  ast_node_t *node = nullptr;
  err_code_t result = parse(&node, stream);

  ASSERT_EQ(result, NO_ERR);
  destroy_node(node);
}