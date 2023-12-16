set terminal pdfcairo size 3.1,2.1 font 'Helvetica,15'

LCases='abcdefghijklmnopqrstuvwxyz'
SCases='ᴀʙᴄᴅᴇғɢʜɪᴊᴋʟᴍɴᴏᴘǫʀsᴛᴜᴠᴡxʏᴢ'

toscchr(c)= c eq ''  ?  ''  :  substr( SCases.c, strstrt(LCases.c, c), strstrt(LCases.c, c) )

texsc(s) = strlen(s) <= 1 ? toscchr(s) : texsc(s[1:strlen(s)/2]).texsc(s[(strlen(s)/2)+1:strlen(s)])


set xlabel 'Test Suite Size'
set ylabel "# Schedule Runs\n(Thousands)"
set style fill solid noborder
set format y "%.0f"
set xtics 100
set ytics 20
set key left top


graph_generators = "barabasi-albert erdos-renyi out-degree-3-3"

do for [graph in graph_generators] {
  set output './plots/test-suite-runs-'.graph.'.pdf'

  plot './results/experiments/pfast/'.graph.'/stats.dat' using 1:($3/1000):($2/1000):($6/1000):($5/1000)  title texsc('Pfast') with candlesticks lc '#5252cc' lw 1.5 whiskerbars, \
       './results/experiments/pradet/'.graph.'/stats.dat' using 1:($3/1000):($2/1000):($6/1000):($5/1000) title texsc('PraDet') with candlesticks lc '#52cc52' lw 1.5 whiskerbars
}
