set terminal pdfcairo size 3.1,2.1 font 'Helvetica,15'

LCases='abcdefghijklmnopqrstuvwxyz'
SCases='ᴀʙᴄᴅᴇғɢʜɪᴊᴋʟᴍɴᴏᴘǫʀsᴛᴜᴠᴡxʏᴢ'

toscchr(c)= c eq ''  ?  ''  :  substr( SCases.c, strstrt(LCases.c, c), strstrt(LCases.c, c) )

texsc(s) = strlen(s) <= 1 ? toscchr(s) : texsc(s[1:strlen(s)/2]).texsc(s[(strlen(s)/2)+1:strlen(s)])

set xlabel 'Test Suite Size'
set ylabel "Longest Schedule \nTest Suite Size\n Ratio"
set style fill solid noborder
set xtics 100

graph_generators = "barabasi-albert erdos-renyi out-degree-3-3"

do for [graph in graph_generators] {
  set output './plots/relative-longest-'.graph.'.pdf'

  plot './results/experiments/pfast/'.graph.'/stats.dat' using 1:($18/$1):($17/$1):($21/$1):($20/$1) title texsc('Pfast') with candlesticks lc '#5252cc' lw 1.5 whiskerbars, \
       './results/experiments/pradet/'.graph.'/stats.dat' using 1:($18/$1):($17/$1):($21/$1):($20/$1) title texsc('PraDet') with candlesticks lc '#52cc52' lw 1.5 whiskerbars
}
