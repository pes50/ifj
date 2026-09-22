EXECUTABLE = compiler
CC = gcc
CFLAGS = -Wall -Wextra --std=c17

SRC_DIR = src
BUILD_DIR = build

OBJS = $(BUILD_DIR)/main.o

.PHONY: clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(EXECUTABLE) $(BUILD_DIR)