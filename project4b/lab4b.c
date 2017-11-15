//NAME: RISHABH JAIN
//EMAIL: rishabhjain@ucla.edu
//ID:   604817863

//THERE IS AMBIGUITY ABOUT WHETHER COMMANDS SHOULD BE OUPUTTED
//TO STDOUT WHEN LOGGING IS DISABLED IN PARTICULAR.
//HENCE, FORCED TO MAKE AN ASSUMPTION.
//ASSUMING THAT COMMANDS SHOULD NEVER BE OUTPUTTED TO STDOUT.

#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <errno.h>
#include <mraa.h>
#include <aio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include<unistd.h>

int ignoreReading = 0;
long periodAmount = 1;
int scale = 'F';

int loggingEnabled = 0;
FILE *logFile;
int loggingToggle;
int true = 1;
int generateReports = 1;

mraa_aio_context sensVar;
mraa_gpio_context butVar;

char *logFileName;

void myTime(char *unixTime){
    time_t t;
    time(&t);
    struct tm* convert = localtime(&t);
    if(!convert) exit(2);
    strftime(unixTime, 9, "%H:%M:%S", convert);
    return;
}

void turnOff(){
    char time[20];
    myTime(time);

    fprintf(stdout, "%s %s\n", time, "SHUTDOWN");

    if(loggingEnabled)	{
	fprintf(logFile, "%s\n", "OFF");
        fprintf(logFile, "%s %s\n", time, "SHUTDOWN");
    }
    
    exit(0);
    return;
}

void printStartToggle(int toggle){
    generateReports = toggle;
    if(loggingEnabled){
        if(toggle == 0)
            fprintf(logFile, "STOP\n");
        else if(toggle == 1)
            fprintf(logFile, "START\n");
    }
    return;
}

void printScaleToggle(char toggle){
    scale = toggle;
    if(loggingEnabled){
        if(toggle == 'C')
            fprintf(logFile, "SCALE=C\n");
        else if(toggle == 'F')
            fprintf(logFile, "SCALE=F\n");
    }
    return;
}

void changePeriod(char *buf){
    char *str = buf + strlen("PERIOD=");
    char *end;
    periodAmount = strtol(str, &end, 10);
    if(loggingEnabled)
        fprintf(logFile, "PERIOD=%lu\n", periodAmount);
    return;
}

float properTemp(int a){

    float R5V = 1023.0/a-1.0;
    float R3V3 = 660.0/a-1.0;	//FROM PIAZZA
    int R0 = 100000;
    R5V = R0*R5V; R3V3 = R0*R3V3;
    int B = 4275; 
    float temperature = 0.0;

    int USEFIVEVOLTAGE = 1;

    if(USEFIVEVOLTAGE)
	temperature = 1.0/(log(R5V/R0)/B+1/298.15)-273.15;	
    else
	temperature = 1.0/(log(R3V3/R0)/B+1/298.15)-273.15;

    return temperature;
}

void * readButton(){
    while(true){
        int reading = mraa_gpio_read(butVar);
        if(reading){
            char time[20];
            myTime(time);
            if(loggingEnabled) fprintf(logFile, "%s %s\n", time, "SHUTDOWN");
            exit(0);
        }
    }
    
    return NULL;
}

void * logTemperature(){
    while (true) {
	if(generateReports){
        float temp = properTemp(mraa_aio_read(sensVar));
        if(scale == 'F')
            temp = temp*1.8 + 32.0;
        
        char time[20];
        
        myTime(time);
        
        if(loggingToggle && loggingEnabled) fprintf(logFile, "%s %.1f\n", time, temp);
        
        if (loggingToggle) fprintf(stdout, "%s %.1f \n", time, temp);
        
        if(!ignoreReading) ignoreReading=1; //only once
        
        sleep(periodAmount);
}
    }
    return NULL;
}


void * readInput(){
    char buf[1024];
    
    while(true) {
        
        if(!ignoreReading) continue;
        
        int readAmount = read(STDIN_FILENO, buf, 1024);
        
        if(readAmount < 0) exit(1);
        
        else if (readAmount > 0){
            int i; int previousReceived = 0;
            for(i = 0; i < readAmount; i++){
                if(buf[i] == '\n'){
                    buf[i] = 0 ;
                    if(strcmp(buf + previousReceived, "OFF") == 0) turnOff();
                    else if(strncmp(buf + previousReceived, "PERIOD=", strlen("PERIOD=")) == 0) changePeriod(buf + previousReceived);
                    else if(strcmp(buf + previousReceived, "SCALE=F") == 0) printScaleToggle('F');
                    else if (strcmp(buf + previousReceived, "SCALE=C") == 0) printScaleToggle('C');
                    else if(strcmp(buf + previousReceived, "STOP") == 0) printStartToggle(0);
                    else if(strcmp(buf + previousReceived, "START") == 0) printStartToggle(1);
                    else    //UNRECOGNIZED COMMAND
                        exit(1);
                    
                    previousReceived = i + 1;
                }
            }
        }
    }
    
    return NULL;
}

int main(int argc, char **argv){
    loggingToggle = 1;
    static struct option long_options[] = {
        {"period", required_argument, 0, 'p'},
        {"scale", required_argument, 0, 's'},
        {"log", required_argument, 0, 'l'},
        {0, 0, 0, 0}
    };
    int opt;
    while((opt = getopt_long(argc, argv, "p:s:l:", long_options, NULL)) != -1) {
        switch(opt) {
            case 'p':
                periodAmount = atoi(optarg);
                break;
            case 's':
                if(optarg[0] == 'C')
                    scale = 'C';
                else if(optarg[0] == 'F')
                    scale = 'F';
                else    {
                    fprintf(stderr, "Please enter C or F for scale value!\n");
                    exit(2);
                }
                break;
            case 'l':
                logFileName = optarg;
                loggingEnabled = 1;
                break;
            default:
                fprintf(stderr, "Please enter valid argument!\n");
                exit(1);
        }
    }
    
    if(loggingEnabled)
        logFile = fopen(logFileName, "a");
     
    sensVar = mraa_aio_init(1);
    butVar = mraa_gpio_init(73);
 if(!butVar) {fprintf(stderr, "BUTTON NOT FOUND\n"); exit(1);}

if(!sensVar) {fprintf(stderr, "SENSOR NOT WORKING\n"); exit(1); }

    pthread_t *threads = malloc(3 * sizeof(pthread_t));

    pthread_create(threads, NULL, logTemperature, NULL);
    pthread_create(threads+1, NULL, readButton, NULL);
    pthread_create(threads+2, NULL, readInput, NULL);
   
    pthread_join(*(threads), NULL);
    pthread_join(*(threads+1), NULL);
    pthread_join(*(threads+2), NULL);
       
    free(threads);
    
    if(loggingEnabled)
        fclose(logFile);
  
    mraa_aio_close(sensVar);
    mraa_gpio_close(butVar);

    return 0;
}
