set terminal wxt size 800,600
set title 'Продажи по месяцам'
set xlabel 'Месяц'
set ylabel 'Сумма'
set style data histograms
set style fill solid
set xtics rotate
plot 'data.dat' using 2:xtic(1) with boxes lc rgb 'blue' notitle
pause -1 'Нажмите Enter для сохранения в PNG...'
set terminal png size 800,600
set output 'graph.png'
replot
set output
