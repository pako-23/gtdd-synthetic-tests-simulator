set terminal pdfcairo

set title '# of Tests Runned'
set xlabel 'Test Suite Size'
set ylabel 'Test Runned'

graph_generators = "barabasi-albert erdos-renyi out-degree-3-3"


do for [graph in graph_generators] {
  set output './plots/test-runs-'.graph.'.pdf'

  plot './results/ex-linear/'.graph.'/stats.dat' using 1:8:7:11:10 title 'Ex Linear' with candlesticks whiskerbars, \
       './results/pradet/'.graph.'/stats.dat' using 1:8:7:11:10 title 'Pradet' with candlesticks whiskerbars

}
