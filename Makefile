CXX := g++
CXXFLAGS := -O3 -Wall -Werror -pedantic -std=c++20
SRC_DIR := src/
BUILD_DIR := build/
PROG_NAME := synthetic-tests-simulator
PROG := $(BUILD_DIR)$(PROG_NAME)
CODE_STYLE := WebKit

SRCS := $(wildcard $(SRC_DIR)*.cc)
H_FILES := $(wildcard $(SRC_DIR)*.h)
OBJS := $(SRCS:$(SRC_DIR)%.cc=$(BUILD_DIR)%.o)
DEPENDS := $(OBJS:%.o=%.d)

ifneq ($(shell which podman 2>/dev/null),)
	CONTAINER_CLI := $(shell which podman)
	MOUNT_OPTIONS := :Z
else ifneq ($(shell which docker 2>/dev/null),)
	CONTAINER_CLI := $(shell which docker)
	MOUNT_OPTIONS :=
endif

.PHONY: all
all: $(PROG)

$(PROG): $(OBJS) | build_dir
	$(CXX) $(CXXFLAGS) -o $(PROG) $^

$(BUILD_DIR)%.o: $(SRC_DIR)%.cc | build_dir
	$(CXX) $(CXXFLAGS) -MM -MP -MT '$@' -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: build_dir
build_dir: $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $@


-include $(DEPENDS)

ifdef CONTAINER_CLI
.PHONY: lint
lint:
	@$(CONTAINER_CLI) run -t --rm -v $(PWD):/workdir$(MOUNT_OPTIONS) \
		-w /workdir \
		neszt/cppcheck-docker \
		cppcheck --enable=warning .

.PHONY: format
format:
	@$(CONTAINER_CLI) run -t --rm -v $(PWD):/workdir$(MOUNT_OPTIONS) \
		-w /workdir \
		unibeautify/clang-format \
		-i -style=$(CODE_STYLE) $(SRCS) $(H_FILES)
endif

MAX_RUNS ?= 50
MIN_TESTS ?= 2
MAX_TESTS ?= 500
TESTS_STEP ?= 10
TIMEOUT ?= 30m
make-experiment = for j in $$(seq 1 $(MAX_RUNS)); do \
		for i in $$(seq $(MIN_TESTS) $(TESTS_STEP) $(MAX_TESTS)); do \
			timeout $(TIMEOUT) $(PROG) -t="$$i" -g="$1" -a="$2" || true; \
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


ifneq ($(wildcard ./results),)
ifneq ($(shell which gnuplot 2>/dev/null),)

PLOTS_DIR := plots/
GNUPLOT_SCRIPTS_DIR := gnuplot/
GNUPLOT_SCRIPTS := $(wildcard $(GNUPLOT_SCRIPTS_DIR)*.gp)

STATS_FILES := $(shell find ./results -name stats.csv)
DAT_FILES := $(STATS_FILES:%.csv=%.dat)

PLOTS := $(GNUPLOT_SCRIPTS:$(GNUPLOT_SCRIPTS_DIR)%.gp=$(PLOTS_DIR)%-barabasi-albert.pdf) \
	$(GNUPLOT_SCRIPTS:$(GNUPLOT_SCRIPTS_DIR)%.gp=$(PLOTS_DIR)%-erdos-renyi.pdf) \
	$(GNUPLOT_SCRIPTS:$(GNUPLOT_SCRIPTS_DIR)%.gp=$(PLOTS_DIR)%-out-degree-3-3.pdf)

.PHONY: plots_dir
plots_dir: $(PLOTS_DIR)

$(PLOTS_DIR):
	mkdir -p $@

%.dat: %.csv
	$(GNUPLOT_SCRIPTS_DIR)compute-stats.py $^

$(PLOTS_DIR)%-barabasi-albert.pdf: $(GNUPLOT_SCRIPTS_DIR)%.gp  $(DAT_FILES) | plots_dir
	gnuplot $<

$(PLOTS_DIR)%-erdos-renyi.pdf: $(GNUPLOT_SCRIPTS_DIR)%.gp $(DAT_FILES) | plots_dir
	gnuplot $<

$(PLOTS_DIR)%-out-degree-3-3.pdf: $(GNUPLOT_SCRIPTS_DIR)%.gp $(DAT_FILES) | plots_dir
	gnuplot $<

.PHONY: plots
plots: $(PLOTS)

endif
endif

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)
ifdef PLOTS_DIR
	@rm -rf $(PLOTS_DIR)
	@rm -rf $(DAT_FILES)
endif
