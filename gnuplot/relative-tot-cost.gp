set terminal pdfcairo

set title 'Total Cost Relative to Test Suite Size'
set xlabel 'Test Suite Size'
set ylabel 'Total Cost Found/Test Suite Size'

graph_generators = "barabasi-albert erdos-renyi out-degree-3-3"

do for [graph in graph_generators] {
  set output './plots/relative-tot-cost-'.graph.'.pdf'

  plot './results/ex-linear/'.graph.'/stats.dat' using 1:($28/$1):($27/$1):($31/$1):($30/$1) title 'Ex Linear' with candlesticks whiskerbars, \
       './results/pradet/'.graph.'/stats.dat' using 1:($28/$1):($27/$1):($31/$1):($30/$1) title 'Pradet' with candlesticks whiskerbars, \
       './results/big-table/'.graph.'/stats.dat' using 1:($28/$1):($27/$1):($31/$1):($30/$1) title 'mem-fast' with candlesticks whiskerbars
}
