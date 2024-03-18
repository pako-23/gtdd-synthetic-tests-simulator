CXX := g++
CXXFLAGS := -O3 -Wall -Werror -pedantic -std=c++20
SRC_DIR := src/
BUILD_DIR := build/
PROG_NAME := synthetic-tests-simulator
PROG := $(BUILD_DIR)$(PROG_NAME)

SRCS := $(wildcard $(SRC_DIR)*.cc)
H_FILES := $(wildcard $(SRC_DIR)*.h)
OBJS := $(SRCS:$(SRC_DIR)%.cc=$(BUILD_DIR)%.o)
DEPENDS := $(OBJS:%.o=%.d)

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

MAX_RUNS ?= 50
MIN_TESTS ?= 2
MAX_TESTS ?= 500
TESTS_STEP ?= 10
TIMEOUT ?= 30m
RESULTS_DIR := ./results/
PROBABILITIES := 0.0001 0.0005 0.001 0.005 0.01 0.02 0.05

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

define MEMFAST_EXP_FILES
for i in $(PROBABILITIES); do \
	for j in $$(seq 1 $(MAX_RUNS)); do \
		echo $(RESULTS_DIR)$1/probability-$$i/graph-$$j.$2; \
	done \
done
endef

define MEMFAST_DIRS
for i in $(PROBABILITIES); do \
	echo $(RESULTS_DIR)$1/probability-$$i/; \
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
mem_fast_experiments: $(shell $(call MEMFAST_EXP_FILES,experiments/pfast/fixed-probability,dot)) \
	$(shell $(call MEMFAST_EXP_FILES,experiments/pradet/fixed-probability,dot)) \
	$(shell $(call MEMFAST_EXP_FILES,experiments/mem-fast/fixed-probability,txt))


$(RESULTS_DIR)experiments/pradet/barabasi-albert/%.dot: $(RESULTS_DIR)graphs/barabasi-albert/%.dot $(PROG) | experiment_dirs
	timeout $(TIMEOUT) $(PROG) deps -i "$<" -a pradet -o $@ -m "$$(dirname "$$(dirname $@)")/stats.csv" || true

$(RESULTS_DIR)experiments/pradet/erdos-renyi/%.dot: $(RESULTS_DIR)graphs/erdos-renyi/%.dot $(PROG) | experiment_dirs
	timeout $(TIMEOUT) $(PROG) deps -i "$<" -a pradet -o $@ -m "$$(dirname "$$(dirname $@)")/stats.csv" || true

$(RESULTS_DIR)experiments/pradet/out-degree-3-3/%.dot: $(RESULTS_DIR)graphs/out-degree-3-3/%.dot $(PROG) | experiment_dirs
	timeout $(TIMEOUT) $(PROG) deps -i "$<" -a pradet -o $@ -m "$$(dirname "$$(dirname $@)")/stats.csv" || true


$(RESULTS_DIR)experiments/pradet/fixed-probability/%.dot: $(RESULTS_DIR)graphs/fixed-probability/%.dot $(PROG) | memfast_experiment_dirs
	timeout $(TIMEOUT) $(PROG) deps -i "$<" -a pradet -o $@ -m "$$(dirname $@)/stats.csv" || true

$(RESULTS_DIR)experiments/pfast/fixed-probability/%.dot: $(RESULTS_DIR)graphs/fixed-probability/%.dot $(PROG) | memfast_experiment_dirs
	timeout $(TIMEOUT) $(PROG) deps -i "$<" -a pfast -o $@ -m "$$(dirname $@)/stats.csv" || true

$(RESULTS_DIR)experiments/mem-fast/fixed-probability/%.txt: $(RESULTS_DIR)graphs/fixed-probability/%.dot $(PROG) | memfast_experiment_dirs
	timeout $(TIMEOUT) $(PROG) deps -i "$<" -a mem-fast -o $@ -m "$$(dirname $@)/stats.csv" || true


$(RESULTS_DIR)experiments/pfast/barabasi-albert/%.dot: $(RESULTS_DIR)graphs/barabasi-albert/%.dot $(PROG) | experiment_dirs
	timeout $(TIMEOUT) $(PROG) deps -i "$<" -a pfast -o $@ -m "$$(dirname "$$(dirname $@)")/stats.csv" || true

$(RESULTS_DIR)experiments/pfast/erdos-renyi/%.dot: $(RESULTS_DIR)graphs/erdos-renyi/%.dot $(PROG) | experiment_dirs
	timeout $(TIMEOUT) $(PROG) deps -i "$<" -a pfast -o $@ -m "$$(dirname "$$(dirname $@)")/stats.csv" || true

