#! /usr/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2b_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#
# output:
#	lab2_list-1.png ... cost per operation vs threads and iterations
#	lab2_list-2.png ... threads and iterations that run (un-protected) w/o failure
#	lab2_list-3.png ... threads and iterations that run (protected) w/o failure
#	lab2_list-4.png ... cost per operation vs number of threads
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#
#	Early in your implementation, you will not have data for all of the
#	tests, and the later sections may generate errors for missing data.
#

# general plot parameters
set terminal png
set datafile separator ","

# throughput vs number of threads for mutex and spin-lock synchronized adds and list operations
set title "2b - 1: Throughput vs Threads for mutex and spin-lock in list"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_1.png'

plot \
     "< grep -E \"list-none-m,[0-9]+,1000,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'mutex' with linespoints lc rgb 'red', \
     "< grep -E \"list-none-s,[0-9]+,1000,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'spin-lock' with linespoints lc rgb 'green'


set title "2b - 2: Time per op and Avg lock wait time vs Threads for mutex"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Time"
set logscale y 10
set output 'lab2b_2.png'
set key left top


plot \
     "< grep -E \"list-none-m,[0-9]+,1000,\" lab2b_list.csv" using ($2):($7) \
	title 'time per op' with linespoints lc rgb 'red', \
     "< grep -E \"list-none-m,[0-9]+,1000,\" lab2b_list.csv" using ($2):($8) \
	title 'avg wait time for a lock' with linespoints lc rgb 'green'


set title "2b - 3: Successful iterations"
set logscale x 2
set xrange [0.75:]
set xlabel "Threads"
set ylabel "successful iterations"
set logscale y 10
set output 'lab2b_3.png'
set key right top
plot \
    "< grep -E \"list-id-none,[0-9]+,[0-9]+,4,\" lab2b_list.csv" using ($2):($3) \
	with points lc rgb "red" title "Unprotected", \
    "< grep -E \"list-id-m,[0-9]+,[0-9]+,4,\" lab2b_list.csv" using ($2):($3) \
	with points lc rgb "green" title "Mutex", \
    "< grep -E \"list-id-s,[0-9]+,[0-9]+,4,\" lab2b_list.csv" using ($2):($3) \
	with points lc rgb "blue" title "Spin-Lock"

set title "2b - 4: Sublist throughput vs number of threads - mutex"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_4.png'


plot \
"< grep -E \"list-none-m,[0-9],1000,1,|list-none-m,12,1000,1,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
title '1 list' with linespoints lc rgb 'red', \
"< grep -E \"list-none-m,[0-9],1000,4,|list-none-m,12,1000,4,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
title '4 lists' with linespoints lc rgb 'green', \
"< grep -E \"list-none-m,[0-9],1000,8,|list-none-m,12,1000,8,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
title '8 lists' with linespoints lc rgb 'blue', \
"< grep -E \"list-none-m,[0-9],1000,16,|list-none-m,12,1000,16,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
title '16 lists' with linespoints lc rgb 'orange'

set title "2b - 5: Sublist throughput vs number of threads - spin-lock"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_5.png'


plot \
"< grep -E \"list-none-s,[0-9]+,1000,1,|list-none-s,12,1000,1,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
title '1 list' with linespoints lc rgb 'red', \
"< grep -E \"list-none-s,[0-9]+,1000,4,|list-none-s,12,1000,4,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
title '4 lists' with linespoints lc rgb 'green', \
"< grep -E \"list-none-s,[0-9]+,1000,8,|list-none-s,12,1000,8,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
title '8 lists' with linespoints lc rgb 'blue', \
"< grep -E \"list-none-s,[0-9]+,1000,16,|list-none-s,12,1000,16,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
title '16 lists' with linespoints lc rgb 'orange'
