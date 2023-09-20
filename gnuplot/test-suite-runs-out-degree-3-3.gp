set terminal pdfcairo
set output './plots/test-suite-runs-out-degree-3-3.pdf'

set title '# of Test Suites Runned'
set xlabel 'Test Suite Size'
set ylabel 'Test Suites Runned'

set datafile separator ','

set style circle radius 0.1
set style fill solid 1 noborder

plot './results/ex-linear/out-degree-3-3/stats.csv' using 1:2 with circles title 'Ex Linear', \
     './results/big-table/out-degree-3-3/stats.csv' using 1:2 with circles title 'Big Table', \
     './results/pradet/out-degree-3-3/stats.csv' using 1:2 with circles title 'Pradet'
