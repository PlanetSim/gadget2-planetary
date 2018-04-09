#set terminal png nocrop size 768,576
#set output 'forsterite_plot.png'
set title "Isobaric phase diagram for forsterite. Isobars running from 318.871868 to 85637699800.0 dyn/cm^2.
set xlabel "Entropy /(erg/gK)"
set ylabel "Temperature /K"
set xrange [30000000.0:80000000.0]
set yrange [0:10000.0]
set style line 1 lt 1 lw 1
set style line 2 lt 3 lw 2

A = 7500
B = 1.05e7
C = 5.4e7
D = 1000

f(x) = A*exp( -(x-C)*(x-C)/B/B/2.0 ) + D

fit f(x) 'peaks.txt' u 1:2 via A,B,C,D

plot 'forsterite_phase.txt' u 1:2 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:3 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:4 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:5 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:6 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:7 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:8 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:9 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:10 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:11 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:12 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:13 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:14 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:15 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:16 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:17 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:18 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:19 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:20 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:21 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:22 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:23 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:24 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:25 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:26 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:27 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:28 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:29 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:30 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:31 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:32 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:33 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:34 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:35 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:36 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:37 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:38 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:39 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:40 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:41 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:42 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:43 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:44 w lines title 'P = 318.871868dyn/cm^2.' ls 2,\
'peaks.txt' u 1:2 w points title '',\
f(x) w lines title ''
