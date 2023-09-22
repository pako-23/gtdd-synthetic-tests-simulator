set terminal pdfcairo

set title 'Longest Schedule Found Over Optimal Longest Schedule'
set xlabel 'Test Suite Size'
set ylabel 'Longest Schedule Found/Optimal Longest Schedule'

set yrange [0.8:1.2]

graph_generators = "barabasi-albert erdos-renyi out-degree-3-3"

do for [graph in graph_generators] {
  set output './plots/longest-optimal-'.graph.'.pdf'

  plot './results/ex-linear/'.graph.'/stats.dat' using 1:($18/$13):($17/$12):($21/$16):($20/$15) title 'Ex Linear' with candlesticks whiskerbars, \
       './results/pradet/'.graph.'/stats.dat' using 1:($18/$13):($17/$12):($21/$16):($20/$15) title 'Pradet' with candlesticks whiskerbars
}
