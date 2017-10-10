//NAME: RISHABH JAIN
//EMAIL:    rishabh.jain1198@gmail.com
//ID:   604817863

#include<fcntl.h>
#include<signal.h>
#include<string.h>
#include<unistd.h>
#include<getopt.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

void nullAssign(void);
void segmentationHandler(int signum);

int main(int argc, char **argv)
{
	int inputfd = 0;	//file descriptor for input, stdin by default
	char* inputFileName;

	int outputfd = 1;	//file descriptor for ouput, stdout by default
	char* outputFileName;

	int causeSegFault = 0;   //flag to check if fault is required
	int catchFault = 0;	//flag to check if fault is to be caught

	//CODE TO PARSE ARGUMENTS
	static struct option long_options[] =
	{
		{ "input", required_argument, 0, 'i'},
		{ "output", required_argument, 0, 'o'},
		{ "segfault", no_argument, 0, 's'},
		{ "catch", no_argument, 0, 'c'},
        {NULL, 0, 0, 0},
	};

	int c, option_index;

	while((c = getopt_long(argc, argv, "i:o:sc", long_options, &option_index)) != -1)
	{
		switch(c){
				
		case 'i':

			if((inputfd = open(optarg, O_RDONLY)) < 0)
			{
				//ERROR IN OPENING FILE; ABORT
				fprintf(stderr, "Error in opening input file %s, check your --input argument:\n%s\n", optarg, strerror(errno));
				exit(2);
			}
			else
			{
				dup2(inputfd, 0);
                close(inputfd);
				inputFileName = optarg;
               // fprintf(stdout, "input file name:   %s\n", inputFileName);
			}
			
			break;

		case 'o':
			if( (outputfd = open(optarg, O_CREAT | O_WRONLY)) < 0)
			{
				//ERROR IN OPENING FILE; ABORT
				fprintf(stderr, "Error in opening output file %s, check your --output argument:\n%s\n", optarg, strerror(errno));
				exit(3);
			}
			else
			{
				dup2(outputfd, 1);
                close(outputfd);
				outputFileName = optarg;
			}
			break;

		case 's':
			causeSegFault = 1;
			break;

		case 'c':
			catchFault = 1;
			break;

		//UNKNOWN FLAG | ARGUMENT HANDLING
		case '?':
			if(optopt == 'i')
			{
				//fprintf(stderr, "--input flag requires an argument.\n");
				exit(2);
			}
			else if(optopt == 'o')
			{
				//fprintf(stderr, "--output flag requires an argument.\n");
				exit(3);
			}
			else
			{
				//fprintf(stderr, "Unknown argument found %x\n", optopt);
				exit(1);
			}
		}
	}




	//Arguments successfully parsed if code has reached here
	
	if(catchFault == 1)
	{
		signal(SIGSEGV, &segmentationHandler);
	}
	
	if(causeSegFault == 1)
	{
		nullAssign();
	}


	//TIME TO READ FROM INPUT AND WRITE TO OUTPUT
	int bufferSize = 10;		//edit this according to preference
	int i;
	char* s = malloc(bufferSize * sizeof(*s));
    
	while( (i = read(0, s, bufferSize)) != 0)
	{
		if(i < 0)
		{
			//ERROR
			fprintf(stderr, "error in reading file %s\n %s\n", inputFileName, strerror(errno));

			exit(2);
		}

		//TIME TO WRITE TO OUTPUT, MAKE SURE TO WRITE i BYTES
		
		if( write(1, s, i) < i )
		{
			//ERROR
			fprintf(stderr, "error in writing to file %s\n %s\n", outputFileName, strerror(errno));
			
			exit(3);
		}
	}

	
    close(0);
    close(1);
	
	return 0;
}

void nullAssign(void)
{
	char* empty = NULL;
	fprintf(stdout, "Unused variable warning here unless used %c", *empty);
	return;
}

void segmentationHandler(int signum)
{
	fprintf(stderr, "CAUGHT: Segmentation error occured, but was caught.    %d\n ", signum);
	exit(4);
	return;
}
