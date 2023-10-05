CXX := g++
CXXFLAGS := -g -Wall -Werror -pedantic -std=c++20
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
RESULTS_DIR := ./results/

define EXP_FILES
for i in $$(seq $(MIN_TESTS) $(TESTS_STEP) $(MAX_TESTS)); do \
	for j in $$(seq 1 $(MAX_RUNS)); do \
		echo $(RESULTS_DIR)$1/$$i/graph-$$j.$2; \
	done \
done
endef

define EXP_DIRS
for i in $$(seq $(MIN_TESTS) $(TESTS_STEP) $(MAX_TESTS)); do \
	echo $(RESULTS_DIR)$1/$$i/; \
done
endef

.PHONY: experiments
experiments: pfast_experiments pradet_experiments mem_fast_experiments


.PHONY: pradet_experiments
pradet_experiments: $(shell $(call EXP_FILES,experiments/pradet/barabasi-albert,dot)) \
	$(shell $(call EXP_FILES,experiments/pradet/erdos-renyi,dot)) \
	$(shell $(call EXP_FILES,experiments/pradet/out-degree-3-3,dot))

.PHONY: pfast_experiments
pfast_experiments: $(shell $(call EXP_FILES,experiments/pfast/barabasi-albert,dot)) \
	$(shell $(call EXP_FILES,experiments/pfast/erdos-renyi,dot)) \
	$(shell $(call EXP_FILES,experiments/pfast/out-degree-3-3,dot))

.PHONY: mem_fast_experiments
mem_fast_experiments: $(shell $(call EXP_FILES,experiments/mem-fast/barabasi-albert,txt)) \
	$(shell $(call EXP_FILES,experiments/mem-fast/erdos-renyi,txt)) \
	$(shell $(call EXP_FILES,experiments/mem-fast/out-degree-3-3,txt))


$(RESULTS_DIR)experiments/pradet/barabasi-albert/%.dot: $(RESULTS_DIR)graphs/barabasi-albert/%.dot  all | experiment_dirs
	timeout $(TIMEOUT) $(PROG) deps -i "$<" -a pradet -o $@ -m "$$(dirname "$$(dirname $@)")/stats.csv" || true

$(RESULTS_DIR)experiments/pradet/erdos-renyi/%.dot: $(RESULTS_DIR)graphs/erdos-renyi/%.dot  all | experiment_dirs
	timeout $(TIMEOUT) $(PROG) deps -i "$<" -a pradet -o $@ -m "$$(dirname "$$(dirname $@)")/stats.csv" || true

$(RESULTS_DIR)experiments/pradet/out-degree-3-3/%.dot: $(RESULTS_DIR)graphs/out-degree-3-3/%.dot  all | experiment_dirs
	timeout $(TIMEOUT) $(PROG) deps -i "$<" -a pradet -o $@ -m "$$(dirname "$$(dirname $@)")/stats.csv" || true


$(RESULTS_DIR)experiments/mem-fast/barabasi-albert/%.txt: $(RESULTS_DIR)graphs/barabasi-albert/%.dot  all | experiment_dirs
	timeout $(TIMEOUT) $(PROG) deps -i "$<" -a mem-fast -o $@ -m "$$(dirname "$$(dirname $@)")/stats.csv" || true

$(RESULTS_DIR)experiments/mem-fast/erdos-renyi/%.txt: $(RESULTS_DIR)graphs/erdos-renyi/%.dot  all | experiment_dirs
	timeout $(TIMEOUT) $(PROG) deps -i "$<" -a mem-fast -o $@ -m "$$(dirname "$$(dirname $@)")/stats.csv" || true

$(RESULTS_DIR)experiments/mem-fast/out-degree-3-3/%.txt: $(RESULTS_DIR)graphs/out-degree-3-3/%.dot  all | experiment_dirs
	timeout $(TIMEOUT) $(PROG) deps -i "$<" -a mem-fast -o $@ -m "$$(dirname "$$(dirname $@)")/stats.csv" || true


$(RESULTS_DIR)experiments/pfast/barabasi-albert/%.dot: $(RESULTS_DIR)graphs/barabasi-albert/%.dot  all | experiment_dirs
	timeout $(TIMEOUT) $(PROG) deps -i "$<" -a pfast -o $@ -m "$$(dirname "$$(dirname $@)")/stats.csv" || true

$(RESULTS_DIR)experiments/pfast/erdos-renyi/%.dot: $(RESULTS_DIR)graphs/erdos-renyi/%.dot  all | experiment_dirs
	timeout $(TIMEOUT) $(PROG) deps -i "$<" -a pfast -o $@ -m "$$(dirname "$$(dirname $@)")/stats.csv" || true

$(RESULTS_DIR)experiments/pfast/out-degree-3-3/%.dot: $(RESULTS_DIR)graphs/out-degree-3-3/%.dot  all | experiment_dirs
	timeout $(TIMEOUT) $(PROG) deps -i "$<" -a pfast -o $@ -m "$$(dirname "$$(dirname $@)")/stats.csv" || true


.PRECIOUS: $(RESULTS_DIR)graphs/barabasi-albert/%.dot
$(RESULTS_DIR)graphs/barabasi-albert/%.dot: all | graph_dirs
	$(PROG) generate -t "$$(basename "$$(dirname $@)")" -g barabasi-albert -o $@

.PRECIOUS: $(RESULTS_DIR)graphs/erdos-renyi/%.dot
$(RESULTS_DIR)graphs/erdos-renyi/%.dot: all | graph_dirs
	$(PROG) generate -t "$$(basename "$$(dirname $@)")" -g erdos-renyi -o $@

.PRECIOUS: $(RESULTS_DIR)graphs/out-degree-3-3/%.dot
$(RESULTS_DIR)graphs/out-degree-3-3/%.dot: all | graph_dirs
	$(PROG) generate -t "$$(basename "$$(dirname $@)")" -g out-degree -o $@

.PHONY: graph_dirs
graph_dirs: $(shell $(call EXP_DIRS,graphs/barabasi-albert)) \
	$(shell $(call EXP_DIRS,graphs/erdos-renyi)) \
	$(shell $(call EXP_DIRS,graphs/out-degree-3-3))

.PHONY: experiment_dirs
experiment_dirs: $(shell $(call EXP_DIRS,experiments/pradet/barabasi-albert)) \
	$(shell $(call EXP_DIRS,experiments/pradet/erdos-renyi)) \
	$(shell $(call EXP_DIRS,experiments/pradet/out-degree-3-3)) \
	$(shell $(call EXP_DIRS,experiments/mem-fast/barabasi-albert)) \
	$(shell $(call EXP_DIRS,experiments/mem-fast/erdos-renyi)) \
	$(shell $(call EXP_DIRS,experiments/mem-fast/out-degree-3-3)) \
	$(shell $(call EXP_DIRS,experiments/pfast/barabasi-albert)) \
	$(shell $(call EXP_DIRS,experiments/pfast/erdos-renyi)) \
	$(shell $(call EXP_DIRS,experiments/pfast/out-degree-3-3))

$(RESULTS_DIR)%/:
	mkdir -p $@


ifneq ($(wildcard $(RESULTS_DIR)experiments),)
ifneq ($(shell which gnuplot 2>/dev/null),)

PLOTS_DIR := plots/
GNUPLOT_SCRIPTS_DIR := gnuplot/
GNUPLOT_SCRIPTS := $(wildcard $(GNUPLOT_SCRIPTS_DIR)*.gp)

STATS_FILES := $(shell find $(RESULTS_DIR)experiments -name stats.csv)
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
