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

int my_yield=0;
int opt_yield = 0;

int yield_i = 0;
int yield_d = 0;
int yield_l = 0;

int list_length;

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


void* thread_function_lister(void* trd_info) {
    
    int thread_num = (*((struct thread_info *) trd_info)).thread_num;
    
  
    int i = thread_num;
    for(i = thread_num; i < total_runs; i+=num_of_threads){
        
        
        switch(lock_type){
            case MUTEX:
                pthread_mutex_lock(&my_mutex);
                SortedList_insert(list, &elem_arr[i]);
                pthread_mutex_unlock(&my_mutex);
                break;
            case SPIN_LOCK:
                
                while(__sync_lock_test_and_set(&my_spin_lock, 1));
                SortedList_insert(list, &elem_arr[i]);
                __sync_lock_release(&my_spin_lock);
                break;
            default:
                SortedList_insert(list, &elem_arr[i]);
                break;
            
        }
    }
    
    
    
    list_length = SortedList_length(list);
    SortedListElement_t *temp;
    
    for(i = thread_num; i < total_runs; i+=num_of_threads){
        switch(lock_type){
            case MUTEX:
                pthread_mutex_lock(&my_mutex);
                temp = SortedList_lookup(list, elem_arr[i].key);
                SortedList_delete(temp);
                pthread_mutex_unlock(&my_mutex);
                break;
            case SPIN_LOCK:
                while(__sync_lock_test_and_set(&my_spin_lock, 1));
                temp = SortedList_lookup(list, elem_arr[i].key);
                SortedList_delete(temp);
                __sync_lock_release(&my_spin_lock);
                break;
            default:
                temp = SortedList_lookup(list, elem_arr[i].key);
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
        {NULL, 0, 0, 0},
    };
    
    num_of_threads = 1;
    num_of_iterations = 1;
    int c, option_index;
    
    
    while((c = getopt_long(argc, argv, "t::i::y:x:", long_options, &option_index)) != -1)
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
    
    list = malloc(sizeof(SortedList_t));
    list->key = NULL;
    list->next = list;
    list->prev = list;
    
    elem_arr = malloc(total_runs*sizeof(SortedListElement_t));
    int i = 0;
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
    
    list_length = SortedList_length(list);
    
    if(list_length != 0)
    {
        fprintf(stderr, "List length not 0, synch problem!\n");
        exit(2);
    }
    
    free(tinfo);
    free(list);
    free(elem_arr);
    
    
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
    fprintf(stdout, "%s,%lld,%lld,1,%lld,%lld,%lld\n", tagger, num_of_threads, num_of_iterations, num_of_operations, my_elapsed_time_in_ns , (( my_elapsed_time_in_ns )/( num_of_operations)));
    
    return 0;
}

