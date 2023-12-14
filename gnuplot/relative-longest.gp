set terminal pdfcairo

set xlabel 'Test Suite Size'
set ylabel 'Longest Schedule Found/Test Suite Size'
set style fill solid noborder

graph_generators = "barabasi-albert erdos-renyi out-degree-3-3"

do for [graph in graph_generators] {
  set output './plots/relative-longest-'.graph.'.pdf'

  plot './results/experiments/pfast/'.graph.'/stats.dat' using 1:($18/$1):($17/$1):($21/$1):($20/$1) title 'PFAST' with candlesticks lc '#5252cc' lw 1.5 whiskerbars, \
       './results/experiments/pradet/'.graph.'/stats.dat' using 1:($18/$1):($17/$1):($21/$1):($20/$1) title 'PraDet' with candlesticks lc '#52cc52' lw 1.5 whiskerbars
}
