set terminal pdfcairo

set title 'Total Cost Over Optimal Total Cost'
set xlabel 'Test Suite Size'
set ylabel 'Total Cost Found/Optimal Total Cost'

# set yrange [0.8:1.2]

graph_generators = "barabasi-albert erdos-renyi out-degree-3-3"

do for [graph in graph_generators] {
  set output './plots/tot-cost-optimal-'.graph.'.pdf'

  plot './results/ex-linear/'.graph.'/stats.dat' using 1:($28/$23):($27/$22):($31/$26):($30/$25) title 'Ex Linear' with candlesticks whiskerbars, \
       './results/pradet/'.graph.'/stats.dat' using 1:($28/$23):($27/$22):($31/$26):($30/$25) title 'Pradet' with candlesticks whiskerbars, \
       './results/big-table/'.graph.'/stats.dat' using 1:($28/$23):($27/$22):($31/$26):($30/$25)  title 'mem-fast' with candlesticks whiskerbars

}
