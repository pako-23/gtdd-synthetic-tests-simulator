CC := g++
CFLAGS := -O3 -g -Wall -std=c++11
SRC_DIR := ./src
BUILD_DIR := ./build
EXAMPLE_PARAMS := --tests=10 --algorithm=ex-linear --graph-generator=erdos-renyi
PROG_NAME := synthetic-tests-simulator
PROG := $(BUILD_DIR)/$(PROG_NAME)

SRCS := $(wildcard $(SRC_DIR)/*.cc)
OBJS := $(SRCS:$(SRC_DIR)/%.cc=$(BUILD_DIR)/%.o)

all: $(OBJS) $(wildcard $(SRC_DIR)/*.h)
	$(CC) $(CFLAGS) -o $(PROG) $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc | build_dir
	$(CC) $(CFLAGS) -o $@ -c $^

build_dir:
	mkdir -p $(BUILD_DIR)

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)


mem_check: all
	@valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         ./$(PROG) $(EXAMPLE_PARAMS)
