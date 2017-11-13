#NAME:	RISHABH JAIN
#EMAIL:	rishabhjain@ucla.edu
#ID:	604817863

default: lab4b.c
	cc -Wall -Wextra -lmraa -o lab4b lab4b.c

dist: lab4b.c Makefile README
	tar -cvzf lab4b-604817863.tar.gz lab4b.c Makefile README

check: lab4b.c
	make
	echo “OFF” | ./lab4b 
	make clean

clean: 
	rm -f lab4b-604817863.tar.gz lab4b
