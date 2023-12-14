set terminal pdfcairo

set xlabel 'Test Suite Size'
set ylabel 'Test Suite Runs'
set style fill solid noborder

graph_generators = "barabasi-albert erdos-renyi out-degree-3-3"

do for [graph in graph_generators] {
  set output './plots/test-suite-runs-'.graph.'.pdf'

  plot './results/experiments/pfast/'.graph.'/stats.dat' using 1:3:2:6:5  title 'PFAST' with candlesticks lc '#5252cc' lw 1.5 whiskerbars, \
       './results/experiments/pradet/'.graph.'/stats.dat' using 1:3:2:6:5 title 'PraDet' with candlesticks lc '#52cc52' lw 1.5 whiskerbars
}
