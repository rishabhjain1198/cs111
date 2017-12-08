#NAME:	RISHABH JAIN
#EMAIL:	rishabhjain@ucla.edu
#ID:	604817863

default:
	make build

build: lab4c_tcp.c lab4c_tls.c
	cc -Wall -Wextra -lm -lmraa -pthread -o lab4c_tcp lab4c_tcp.c
	cc -Wall -Wextra -lm -lmraa -pthread -o lab4c_tls lab4c_tls.c -lssl -lcrypto

dist: lab4c_tls.c lab4c_tcp.c Makefile README
	tar -cvzf lab4c-604817863.tar.gz lab4c_tls.c lab4c_tcp.c Makefile README

check: 
	make
	make clean

clean: 
	rm -f lab4c-604817863.tar.gz lab4c_tls lab4c_tcp

