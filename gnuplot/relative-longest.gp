set terminal pdfcairo

set title 'Longest Schedule Relative to Test Suite Size'
set xlabel 'Test Suite Size'
set ylabel 'Longest Schedule Found/Test Suite Size'

graph_generators = "barabasi-albert erdos-renyi out-degree-3-3"

do for [graph in graph_generators] {
  set output './plots/relative-longest-'.graph.'.pdf'

  plot './results/experiments/pfast/'.graph.'/stats.dat' using 1:($18/$1):($17/$1):($21/$1):($20/$1) title 'PFAST' with candlesticks whiskerbars, \
       './results/experiments/pradet/'.graph.'/stats.dat' using 1:($18/$1):($17/$1):($21/$1):($20/$1) title 'PraDet' with candlesticks whiskerbars
}
