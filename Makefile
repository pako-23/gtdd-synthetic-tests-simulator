CXX := g++
CXXFLAGS := -O3 -g -Wall -std=c++11
SRC_DIR := src/
BUILD_DIR := build/
PROG_NAME := synthetic-tests-simulator
PROG := $(BUILD_DIR)$(PROG_NAME)

SRCS := $(wildcard $(SRC_DIR)*.cc)
OBJS := $(SRCS:$(SRC_DIR)%.cc=$(BUILD_DIR)%.o)

.PHONY: all
all: $(PROG)

$(PROG): $(OBJS) | build_dir
	$(CXX) $(CXXFLAGS) -o $(PROG) $^

$(BUILD_DIR)%.o: $(SRC_DIR)%.cc | build_dir
	$(CXX) $(CXXFLAGS) -c -MD -o $@ $<
	@c=$$(cat '$(BUILD_DIR)$*.d') && echo "$$c" >> '$(BUILD_DIR)$*.d'
	@awk -i inplace '{\
		if (/$(subst /,\/,$(BUILD_DIR)$*.o: )/ && count < 1) { \
			gsub("$(BUILD_DIR)$*.o: ","$(BUILD_DIR)$*.d: $$(wildcard ") \
			count++ \
		} \
		if (!/\\$$/ && count < 1) { gsub("$$",")") count++ } \
		print \
	}' $(BUILD_DIR)$*.d

.PHONY: build_dir
build_dir: $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $@

-include $(DEPENDS)


MAX_RUNS ?= 50
MIN_TESTS ?= 2
MAX_TESTS ?= 500
TESTS_STEP ?= 10

make-experiment = for j in $$(seq 1 $(MAX_RUNS)); do \
		for i in $$(seq $(MIN_TESTS) $(TESTS_STEP) $(MAX_TESTS)); do \
			$(PROG) -t="$$i" -g="$1" -a="$2"; \
		done \
	done

.PHONY: experiments
experiments: big-table \
	pradet \
	ex-linear


# big-table experiments
.PHONY: big-table
big-table: barabasi-albert-big-table \
	erdos-renyi-big-table \
	out-degree-big-table

.PHONY: barabasi-albert-big-table
barabasi-albert-big-table: all
	$(call make-experiment,barabasi-albert,big-table)

.PHONY: erdos-renyi-big-table
erdos-renyi-big-table: all
	$(call make-experiment,erdos-renyi,big-table)

.PHONY: out-degree-big-table
out-degree-big-table: all
	$(call make-experiment,out-degree,big-table)


# ex-linear experiments
.PHONY: ex-linear
ex-linear: barabasi-albert-ex-linear \
	erdos-renyi-ex-linear \
	out-degree-ex-linear

.PHONY: barabasi-albert-ex-linear
barabasi-albert-ex-linear: all
	$(call make-experiment,barabasi-albert,ex-linear)

.PHONY: erdos-renyi-ex-linear
erdos-renyi-ex-linear: all
	$(call make-experiment,erdos-renyi,ex-linear)

.PHONY: out-degree-ex-linear
out-degree-ex-linear: all
	$(call make-experiment,out-degree,ex-linear)


# pradet experiments
.PHONY: pradet
pradet: barabasi-albert-pradet \
	erdos-renyi-pradet \
	out-degree-pradet

.PHONY: barabasi-albert-pradet
barabasi-albert-pradet: all
	$(call make-experiment,barabasi-albert,pradet)

.PHONY: erdos-renyi-pradet
erdos-renyi-pradet: all
	$(call make-experiment,erdos-renyi,pradet)

.PHONY: out-degree-pradet
out-degree-pradet: all
	$(call make-experiment,out-degree,pradet)


.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)
