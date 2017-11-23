#NAME:	RISHABH JAIN
#EMAIL:	rishabhjain@ucla.edu
#ID:	604817863

default: 
	make build

build: lab3a.c
	cc -Wall -Wextra -o lab3a lab3a.c

dist: lab3a.c Makefile README ext2_fs.h
	tar -cvzf lab3a-604817863.tar.gz lab3a.c Makefile README ext2_fs.h

check: lab3a.c
	make
	make clean

clean: 
	rm -f lab3a-604817863.tar.gz lab3a
