//NAME: RISHABH JAIN
//EMAIL: rishabh.jain1198@gmail.com
//ID: 604817863

#include<pthread.h>
#include<getopt.h>
#include<stdio.h>
#include<fcntl.h>
#include<signal.h>
#include<string.h>
#include<unistd.h>
#include<getopt.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<termios.h>
#include<sys/poll.h>
#include<sys/time.h>
#include<sys/wait.h>
#include <string.h>


#define NO_LOCK 0
#define MUTEX 1
#define SPIN_LOCK 2
#define COMPARE_AND_SWAP 3

int lock_type = 0;
long long num_of_iterations;
pthread_mutex_t my_mutex;
long long my_spin_lock;
long long counter = 0;
int opt_yield = 0;

struct thread_info {    /* Used as argument to thread_start() */
           pthread_t thread_id;        /* ID returned by pthread_create() */
           int       thread_num;       /* Application-defined thread # */
           char     *argv_string;      /* From command-line argument */
};

void add(long long *pointer, long long value) {
	long long sum = *pointer + value;
	if(opt_yield)
		sched_yield();
	*pointer = sum;
	return ;
}

void cas_add(long long *pointer, long long value) {
	long long temp = *pointer;

	while(__sync_val_compare_and_swap(pointer, temp, temp+value) != temp) {
		temp = *pointer;
	}

	return;
}

void* thread_function_adder() {
    int i = 0;
	for(i = 0; i < num_of_iterations; ++i){

		switch(lock_type) {

			case NO_LOCK:
				{
					add(&counter, 1);
					break;
				}

			case MUTEX:
				{
					pthread_mutex_lock(&my_mutex);
					add(&counter, 1);
					pthread_mutex_unlock(&my_mutex);
					break;
				}

			case SPIN_LOCK:
				{
					while(__sync_lock_test_and_set(&my_spin_lock, 1));
					add(&counter, 1);
					__sync_lock_release(&my_spin_lock);
					break;
				}
			case COMPARE_AND_SWAP:
				{
					//TO WRITE CODE HERE
					cas_add(&counter, 1);
					break;
				}

		} //SWITCH BLOCK ENDS HERE FOR ADDITION




	}	//FOR LOOP ENDS HERE


	//DO ANOTHER FOR LOOP HERE TO SUBTRACT -1

	for(i = 0; i < num_of_iterations; ++i) {
		//CALL ADD HERE TO SUBTRACT
		//ONLY COPY PASTE CODE HERE FROM ABOVE
		//DONT WRITE CODE HERE INDEPENDENTLY, IT HAS TO BE SYMMETRICAL


				switch(lock_type) {

					case NO_LOCK:
						{
							add(&counter, -1);
							break;
						}

					case MUTEX:
						{
							pthread_mutex_lock(&my_mutex);
							add(&counter, -1);
							pthread_mutex_unlock(&my_mutex);
							break;
						}

					case SPIN_LOCK:
						{
							while(__sync_lock_test_and_set(&my_spin_lock, -1));
							add(&counter, -1);
							__sync_lock_release(&my_spin_lock);
							break;
						}
					case COMPARE_AND_SWAP:
						{
							//TO WRITE CODE HERE
							cas_add(&counter, -1);
							break;
						}

				} //SWITCH BLOCK ENDS HERE

	} //FOR LOOP ENDS HERE FOR SUBTRACTION

	return NULL;

}	//FUNCTION ENDS HERE



