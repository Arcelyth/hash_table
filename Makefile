BUILD_DIR 		:= build
TEST_OUT_DIR	:= $(BUILD_DIR)/tests
EXP_OUT_DIR		:= $(BUILD_DIR)/examples
CC        		:= gcc
CFLAGS    		:= -Wall -Wextra -O2
EXP_DIR   		:= ./examples
TEST_DIR 		:= ./tests
INCLUDE_DIR 	:= .

EXP_SRCS    := $(wildcard $(EXP_DIR)/*.c)
TEST_SRCS      	:= $(wildcard $(TEST_DIR)/*.c)

EXP_TARGETS   	:= $(patsubst $(EXP_DIR)/%.c, $(EXP_OUT_DIR)/%, $(EXP_SRCS))
TEST_TARGETS   	:= $(patsubst $(TEST_DIR)/%.c, $(TEST_OUT_DIR)/%, $(TEST_SRCS))

all: test exp

.PHONY: test
test: $(TEST_OUT_DIR) $(TEST_TARGETS)
	@echo "=== Running Tests ==="
	@for test in $(TEST_TARGETS); do \
		$$test; \
	done
	@echo "=== All Tests Passed ===" \

$(TEST_OUT_DIR)/%: $(TEST_DIR)/%.c | $(TEST_OUT_DIR)
	@echo "compiler $< ..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $< -o $@

.PHONY: exp
exp: $(EXP_OUT_DIR) $(EXP_TARGETS)

$(EXP_OUT_DIR)/%: $(EXP_DIR)/%.c | $(EXP_OUT_DIR)
	@echo "compiler $< ..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $< -o $@


$(TEST_OUT_DIR): 
	mkdir -p $(BUILD_DIR)/tests

$(EXP_OUT_DIR): 
	mkdir -p $(BUILD_DIR)/examples

.PHONY: clean
clean:
	@echo "cleaning..."
	rm -rf $(BUILD_DIR)
