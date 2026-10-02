EXECUTABLE = compiler
CC = gcc
CFLAGS = -Wall -Wextra --std=c17

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I$(SRC_DIR) -pthread
GTEST_LIBS = -lgtest -lgtest_main -pthread

SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build

TEST_EXECUTABLE = run_tests

OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))
OBJS_NO_MAIN = $(filter-out $(BUILD_DIR)/main.o, $(OBJS))

TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp, $(BUILD_DIR)/test_%.o, $(TEST_SRCS))

.PHONY: all clean test

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

$(TEST_EXECUTABLE): $(TEST_OBJS) $(OBJS_NO_MAIN)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(GTEST_LIBS)

$(BUILD_DIR)/test_%.o: $(TEST_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(EXECUTABLE) $(TEST_EXECUTABLE) $(BUILD_DIR)