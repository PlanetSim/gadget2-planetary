set terminal png nocrop size 768,576
set output 'forsterite_plot.png'
set title "Isobaric phase diagram for forsterite.\nIsobars running from 1.0 to 53303926300.0 dyn/cm^2.
set xlabel "Entropy /(erg/gK)"
set ylabel "Temperature /K"
set xrange [35000000.0:80000000.0]
set yrange [2000.0:10000.0]
set style line 1 lt 1 lw 1
set style line 2 lt 3 lw 2
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
'forsterite_phase.txt' u 1:44 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:45 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:46 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:47 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:48 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:49 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:50 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:51 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:52 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:53 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:54 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:55 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:56 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:57 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:58 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:59 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:60 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:61 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:62 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:63 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:64 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:65 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:66 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:67 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:68 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:69 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:70 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:71 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:72 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:73 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:74 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:75 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:76 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:77 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:78 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:79 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:80 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:81 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:82 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:83 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:84 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:85 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:86 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:87 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:88 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:89 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:90 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:91 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:92 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:93 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:94 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:95 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:96 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:97 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:98 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:99 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:100 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:101 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:102 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:103 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:104 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:105 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:106 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:107 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:108 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:109 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:110 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:111 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:112 w lines title '' ls 1,\
'forsterite_phase.txt' u 1:113 w lines title 'P = 1056354.44dyn/cm^2.' ls 2,\
'forsterite_peaks.txt' u 1:2 w points title ''
