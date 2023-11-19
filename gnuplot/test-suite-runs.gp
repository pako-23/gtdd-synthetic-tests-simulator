set terminal pdfcairo

set title '# of Test Suites Runned'
set xlabel 'Test Suite Size'
set ylabel 'Test Suites Runned'

graph_generators = "barabasi-albert erdos-renyi out-degree-3-3"

do for [graph in graph_generators] {
  set output './plots/test-suite-runs-'.graph.'.pdf'

  plot './results/experiments/pfast/'.graph.'/stats.dat' using 1:3:2:6:5  title 'PFAST' with candlesticks whiskerbars, \
       './results/experiments/pradet/'.graph.'/stats.dat' using 1:3:2:6:5 title 'PraDet' with candlesticks whiskerbars
}
