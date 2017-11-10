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
set title "2b - 1: Thoughput vs Threads for mutex and spin-lock in list"
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

# time per op and avg wait for lock time vs number of threads for mutex
set title "2b - 2: Time per op and Avg lock wait time vs Threads for mutex"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Time"
set logscale y 10
set output 'lab2b_2.png'
set key left top

# grep out only successful (sum=0) yield runs
plot \
     "< grep -E \"list-none-m,[0-9]+,1000,\" lab_2b_list.csv" using ($2):($7) \
	title 'time per op' with linespoints lc rgb 'red', \
     "< grep -E \"list-none-m,[0-9]+,1000,\" lab_2b_list.csv" using ($2):($8) \
	title 'avg wait time for a lock' with linespoints lc rgb 'green'
