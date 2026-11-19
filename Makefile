EXECUTABLE = compiler
TEST_EXECUTABLE = run_tests

DEBUG_EXECUTABLE = compiler_debug
DEBUG_TEST_EXECUTABLE = run_tests_debug

CC = gcc
BASE_CFLAGS = -Wall -Wextra -std=c17 -MMD -MP
CFLAGS = $(BASE_CFLAGS) -O2
DEBUG_CFLAGS = $(BASE_CFLAGS) -g -O0 -DDEBUG

CXX = g++
BASE_CXXFLAGS = -Wall -Wextra -std=c++17 -I$(SRC_DIR) -pthread -MMD -MP
CXXFLAGS = $(BASE_CXXFLAGS) -O2
DEBUG_CXXFLAGS = $(BASE_CXXFLAGS) -g -O0 -DDEBUG
GTEST_LIBS = -lgtest -lgtest_main -pthread

SRC_DIR = src
TEST_DIR = tests

BUILD_DIR = build/release
DEBUG_BUILD_DIR = build/debug

FILTER ?= *

# Release objects
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))
OBJS_NO_MAIN = $(filter-out $(BUILD_DIR)/main.o, $(OBJS))
TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp, $(BUILD_DIR)/test_%.o, $(TEST_SRCS))

# Debug objects
DEBUG_OBJS = $(patsubst $(SRC_DIR)/%.c, $(DEBUG_BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))
DEBUG_OBJS_NO_MAIN = $(filter-out $(DEBUG_BUILD_DIR)/main.o, $(DEBUG_OBJS))
DEBUG_TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp, $(DEBUG_BUILD_DIR)/test_%.o, $(TEST_SRCS))
DEPS = $(OBJS:.o=.d) $(TEST_OBJS:.o=.d) $(DEBUG_OBJS:.o=.d) $(DEBUG_TEST_OBJS:.o=.d)

# Pack
ARCHIVE = xskoupr00.zip
EXTRA_FILES = rozdeleni rozsireni Makefile dokumentace.pdf

.PHONY: all debug clean test test-debug pack

# Release Targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE) --gtest_filter=$(FILTER)

$(TEST_EXECUTABLE): $(TEST_OBJS) $(OBJS_NO_MAIN)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(GTEST_LIBS)

$(BUILD_DIR)/test_%.o: $(TEST_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Debug Targets
debug: $(DEBUG_EXECUTABLE) $(DEBUG_TEST_EXECUTABLE)

$(DEBUG_EXECUTABLE): $(DEBUG_OBJS)
	$(CC) $(DEBUG_CFLAGS) $^ -o $@

$(DEBUG_BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(DEBUG_BUILD_DIR)
	$(CC) $(DEBUG_CFLAGS) -c $< -o $@

test-debug: $(DEBUG_TEST_EXECUTABLE)
	./$(DEBUG_TEST_EXECUTABLE) --gtest_filter=$(FILTER)

$(DEBUG_TEST_EXECUTABLE): $(DEBUG_TEST_OBJS) $(DEBUG_OBJS_NO_MAIN)
	$(CXX) $(DEBUG_CXXFLAGS) $^ -o $@ $(GTEST_LIBS)

$(DEBUG_BUILD_DIR)/test_%.o: $(TEST_DIR)/%.cpp | $(DEBUG_BUILD_DIR)
	$(CXX) $(DEBUG_CXXFLAGS) -c $< -o $@

# Directories
$(BUILD_DIR) $(DEBUG_BUILD_DIR):
	mkdir -p $@

# Pack
pack:
	@rm -f $(ARCHIVE)
	zip -r $(ARCHIVE) $(SRC_DIR) $(EXTRA_FILES) $(TEST_DIR)
	./is_it_ok.sh $(ARCHIVE)

# Cleanup
clean:
	rm -rf $(EXECUTABLE) $(TEST_EXECUTABLE) $(DEBUG_EXECUTABLE) $(DEBUG_TEST_EXECUTABLE) build $(ARCHIVE)

-include $(DEPS)