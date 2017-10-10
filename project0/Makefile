#NAME: RISHABH JAIN
#EMAIL: rishabh.jain1198@gmail.com
#ID: 604817863

execname=lab0

default: lab0.c
	cc -Wall -Wextra -o $(execname) lab0.c
check: lab0.c
	@cc -Wall -Wextra -o $(execname) lab0.c

	@echo "STDIN TEST" > expect; cat expect | ./$(execname) > test
	@diff test expect || { echo "Stdin to Stdout test failed"; rm test expect; exit 1; }

	@echo "file input test to stdout" > infile && ./$(execname) --input=infile > test
	@diff test infile || { echo "File input to Stdout test failed"; rm infile test; exit 1; }

	@echo "" > outfile
	@echo "stdin to file output" > expect; cat expect | ./$(execname) --output=outfile
	@diff outfile expect || { echo "Stdin to File Output test failed"; rm outfile expect; exit 1; }

	@echo "file input to file output" > infile; ./$(execname) --input=infile --output=outfile
	@diff infile outfile || { echo "File input to file output test failed"; rm outfile infile; exit 1; }

	@./lab0 -s -c || echo "All tests passed"
	@rm test expect infile outfile lab0

dist: lab0.c Makefile README backtrace.png breakpoint.png
	tar -cvzf lab0-604817863.tar.gz lab0.c Makefile README backtrace.png breakpoint.png

clean:
	rm -f lab0 lab0-604817863.tar.gz