int main(int argc, char **argv){

	// USING GET OPT TO PARSE ARGUMENTS

	static struct option long_options[] =
	{
		{ "threads", required_argument, 0, 't'},
		{ "iterations", required_argument, 0, 'i'},
		{ "yield", no_argument, 0, 'y'},
		{ "sync", required_argument, 0, 'x'},
        {NULL, 0, 0, 0},
	};

	long long num_of_threads = -1;
	num_of_iterations = -1;
	int c, option_index;


	while((c = getopt_long(argc, argv, "t:i:yx:", long_options, &option_index)) != -1)
	{
			switch(c){

				case 't':
					num_of_threads = atoi(optarg);
					break;

				case 'i':
					num_of_iterations = atoi(optarg);
					break;

				case 'y':
					//fprintf(stderr, "YIELD RECEIVED MADARCHOD\n");
					opt_yield = 1;
					break;

				case 'x':
					//fprintf(stderr, "SYNC RECEIVED BHENCHOD\n");
					switch(optarg[0]){

						case 'm':
							lock_type = MUTEX;
							break;
						case 's':
							lock_type = SPIN_LOCK;
							break;
						case 'c':
						  lock_type = COMPARE_AND_SWAP;
							break;
					}
					break;

				case '?':
					fprintf(stderr, "Unknown argument found %x\n", optopt);
					exit(1);
					break;
			}

	}

	// CHECKING IF THREADS OR ITERATIONS FLAG IS ABSENT
	if(num_of_iterations == -1 || num_of_threads == -1)
	{
		fprintf(stderr, "Specify number of iterations and threads properly!\n");
		exit(1);
	}


	//PARSING OF ARGUMENTS DONE
    
    if(lock_type == MUTEX)
        pthread_mutex_init(&my_mutex, NULL);

	int s;
	pthread_attr_t attr;

	s = pthread_attr_init(&attr);
	if(s != 0){
		fprintf(stderr, "Error in initializing thread attributes!\n");
		exit(1);
	}

	struct thread_info *tinfo = malloc(num_of_threads * sizeof(struct thread_info));
	if (tinfo == NULL) {
		fprintf(stderr, "Error in allocating memory for thread info!\n");
		exit(1);
	}

	struct timespec my_start_time;
	clock_gettime(CLOCK_MONOTONIC, &my_start_time);

    int i = 0;
	for(i = 0; i < num_of_threads; i++){

		tinfo[i].thread_num = i + 1;
		tinfo[i].argv_string = "";

		int ret = pthread_create(&tinfo[i].thread_id, &attr, &thread_function_adder, &tinfo[i]);
		if(ret != 0) {
			fprintf(stderr, "Error in creating threads!\n");
			exit(1);
		}

	}

	s = pthread_attr_destroy(&attr);
	if (s != 0) {
		fprintf(stderr, "Error in destroying attributes of threads\n");
		exit(1);
	}

	for(i = 0; i < num_of_threads; i++){

		int ret = pthread_join(tinfo[i].thread_id, NULL);
		//CHECK ret here as well
		if( ret != 0) {
			fprintf(stderr, "Error in joining threads!\n");
			exit(1);
		}
	}

	struct timespec my_end_time;
	clock_gettime(CLOCK_MONOTONIC, &my_end_time);

	long long my_elapsed_time_in_ns = (my_end_time.tv_sec - my_start_time.tv_sec) * 1000000000;
	my_elapsed_time_in_ns += my_end_time.tv_nsec;
	my_elapsed_time_in_ns -= my_start_time.tv_nsec;

	free(tinfo);

	//WRITE TO STDOUT NOW


	// fprintf(stdout, "elapsed time is %lld\n", my_elapsed_time_in_ns);
	// fprintf(stdout, "counter value is %lld\n", counter);

	char tagger [100];
	strcpy(tagger, "add");

  if(opt_yield)
	 	strcat(tagger, "-yield");

	switch(lock_type) {
		case NO_LOCK:
			strcat(tagger, "-none");
			break;
		case MUTEX:
			strcat(tagger, "-m");
			break;
		case SPIN_LOCK:
			strcat(tagger, "-s");
			break;
		case COMPARE_AND_SWAP:
			strcat(tagger, "-c");
			break;
	}

	fprintf(stdout, "%s,%lld,%lld,%lld,%lld,%lld,%lld\n", tagger, num_of_threads, num_of_iterations, (num_of_threads*num_of_iterations*2),(my_elapsed_time_in_ns), ((my_elapsed_time_in_ns)/(num_of_threads*num_of_iterations*2)), counter);

	return 0;
}
