//NAME: RISHABH JAIN
//EMAIL: rishabhjain@ucla.edu
//ID:   604817863

//THERE IS AMBIGUITY ABOUT WHETHER COMMANDS SHOULD BE OUPUTTED
//TO STDOUT WHEN LOGGING IS DISABLED IN PARTICULAR.
//HENCE, FORCED TO MAKE AN ASSUMPTION.
//ASSUMING THAT COMMANDS SHOULD NEVER BE OUTPUTTED TO STDOUT.

#include<unistd.h>
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
#include<netdb.h>
#include<netinet/in.h>
#include<fcntl.h>


#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

SSL *ssl;
int ignoreReading = 0;
long periodAmount = 1;
int scale = 'F';

int loggingEnabled = 0;
FILE *logFile;
int loggingToggle;
int true = 1;
int generateReports = 1;

mraa_aio_context sensVar;

int portNumber = -1;

char *uid;
int uidGiven = 0;

char *hostName;
int hostGiven = 0;

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

    //fprintf(stderr, "%s %s\n", time, "SHUTDOWN");
    char tempBuffer[80] = "";
    int count = sprintf(tempBuffer, "%s %s\n", time, "SHUTDOWN");
    SSL_write(ssl, tempBuffer, count);
    
    
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

float randomRead()    {
    float min = 15.0;
    float max = 30.0;
    float scale = rand() / (float) RAND_MAX;
    return min + scale * (max - min);
}

void * logTemperature(){
    while (true) {
	if(generateReports){
        
        float temp = properTemp(mraa_aio_read(sensVar));

        //float temp = randomRead();
        
        
        
        
        if(scale == 'F')
            temp = temp*1.8 + 32.0;
        
        char time[20];
        
        myTime(time);
        
        if(loggingToggle && loggingEnabled) {
            fprintf(logFile, "%s %.1f\n", time, temp);
        }
        
        if (loggingToggle) {
            //fprintf(stderr, "%s %.1f\n", time, temp);
            char tempBuffer[80] = "";
            int count = sprintf(tempBuffer, "%s %.1f\n", time, temp);
            SSL_write(ssl, tempBuffer, count);
        }
        
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
        
        int readAmount = SSL_read(ssl, buf, 1024);
        
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
        {"id", required_argument, 0, 'i'},
        {"host", required_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    int opt;
    while((opt = getopt_long(argc, argv, "p:s:l:i:h:", long_options, NULL)) != -1) {
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
            case 'i':
                uid = optarg;
                uidGiven = 1;
                break;
            case 'h':
                hostName = optarg;
                hostGiven = 1;
                break;
            default:
                fprintf(stderr, "Please enter valid argument!\n");
                exit(1);
        }
    }
    
    int breakCode = 0;
    
    if(!hostGiven)  {
        fprintf(stderr, "Please enter host number!\n");
        exit(1);
    }
    
    if(!uidGiven)    {
        fprintf(stderr, "Please enter UID!\n");
        exit(1);
    }
    
    if(!loggingEnabled) {
        fprintf(stderr, "Please give log file name!\n");
        exit(1);
    }
    
    int index = optind;
    for(index = optind; index < argc; index++)  {
        if(breakCode)   {
            fprintf(stderr, "Please enter valid arguments!\n");
            exit(1);
        }
        //fprintf(stderr, "YELLO %s\n", argv[index]);
        portNumber = atoi(argv[index]);
        breakCode = 1;
    }
    
    if(portNumber == -1)    {
        fprintf(stderr, "Please enter valid port number!\n");
        exit(1);
    }
    
    //OPENING TCP CONNECTION
    

    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)  {
        fprintf(stderr, "ERROR OPENING SOCKET\n");
        exit(1);
    }
    server = gethostbyname(hostName);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portNumber);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "ERROR CONNECTING SOCKET");
        exit(1);
    }

    
    OpenSSL_add_all_algorithms();

    if(SSL_library_init() < 0)  {
        fprintf(stderr, "Could not initialize the OpenSSL library !\n");
        exit(1);
    }
    const SSL_METHOD *method = SSLv23_client_method();
    
    SSL_CTX *ctx;
    
    if ( (ctx = SSL_CTX_new(method)) == NULL)   {
        fprintf(stderr, "Unable to create a new SSL context structure.\n");
        exit(1);
    }
    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
    
    ssl = SSL_new(ctx);
  
    SSL_set_fd(ssl, sockfd);

    if ( SSL_connect(ssl) != 1 )    {
        fprintf(stderr, "Error: Could not build a SSL session to: %s.\n", hostName);
        exit(1);
    }

 
    X509 *cert = SSL_get_peer_certificate(ssl);
    if (cert == NULL)   {
        fprintf(stderr, "Error: Could not get a certificate from: %s.\n", hostName);
        exit(1);
    }
  
    X509_NAME *certname = X509_NAME_new();
    certname = X509_get_subject_name(cert);
    
    if(loggingEnabled)  {
        logFile = fopen(logFileName, "a");
        if(logFile == NULL) {
            fprintf(stderr, "ERROR IN OPENING LOG FILE: %s\n", strerror(errno));
            exit(1);
        }


    }


    char logUid[80] = "";
    strcat(logUid, "ID=");
    strcat(logUid, uid);
    strcat(logUid, "\n");
    int count = sprintf(logUid, "ID=%s\n", uid);

    //fprintf(stderr, "%s", logUid);
    SSL_write(ssl, logUid, count);
    fprintf(logFile, "%s", logUid);

    
    

    sensVar = mraa_aio_init(1);

if(!sensVar) {fprintf(stderr, "SENSOR NOT WORKING\n"); exit(1); }

    pthread_t *threads = malloc(2 * sizeof(pthread_t));

    pthread_create(threads, NULL, logTemperature, NULL);
    pthread_create(threads+1, NULL, readInput, NULL);
   
    pthread_join(*(threads), NULL);
    pthread_join(*(threads+1), NULL);
       
    free(threads);
    
    if(loggingEnabled)
        fclose(logFile);
  
    mraa_aio_close(sensVar);

    return 0;
}