$(RESULTS_DIR)experiments/pfast/out-degree-3-3/%.dot: $(RESULTS_DIR)graphs/out-degree-3-3/%.dot $(PROG) | experiment_dirs
	timeout $(TIMEOUT) $(PROG) deps -i "$<" -a pfast -o $@ -m "$$(dirname "$$(dirname $@)")/stats.csv" || true


.PRECIOUS: $(RESULTS_DIR)graphs/barabasi-albert/%.dot
$(RESULTS_DIR)graphs/barabasi-albert/%.dot: $(PROG) | graph_dirs
	$(PROG) generate -t "$$(basename "$$(dirname $@)")" -g barabasi-albert -o $@

.PRECIOUS: $(RESULTS_DIR)graphs/erdos-renyi/%.dot
$(RESULTS_DIR)graphs/erdos-renyi/%.dot: $(PROG) | graph_dirs
	$(PROG) generate -t "$$(basename "$$(dirname $@)")" -g erdos-renyi -o $@

.PRECIOUS: $(RESULTS_DIR)graphs/out-degree-3-3/%.dot
$(RESULTS_DIR)graphs/out-degree-3-3/%.dot: $(PROG) | graph_dirs
	$(PROG) generate -t "$$(basename "$$(dirname $@)")" -g out-degree -o $@

.PRECIOUS: $(RESULTS_DIR)graphs/fixed-probability/probability-%.dot
$(RESULTS_DIR)graphs/fixed-probability/probability-%.dot: $(PROG) | memfast_graph_dirs
	$(PROG) generate -t 50 -g erdos-renyi -p "$$(basename "$$(dirname $@)" | cut -d'-' -f2)" -o $@


.PHONY: graph_dirs
graph_dirs: $(shell $(call EXP_DIRS,graphs/barabasi-albert)) \
	$(shell $(call EXP_DIRS,graphs/erdos-renyi)) \
	$(shell $(call EXP_DIRS,graphs/out-degree-3-3))

.PHONY: experiment_dirs
experiment_dirs: $(shell $(call EXP_DIRS,experiments/pradet/barabasi-albert)) \
	$(shell $(call EXP_DIRS,experiments/pradet/erdos-renyi)) \
	$(shell $(call EXP_DIRS,experiments/pradet/out-degree-3-3)) \
	$(shell $(call EXP_DIRS,experiments/pfast/barabasi-albert)) \
	$(shell $(call EXP_DIRS,experiments/pfast/erdos-renyi)) \
	$(shell $(call EXP_DIRS,experiments/pfast/out-degree-3-3))

.PHONY: memfast_experiment_dirs
memfast_experiment_dirs: $(shell $(call MEMFAST_DIRS,experiments/pradet/fixed-probability)) \
	$(shell $(call MEMFAST_DIRS,experiments/pfast/fixed-probability)) \
	$(shell $(call MEMFAST_DIRS,experiments/mem-fast/fixed-probability))

.PHONY: memfast_graph_dirs
memfast_graph_dirs: $(shell $(call MEMFAST_DIRS,graphs/fixed-probability))


$(RESULTS_DIR)%/:
	mkdir -p $@


ifneq ($(wildcard $(RESULTS_DIR)experiments),)
ifneq ($(shell which gnuplot 2>/dev/null),)

PLOTS_DIR := plots/
GNUPLOT_SCRIPTS_DIR := gnuplot/
GNUPLOT_SCRIPTS := $(wildcard $(GNUPLOT_SCRIPTS_DIR)*.gp)

STATS_FILES := $(shell find $(RESULTS_DIR)experiments -name stats.csv)
DAT_FILES := $(STATS_FILES:%.csv=%.dat)

CANDLESTICKS := $(filter-out %-box-plot.gp,$(GNUPLOT_SCRIPTS))
BOX_PLOTS := $(filter %-box-plot.gp,$(GNUPLOT_SCRIPTS))

PLOTS := $(CANDLESTICKS:$(GNUPLOT_SCRIPTS_DIR)%.gp=$(PLOTS_DIR)%-barabasi-albert.pdf) \
	$(CANDLESTICKS:$(GNUPLOT_SCRIPTS_DIR)%.gp=$(PLOTS_DIR)%-erdos-renyi.pdf) \
	$(CANDLESTICKS:$(GNUPLOT_SCRIPTS_DIR)%.gp=$(PLOTS_DIR)%-out-degree-3-3.pdf) \
	$(BOX_PLOTS:$(GNUPLOT_SCRIPTS_DIR)%.gp=$(PLOTS_DIR)%.pdf)

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

$(PLOTS_DIR)%-box-plot.pdf: $(GNUPLOT_SCRIPTS_DIR)%-box-plot.gp | plots_dir
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
