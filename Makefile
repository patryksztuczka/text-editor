# Kompilator i flagi
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -Iinclude -Itests -Isrc
LDFLAGS =
LDLIBS  = -lncurses

# Katalogi
SRC_DIR   = src
TEST_DIR  = tests
BUILD_DIR = build
DEBUG_DIR = build/debug
TEST_BUILD_DIR = build/test

# Pliki źródłowe i obiekty
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))
DBG_OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(DEBUG_DIR)/%.o,$(SRC_FILES))
TEST_OBJ_FILES = $(patsubst $(TEST_DIR)/%.c,$(TEST_BUILD_DIR)/%.o,$(TEST_FILES))

# Filter out main.c for test linking (tests have their own main)
SRC_FILES_NO_MAIN = $(filter-out $(SRC_DIR)/main.c,$(SRC_FILES))
OBJ_FILES_NO_MAIN = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES_NO_MAIN))
TEST_EXECUTABLES = $(patsubst $(TEST_DIR)/%.c,$(TEST_BUILD_DIR)/%,$(TEST_FILES))

# Cele binarne
TARGET = $(BUILD_DIR)/text_editor
DEBUG_TARGET = $(DEBUG_DIR)/text_editor_debug

.PHONY: all clean debug run gdb test test-all test-insert test-delete

# Domyślnie buduj wersję release
all: $(TARGET)

# ------------------------------------------------------------
# REGUŁY DLA BUILD/RELEASE
# ------------------------------------------------------------
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(BUILD_DIR) $(OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES) -o $@ $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -O2 -c $< -o $@

# ------------------------------------------------------------
# REGUŁY DLA DEBUG
# ------------------------------------------------------------
debug: CFLAGS += -g -O0 -DDEBUG
debug: $(DEBUG_TARGET)

$(DEBUG_DIR):
	mkdir -p $(DEBUG_DIR)

$(DEBUG_TARGET): $(DEBUG_DIR) $(DBG_OBJ_FILES)
	$(CC) $(CFLAGS) $(DBG_OBJ_FILES) -o $@ $(LDLIBS)

$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# ------------------------------------------------------------
# URUCHAMIANIE I SPRZĄTANIE
# ------------------------------------------------------------
run: $(TARGET)
	./$(TARGET)

gdb: debug
	gdb $(DEBUG_TARGET)

# ------------------------------------------------------------
# TESTY
# ------------------------------------------------------------
test: test-all

test-all: $(TEST_EXECUTABLES)
	@echo "Running all tests..."
	@for test in $(TEST_EXECUTABLES); do \
		echo "Running $$test..."; \
		$$test; \
		if [ $$? -eq 0 ]; then \
			echo "✓ $$test passed"; \
		else \
			echo "✗ $$test failed"; \
			exit 1; \
		fi; \
	done
	@echo "All tests passed!"

test-insert: $(TEST_BUILD_DIR)/test_insert
	@echo "Running insert tests..."
	@$(TEST_BUILD_DIR)/test_insert

test-delete: $(TEST_BUILD_DIR)/test_delete
	@echo "Running delete tests..."
	@$(TEST_BUILD_DIR)/test_delete

$(TEST_BUILD_DIR):
	mkdir -p $(TEST_BUILD_DIR)

$(TEST_BUILD_DIR)/%: $(TEST_DIR)/%.c $(OBJ_FILES_NO_MAIN) | $(TEST_BUILD_DIR)
	$(CC) $(CFLAGS) $(OBJ_FILES_NO_MAIN) $< -o $@ $(LDLIBS)

$(TEST_BUILD_DIR)/%.o: $(TEST_DIR)/%.c | $(TEST_BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ------------------------------------------------------------
# SPRZĄTANIE
# ------------------------------------------------------------
clean:
	rm -rf $(BUILD_DIR)
