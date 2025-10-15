CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
LDFLAGS= -lncurses
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))

TARGET = $(BUILD_DIR)/text_editor
TEST_TARGET = $(BUILD_DIR)/test_all

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(BUILD_DIR) $(OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

test-fp: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC_DIR)/piece_table.c $(TEST_DIR)/test_piece_table.c -o $(TEST_TARGET)
	./$(TEST_TARGET)

test-insert: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC_DIR)/piece_table.c $(TEST_DIR)/test_insert.c -o $(TEST_TARGET)
	./$(TEST_TARGET)

test: test-fp test-insert

clean:
	rm -rf $(BUILD_DIR)
