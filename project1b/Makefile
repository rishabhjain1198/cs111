#NAME:	RISHABH JAIN
#EMAIL:	rishabh.jain1198@gmail.com
#ID:	604817863


default: lab1b-server.c lab1b-client.c
	cc -Wall -Wextra lab1b-server.c -o lab1b-server -lmcrypt
	cc -Wall -Wextra lab1b-client.c -o lab1b-client -lmcrypt

dist: lab1b-server.c lab1b-client.c Makefile README my.key
	tar -cvzf lab1b-604817863.tar.gz lab1b-client.c lab1b-server.c Makefile README my.key

clean: 
	rm -f lab1b-604817863.tar.gz lab1b-client lab1b-server
