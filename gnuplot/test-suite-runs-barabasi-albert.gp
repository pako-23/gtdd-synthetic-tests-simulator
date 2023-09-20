set terminal pdfcairo
set output './plots/test-suite-runs-barabasi-albert.pdf'

set title '# of Test Suites Runned'
set xlabel 'Test Suite Size'
set ylabel 'Test Suites Runned'

set datafile separator ','


set style circle radius 0.1
set style fill solid 1 noborder

plot './results/ex-linear/barabasi-albert/stats.csv' using 1:2 with circles title 'Ex Linear', \
     './results/big-table/barabasi-albert/stats.csv' using 1:2 with circles title 'Big Table', \
     './results/pradet/barabasi-albert/stats.csv' using 1:2 with circles title 'Pradet'
