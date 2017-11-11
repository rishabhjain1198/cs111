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

#include "SortedList.h"


#define NO_LOCK 0
#define MUTEX 1
#define SPIN_LOCK 2

int lock_type = 0;
long long num_of_threads;
long long num_of_iterations;
pthread_mutex_t my_mutex;
int my_spin_lock = 0;
long long counter = 0;

long long total_runs = 0;

long long number_of_locks = 0;
long long lock_up_time = 0;

long long num_lists = 1;

int my_yield=0;
int opt_yield = 0;

int yield_i = 0;
int yield_d = 0;
int yield_l = 0;


typedef struct {
  SortedList_t list;
  pthread_mutex_t lock;
  int spin_lock;
} SubList_t;

SubList_t *lists;
SortedList_t *list;
SortedListElement_t *elem_arr;

struct thread_info {    /* Used as argument to thread_start() */
    pthread_t thread_id;        /* ID returned by pthread_create() */
    int       thread_num;       /* Application-defined thread # */
    char     *argv_string;      /* From command-line argument */
};

void sigSegment() {
    fprintf(stderr, "Segmentation fault occured, caught\n");
    exit(2);
}

unsigned long hash(const char *key) {
    unsigned long val = 5381;
    int i = 0;
    for (i = 0; i < 5; i++)
        val = ((val << 5) + val) + key[i];
    return val;
}


void* thread_function_lister(void* trd_info) {

    int thread_num = (*((struct thread_info *) trd_info)).thread_num;
    SortedListElement_t *ele;
    SubList_t *sublist;
    pthread_mutex_t *lock;
    int *spinlock;

    int i = thread_num;
    for(i = thread_num; i < total_runs; i+=num_of_threads){

        ele = &elem_arr[i];
        const char *key = ele -> key;
        sublist = &lists[hash(key) % num_lists];

        switch(lock_type){
            case MUTEX:
              {

                struct timespec my_start;
                clock_gettime(CLOCK_MONOTONIC, &my_start);


                lock = &sublist -> lock;
                pthread_mutex_lock(lock);



                number_of_locks++;
                struct timespec my_end;
                clock_gettime(CLOCK_MONOTONIC, &my_end);
                lock_up_time += (my_end.tv_sec - my_start.tv_sec) * 1000000000;
                lock_up_time += my_end.tv_nsec;
                lock_up_time -= my_start.tv_nsec;


              //  SortedList_insert(list, &elem_arr[i]);
                SortedList_insert(&sublist -> list, ele);

                pthread_mutex_unlock(lock);
                break;
              }
            case SPIN_LOCK:
              {
                struct timespec my_start;
                clock_gettime(CLOCK_MONOTONIC, &my_start);

                spinlock = &sublist -> spin_lock;

                while(__sync_lock_test_and_set(spinlock, 1));


                number_of_locks++;
                struct timespec my_end;
                clock_gettime(CLOCK_MONOTONIC, &my_end);
                lock_up_time += (my_end.tv_sec - my_start.tv_sec) * 1000000000;
                lock_up_time += my_end.tv_nsec;
                lock_up_time -= my_start.tv_nsec;


                // SortedList_insert(list, &elem_arr[i]);
                SortedList_insert(&sublist -> list, ele);

                __sync_lock_release(spinlock);
                break;
              }
            default:
                // SortedList_insert(list, &elem_arr[i]);
                SortedList_insert(&sublist -> list, ele);
                break;

        }
    }



    int sumVar;
    sumVar = 0;
    switch(lock_type) {
      case MUTEX:
      {
      struct timespec my_start;
      clock_gettime(CLOCK_MONOTONIC, &my_start);

      for(i = 0; i < num_lists; i++)
        pthread_mutex_lock(&lists[i].lock);

      struct timespec my_end;
      clock_gettime(CLOCK_MONOTONIC, &my_end);
      lock_up_time += (my_end.tv_sec - my_start.tv_sec) * 1000000000;
      lock_up_time += my_end.tv_nsec;
      lock_up_time -= my_start.tv_nsec;

      for(i = 0; i < num_lists; i++)  {
          sumVar = SortedList_length(&lists[i].list);

      for(i = 0; i < num_lists; i++)
              pthread_mutex_unlock(&lists[i].lock);
      }

      break;
    }
      case SPIN_LOCK:{
      struct timespec my_start;
      clock_gettime(CLOCK_MONOTONIC, &my_start);

      for(i = 0; i < num_lists; i++)
        while(__sync_lock_test_and_set(&lists[i].spin_lock, 1));

        struct timespec my_end;
        clock_gettime(CLOCK_MONOTONIC, &my_end);
        lock_up_time += (my_end.tv_sec - my_start.tv_sec) * 1000000000;
        lock_up_time += my_end.tv_nsec;
        lock_up_time -= my_start.tv_nsec;

      for(i = 0; i < num_lists; i++)
        sumVar = SortedList_length(&lists[i].list);

      for(i = 0; i < num_lists; i++)
        __sync_lock_release(&lists[i].spin_lock);

      break;}

      default:
        for(i = 0; i < num_lists; i++)
          sumVar = SortedList_length(&lists[i].list);

      break;

    }

    if(sumVar < 0)  {
      fprintf(stderr, "Error in list!\n");
      exit(2);

    }


    SortedListElement_t *temp;

    for(i = thread_num; i < total_runs; i+=num_of_threads){
        ele = &elem_arr[i];
        const char *key = ele -> key;
        sublist = &lists[hash(key) % num_lists];
        switch(lock_type){
            case MUTEX:{
              lock = &sublist -> lock;
              struct timespec my_start;
              clock_gettime(CLOCK_MONOTONIC, &my_start);
                pthread_mutex_lock(lock);
                struct timespec my_end;
                clock_gettime(CLOCK_MONOTONIC, &my_end);
                lock_up_time += (my_end.tv_sec - my_start.tv_sec) * 1000000000;
                lock_up_time += my_end.tv_nsec;
                lock_up_time -= my_start.tv_nsec;
                temp = SortedList_lookup(&lists[i].list, key);
                SortedList_delete(temp);
                pthread_mutex_unlock(lock);
                break;}
            case SPIN_LOCK:{
                spinlock = &sublist -> spin_lock;
                struct timespec my_start;
                clock_gettime(CLOCK_MONOTONIC, &my_start);
                while(__sync_lock_test_and_set(spinlock, 1));
                struct timespec my_end;
                clock_gettime(CLOCK_MONOTONIC, &my_end);
                lock_up_time += (my_end.tv_sec - my_start.tv_sec) * 1000000000;
                lock_up_time += my_end.tv_nsec;
                lock_up_time -= my_start.tv_nsec;
                temp = SortedList_lookup(&lists[i].list, key);
                SortedList_delete(temp);
                __sync_lock_release(spinlock);
                break;}
            default:
                temp = SortedList_lookup(&lists[i].list, key);
                SortedList_delete(temp);
                break;

        }
    }


    return NULL;

}    //FUNCTION ENDS HERE



