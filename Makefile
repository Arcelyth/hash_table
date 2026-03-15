BUILD_DIR 	:= build
CC        	:= gcc
CFLAGS    	:= -Wall -Wextra -O2
EXP_DIR   	:= ./examples
INCLUDE_DIR := .

SRCS      	:= $(wildcard $(EXP_DIR)/*.c)

TARGETS   	:= $(patsubst $(EXP_DIR)/%.c, $(BUILD_DIR)/%, $(SRCS))

all: $(BUILD_DIR) $(TARGETS)

$(BUILD_DIR)/%: $(EXP_DIR)/%.c | $(BUILD_DIR)
	@echo "compiler $< ..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

.PHONY: clean
clean:
	@echo "cleaning..."
	rm -rf $(BUILD_DIR)
