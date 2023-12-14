set terminal pdfcairo

set xlabel 'Probability'
set ylabel 'Test Suite Runs'
set datafile separator ','
set boxwidth 0.1
set xrange [0.5:5.5]
set logscale y 8
set style fill solid noborder
set style boxplot nooutliers

set key left top

set xtics ("0.0005" 1, "0.001" 2, "0.005" 3, "0.01" 4, "0.02" 5)
set output './plots/test-suite-runs-box-plot.pdf'

plot './results/experiments/pfast/fixed-probability/probability-0.0005/stats.csv' using (1):2 title 'PFAST' with boxplot  lc '#5252cc' lw 1.5, \
     './results/experiments/pfast/fixed-probability/probability-0.001/stats.csv' using (2):2 notitle with boxplot lc '#5252cc' lw 1.5, \
     './results/experiments/pfast/fixed-probability/probability-0.005/stats.csv' using (3):2 notitle with boxplot  lc '#5252cc' lw 1.5, \
     './results/experiments/pfast/fixed-probability/probability-0.01/stats.csv' using (4):2 notitle with boxplot lc '#5252cc' lw 1.5, \
     './results/experiments/pfast/fixed-probability/probability-0.02/stats.csv' using (5):2 notitle with boxplot lc '#5252cc' lw 1.5, \
     './results/experiments/pradet/fixed-probability/probability-0.0005/stats.csv' using (0.7):2 title 'PraDet' with boxplot  lc '#52cc52' lw 1.5, \
     './results/experiments/pradet/fixed-probability/probability-0.001/stats.csv' using (1.7):2 notitle with boxplot lc '#52cc52' lw 1.5, \
     './results/experiments/pradet/fixed-probability/probability-0.005/stats.csv' using (2.7):2 notitle with boxplot  lc '#52cc52' lw 1.5, \
     './results/experiments/pradet/fixed-probability/probability-0.01/stats.csv' using (3.7):2 notitle with boxplot lc '#52cc52' lw 1.5, \
     './results/experiments/pradet/fixed-probability/probability-0.02/stats.csv' using (4.7):2 notitle with boxplot lc '#52cc52' lw 1.5, \
     './results/experiments/mem-fast/fixed-probability/probability-0.0005/stats.csv' using (1.2):2 title 'MEM-FAST' with boxplot  lc '#cc5258' lw 1.5, \
     './results/experiments/mem-fast/fixed-probability/probability-0.001/stats.csv' using (2.2):2 notitle with boxplot lc '#cc5258' lw 1.5, \
     './results/experiments/mem-fast/fixed-probability/probability-0.005/stats.csv' using (3.2):2 notitle with boxplot  lc '#cc5258' lw 1.5, \
     './results/experiments/mem-fast/fixed-probability/probability-0.01/stats.csv' using (4.2):2 notitle with boxplot lc '#cc5258' lw 1.5, \
     './results/experiments/mem-fast/fixed-probability/probability-0.02/stats.csv' using (5.2):2 notitle with boxplot lc '#cc5258' lw 1.5