int main(int argc, char **argv){

    // USING GET OPT TO PARSE ARGUMENTS

    static struct option long_options[] =
    {
        { "threads", optional_argument, 0, 't'},
        { "iterations", optional_argument, 0, 'i'},
        { "yield", required_argument, 0, 'y'},
        { "sync", required_argument, 0, 'x'},
        {"lists", required_argument, 0, 'l'},
        {NULL, 0, 0, 0},
    };

    num_of_threads = 1;
    num_of_iterations = 1;
    int c, option_index;


    while((c = getopt_long(argc, argv, "t::i::y:x:l:", long_options, &option_index)) != -1)
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
                my_yield = 1;
                switch(optarg[0]) {
                    case 'i':
                        yield_i = 1;
                        opt_yield |= INSERT_YIELD;
                        break;
                    case 'd':
                        yield_d = 1;
                         opt_yield |= DELETE_YIELD;
                        break;
                    case 'l':
                        yield_l = 1;opt_yield |= LOOKUP_YIELD;
                        break;
                }

                if(optarg[1] != '\0'){
                    switch(optarg[1]) {
                        case 'i':
                            yield_i = 1;
                             opt_yield |= INSERT_YIELD;
                            break;
                        case 'd':
                            yield_d = 1;
                            opt_yield |= DELETE_YIELD;
                            break;
                        case 'l':
                            yield_l = 1;
                            opt_yield |= LOOKUP_YIELD;
                            break;
                    }
                }

                if(optarg[2] != '\0') {
                    switch(optarg[2]) {
                        case 'i':
                            yield_i = 1;
                             opt_yield |= INSERT_YIELD;
                            break;
                        case 'd':
                            yield_d = 1;
                            opt_yield |= DELETE_YIELD;
                            break;
                        case 'l':
                            yield_l = 1;
                            opt_yield |= LOOKUP_YIELD;
                            break;
                    }
                }

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

                }
                break;

            case 'l':
                num_lists = atoi(optarg);
                break;

            case '?':
                fprintf(stderr, "Unknown argument found %x\n", optopt);
                exit(1);
                break;
        }

    }


    //PARSING OF ARGUMENTS DONE

    signal(SIGSEGV, &sigSegment);

    total_runs = num_of_threads * num_of_iterations;

    if(lock_type == MUTEX)
        pthread_mutex_init(&my_mutex, NULL);


    //////////////////
    ////INITIALZING LISTS
    int i = 0;

    if((lists = malloc(sizeof(SubList_t)*num_lists)) == NULL) {
      fprintf(stderr, "Error in allocating memory for sublists!\n");
      exit(1);
    }

    for(i = 0; i < num_lists; i++)  {
      SubList_t *temp_list = &lists[i];
      SortedList_t* my_list = &temp_list -> list;
      my_list -> key = NULL;
      my_list -> next = list;
      my_list -> prev = list;
      if(lock_type == SPIN_LOCK)
        temp_list->spin_lock = 0;
      else if(lock_type == MUTEX) {
        if(pthread_mutex_init(&temp_list->lock, NULL))  {
          fprintf(stderr, "Error in initializing mutex for sublists!");
          exit(1);
        }
      }
    }

  //  fprintf(stderr, "Initialization successful\n");


    elem_arr = malloc(total_runs*sizeof(SortedListElement_t));

    for(i = 0; i < total_runs; i++)
    {
        int rand_letter = rand() % 26;
        char* rand_key = malloc(6*sizeof(char));
        int j = 0;
        for(j = 0; j < 5; j++){
            rand_key[j] = 'A' + rand_letter;
            rand_letter = rand() % 26;
        }
        rand_key[5] = '\0';
        elem_arr[i].key = rand_key;
//        fprintf(stderr, "%d:  %s", i, rand_key);
    }


    //////////////////

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

    for(i = 0; i < num_of_threads; i++){

        tinfo[i].thread_num = i;
        tinfo[i].argv_string = "";

        fprintf(stderr, "Successful until thread creation\n");

        int ret = pthread_create(&tinfo[i].thread_id, &attr, &thread_function_lister, &tinfo[i]);
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

    // list_length = SortedList_length(list);
    //
    // if(list_length != 0)
    // {
    //     fprintf(stderr, "List length not 0, synch problem!\n");
    //     exit(2);
    // }

    int list_length = 0;
    int sumVar = 0;
    for(i = 0; i < num_lists; i++)  {
      sumVar = SortedList_length(&lists[i].list);
      if( sumVar < 0)  {
        fprintf(stderr, "Error in list!\n");
        free(tinfo);
        free(list);
        free(elem_arr);
        exit(2);
      }
      else  {
        list_length += sumVar;
      }
    }

    free(tinfo);
    free(lists);
    free(elem_arr);

    if(list_length != 0)  {
      fprintf(stderr, "Error in list length!\n");
      exit(2);
    }


    char tagger [100];
    strcpy(tagger, "list-");

    if(!my_yield)
        strcat(tagger, "none");

    if(yield_i)
        strcat(tagger, "i");
    if(yield_d)
        strcat(tagger, "d");
    if(yield_l)
        strcat(tagger, "l");

    strcat(tagger, "-");

    switch(lock_type) {
        case MUTEX:
            strcat(tagger, "m");
            break;
        case SPIN_LOCK:
            strcat(tagger, "s");
            break;

        default:
            strcat(tagger, "none");
            break;
    }


    long long num_of_operations = num_of_threads*num_of_iterations*3;
    long long average_lock;

    if(number_of_locks < 1) {
      average_lock = 0;
    }
    else {
      average_lock = lock_up_time/number_of_locks;
    }

    fprintf(stdout, "%s,%lld,%lld,%lld,%lld,%lld,%lld,%lld\n", tagger, num_of_threads, num_of_iterations, num_lists, num_of_operations, my_elapsed_time_in_ns , (( my_elapsed_time_in_ns )/( num_of_operations)), average_lock);

    return 0;
}
