#NAME:	RISHABH JAIN
#EMAIL:	rishabh.jain1198@gmail.com
#ID:	604817863


default: lab2_list.c SortedList.c SortedList.h
	cc -g -lprofiler -Wall -Wextra -pthread lab2_list.c SortedList.c -o lab2_list

build: lab2_list.c SortedList.c SortedList.h
	cc -g -lprofiler -Wall -Wextra -pthread lab2_list.c SortedList.c -o lab2_list

profile: lab2_list
	-rm -f ./raw.gperf
	-rm -f profile.out
	CPUPROFILE=./raw.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text ./lab2_list ./raw.gperf >> profile.out
	pprof --list=thread_function_lister ./lab2_list ./raw.gperf >> profile.out
	-rm -f ./raw.gperf

tests: lab2_list
	-rm -f lab2b_list.csv
	-./lab2_list --threads=1  --iterations=1000 --sync=m > lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2b_list.csv


	-./lab2_list --threads=1  --iterations=1   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=2   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=4   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=8   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=16  --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=1   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=2   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=4   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=8   --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=16  --yield=id --lists=4 >> lab2b_list.csv


	-./lab2_list --threads=1  --iterations=10  --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=20  --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=40  --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=80  --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=10  --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=20  --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=40  --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=80  --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=10  --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=20  --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=40  --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=80  --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=10  --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=20  --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=40  --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=80  --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=10  --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=20  --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=40  --yield=id --lists=4 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=80  --yield=id --lists=4 --sync=m >> lab2b_list.csv

	-./lab2_list --threads=1  --iterations=10  --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=20  --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=40  --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=80  --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=10  --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=20  --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=40  --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=80  --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=10  --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=20  --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=40  --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=80  --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=10  --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=20  --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=40  --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=80  --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=10  --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=20  --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=40  --yield=id --lists=4 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=80  --yield=id --lists=4 --sync=s >> lab2b_list.csv


	-./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=1000 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --sync=s >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv

	-./lab2_list --threads=1  --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv

	-./lab2_list --threads=1  --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv

	-./lab2_list --threads=1  --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv




graphs: lab2b_list.csv lab2_list.gp
	-rm -f lab2b_1.png
	-rm -f lab2b_2.png
	-rm -f lab2b_3.png
	-rm -f lab2b_4.png
	-rm -f lab2b_5.png
	gnuplot lab2_list.gp

dist: Makefile lab2_list.c SortedList.c SortedList.h README lab2_list.gp
	-rm -f lab2b-604817863.tar.gz lab2_list *.csv *.png profile.out
	make build
	make tests
	make graphs
	make profile
	tar -cvzf lab2b-604817863.tar.gz Makefile lab2_list.c SortedList.c SortedList.h lab2b_list.csv README lab2_list.gp \
	lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png profile.out

clean:
	-rm -f lab2b-604817863.tar.gz lab2_list
