set terminal pdfcairo
set output './plots/test-runs-barabasi-albert.pdf'

set title '# of Tests Runned'
set xlabel 'Test Suite Size'
set ylabel 'Test  Runned'

set datafile separator ','

set style circle radius 0.1
set style fill solid 1 noborder

plot './results/ex-linear/barabasi-albert/stats.csv' using 1:3 with circles title 'Ex Linear', \
     './results/big-table/barabasi-albert/stats.csv' using 1:3 with circles title 'Big Table', \
     './results/pradet/barabasi-albert/stats.csv' using 1:3 with circles title 'Pradet'