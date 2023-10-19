set terminal pdfcairo

set title 'Total Cost Over Optimal Total Cost'
set xlabel 'Test Suite Size'
set ylabel 'Total Cost Found/Optimal Total Cost'

set yrange [0.8:1.2]

graph_generators = "barabasi-albert erdos-renyi out-degree-3-3"

do for [graph in graph_generators] {
  set output './plots/tot-cost-optimal-'.graph.'.pdf'

  plot './results/experiments/pfast/'.graph.'/stats.dat' using 1:($28/$23):($27/$22):($31/$26):($30/$25) title 'PFAST' with candlesticks whiskerbars, \
       './results/experiments/pradet/'.graph.'/stats.dat' using 1:($28/$23):($27/$22):($31/$26):($30/$25) title 'PraDet' with candlesticks whiskerbars, \
       './results/experiments/mem-fast/'.graph.'/stats.dat' using 1:($28/$23):($27/$22):($31/$26):($30/$25)  title 'MEM-FAST' with candlesticks whiskerbars
}
