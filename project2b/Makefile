#NAME:	RISHABH JAIN
#EMAIL:	rishabh.jain1198@gmail.com
#ID:	604817863


default: lab2_list.c SortedList.c SortedList.h
	cc -g -lprofiler -Wall -Wextra -pthread lab2_list.c SortedList.c -o lab2_list

build: lab2_list.c SortedList.c SortedList.h
	cc -g -lprofiler -Wall -Wextra -pthread lab2_list.c SortedList.c -o lab2_list

profile: lab2_list
	-rm -f ./raw.gperf
	CPUPROFILE=./raw.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text ./lab2_list ./raw.gperf > profile.out
	pprof --list=thread_function_lister ./lab2_list ./raw.gperf >> profile.out
	-rm -f ./raw.gperf

tests: lab2_list
	-rm -f *.csv
	./lab2_list --threads=1 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2b_list.csv






graphs: lab2b_list.csv lab2_list.gp
	gnuplot lab2_list.gp

dist: Makefile lab2_list.c SortedList.c SortedList.h README lab2_list.gp
	-rm -f lab2a-604817863.tar.gz lab2_add lab2_list *.csv *.png
	make tests
	make graphs
	make profile
#FILL THIS LINE YOU FUCKER
	-rm -f *.csv *.png

clean:
	-rm -f lab2b-604817863.tar.gz lab2_list *.csv *.png profile.out