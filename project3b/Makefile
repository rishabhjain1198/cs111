#NAME:	RISHABH JAIN
#EMAIL:	rishabhjain@ucla.edu
#ID:	604817863

default:
	make build

build: lab3b.py
	rm -f lab3b
	ln -s lab3b.py lab3b
	chmod +x lab3b

dist: lab3b.py Makefile README
	tar -cvzf lab3b-604817863.tar.gz lab3b.py Makefile README

clean: 
	rm -f lab3b-604817863.tar.gz lab3b

