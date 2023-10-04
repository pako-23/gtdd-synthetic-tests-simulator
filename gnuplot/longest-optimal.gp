set terminal pdfcairo

set title 'Longest Schedule Found Over Optimal Longest Schedule'
set xlabel 'Test Suite Size'
set ylabel 'Longest Schedule Found/Optimal Longest Schedule'

# set yrange [0.8:1.2]

graph_generators = "barabasi-albert"

do for [graph in graph_generators] {
  set output './plots/longest-optimal-'.graph.'.pdf'

  plot './results/experiments/pfast/'.graph.'/stats.dat' using 1:($18/$13):($17/$12):($21/$16):($20/$15) title 'PFAST' with candlesticks whiskerbars, \
       './results/experiments/pradet/'.graph.'/stats.dat' using 1:($18/$13):($17/$12):($21/$16):($20/$15) title 'PraDet' with candlesticks whiskerbars, \
       './results/experiments/mem-fast/'.graph.'/stats.dat' using 1:($18/$13):($17/$12):($21/$16):($20/$15) title 'MEM-FAST' with candlesticks whiskerbars
}
