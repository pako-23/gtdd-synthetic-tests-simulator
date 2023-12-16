set terminal pdfcairo font "Helvetica,15"

LCases='abcdefghijklmnopqrstuvwxyz'
SCases='ᴀʙᴄᴅᴇғɢʜɪᴊᴋʟᴍɴᴏᴘǫʀsᴛᴜᴠᴡxʏᴢ'

toscchr(c)= c eq ''  ?  ''  :  substr( SCases.c, strstrt(LCases.c, c), strstrt(LCases.c, c) )

texsc(s) = strlen(s) <= 1 ? toscchr(s) : texsc(s[1:strlen(s)/2]).texsc(s[(strlen(s)/2)+1:strlen(s)])

set xlabel 'Test Suite Size'
set ylabel 'Total Cost Found/Optimal Total Cost'
set style fill solid noborder

set yrange [0.8:1.2]

graph_generators = "barabasi-albert erdos-renyi out-degree-3-3"

do for [graph in graph_generators] {
  set output './plots/tot-cost-optimal-'.graph.'.pdf'

  plot './results/experiments/pfast/'.graph.'/stats.dat' using 1:($28/$23):($27/$22):($31/$26):($30/$25) title texsc('Pfast') with candlesticks lc '#5252cc' lw 1.5 whiskerbars, \
       './results/experiments/pradet/'.graph.'/stats.dat' using 1:($28/$23):($27/$22):($31/$26):($30/$25) title texsc('PraDet') with candlesticks lc '#52cc52' lw 1.5 whiskerbars
}
