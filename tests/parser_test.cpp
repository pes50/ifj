// LLM GENERATED
// Gemini 3.1 Pro
#include <cstdio>
#include <filesystem>
#include <gtest/gtest.h>
#include <string>

extern "C" {
#include "../src/parser.h"
}

namespace fs = std::filesystem;

TEST(ParserTest, ParsesAllScalaExamples) {
  const std::string examples_dir = "examples";

  ASSERT_TRUE(fs::exists(examples_dir) && fs::is_directory(examples_dir))
      << "Examples directory not found at: " << examples_dir;

  bool found_scala_files = false;

  for (const auto &entry : fs::recursive_directory_iterator(examples_dir)) {
    if (entry.is_regular_file() && entry.path().extension() == ".scala") {

      // Extract just the filename (e.g., "lists.scala")
      std::string filename = entry.path().filename().string();

      if (filename == "ifj26.scala") {
        continue;
      }

      found_scala_files = true;
      std::string filepath = entry.path().string();

      FILE *stream = fopen(filepath.c_str(), "r");
      ASSERT_NE(stream, nullptr) << "Failed to open file: " << filepath;

      ast_node_t *node = nullptr;

      err_code_t err = parse(&node, stream);

      EXPECT_EQ(err, NO_ERR) << "Failed parsing file: " << filepath;

      if (node != nullptr) {
        destroy_node(node);
      }

      fclose(stream);
    }
  }

  EXPECT_TRUE(found_scala_files)
      << "No valid .scala files found to test in " << examples_dir;
}
