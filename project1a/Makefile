#NAME:	RISHABH JAIN
#EMAIL:	rishabh.jain1198@gmail.com
#ID:	604817863

execname=lab1a

default: lab1a.c
	cc -Wall -Wextra -o $(execname) lab1a.c

dist: lab1a.c Makefile README
	tar -cvzf lab1a-604817863.tar.gz lab1a.c Makefile README

clean: 
	rm -f lab1a-604817863.tar.gz lab1a
