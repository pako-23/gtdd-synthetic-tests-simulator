set terminal pdfcairo

set xlabel 'Test Suite Size'
set ylabel 'Total Cost Found/Test Suite Size'
set style fill solid noborder

graph_generators = "barabasi-albert erdos-renyi out-degree-3-3"

do for [graph in graph_generators] {
  set output './plots/relative-tot-cost-'.graph.'.pdf'

  plot './results/experiments/pfast/'.graph.'/stats.dat' using 1:($28/$1):($27/$1):($31/$1):($30/$1) title 'PFAST' with candlesticks lc '#5252cc' lw 1.5 whiskerbars, \
       './results/experiments/pradet/'.graph.'/stats.dat' using 1:($28/$1):($27/$1):($31/$1):($30/$1) title 'PraDet' with candlesticks lc '#52cc52' lw 1.5 whiskerbars
}
