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
#include<termios.h>
#include<sys/poll.h>
#include<sys/time.h>
#include<sys/wait.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

#include<mcrypt.h>

struct termios saved_attributes;

void sigpipeHandler(int signum);

void reset_input_mode(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &saved_attributes);
}


int main(int argc, char **argv)
{

	int loggingEnabled = 0;
	int logFile;


	int encryptionEnabled = 0;
	char keyString[1024];
	int keyStringSize = 1024;




	//CODE TO PARSE ARGUMENTS
	static struct option long_options[] =
	{
		{ "port", required_argument, 0, 'p'},
		{"log", required_argument, 0, 'l'},
		{"encrypt", required_argument, 0, 'e'},
        {NULL, 0, 0, 0},
	};

	int c, option_index;

	int portno = 8080;
	int pwalaflagged = 0;

	while((c = getopt_long(argc, argv, "p:l:e:", long_options, &option_index)) != -1)
	{
		switch(c){

		case 'p':
		pwalaflagged = 1;
		portno = atoi(optarg);
		//fprintf(stderr,"ael:  %d", portno);
			break;

		case 'l':
			loggingEnabled = 1;
			if((logFile = open(optarg, O_CREAT | O_WRONLY, 0666)) < 0)
			{
				fprintf(stderr, "Error in opening log file for writing %s, check your --log argument:\n%s\n", optarg, strerror(errno));
				exit(1);
			}
			//fprintf(stderr, "LOGGED MATE");
			break;

		case 'e':
			encryptionEnabled = 1;
			int keyFile;
			if((keyFile = open(optarg, O_RDONLY)) < 0)
			{
				fprintf(stderr, "Error in opening key file %s, check your --encrypt argument:\n%s\n", optarg, strerror(errno));
				exit(1);
			}


				if((keyStringSize = read(keyFile, keyString, keyStringSize)) < 0){
					fprintf(stderr, "Error in reading key file %s, check your --encrypt argument:\n%s\n", optarg, strerror(errno));
					exit(1);
				}
			//fprintf(stderr, "yeh lo key: %s\n", keyString);

			break;


		case '?':
			fprintf(stderr, "  %x\n", optopt);
			exit(1);
			break;
		}
	}

	if(!pwalaflagged){
		fprintf(stderr, "Mandatory argument --port not found!\n");
		exit(1);
	}


		//TO TAKE CARE OF SIGPIPE UPON CLOSING OF PIPE
		signal(SIGPIPE, &sigpipeHandler);
		//THIS IS NECESSARY

		struct termios tattr;
		tcgetattr(STDIN_FILENO, &saved_attributes);
		atexit(reset_input_mode);
		tcgetattr(STDIN_FILENO, &tattr);
		tattr.c_iflag = ISTRIP;
		tattr.c_oflag = 0;
		tattr.c_lflag = 0;
		tattr.c_cc[VMIN] = 1;
		tattr.c_cc[VTIME] = 0;
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &tattr);





int sockfd;
struct sockaddr_in serv_addr;
   struct hostent *server;


	 sockfd = socket(AF_INET, SOCK_STREAM, 0);

   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }

   server = gethostbyname("localhost");

   if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }

	 serv_addr.sin_family = AF_INET;
	 bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	 serv_addr.sin_port = htons(portno);

	 if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR connecting");
      exit(1);
   }


	 //ENCRYPTION CODE

	 MCRYPT td1, td2;
	 char *IV;

	 if(encryptionEnabled){


		 td1 = mcrypt_module_open("twofish", NULL, "cfb", NULL);
		   if (td1==MCRYPT_FAILED) {
		      exit(1);
		   }

		IV = malloc(mcrypt_enc_get_iv_size(td1));

		int qwe = 0;
		for (qwe=0; qwe< mcrypt_enc_get_iv_size( td1); qwe++) {
    					IV[qwe]=qwe;
  		}

		qwe=mcrypt_generic_init( td1, keyString, keyStringSize, IV);

		if (qwe<0) {
     mcrypt_perror(qwe);
     exit(1);
  		}



			td2 = mcrypt_module_open("twofish", NULL, "cfb", NULL);
				if (td2==MCRYPT_FAILED) {
					 exit(1);
				}

			int yus = mcrypt_generic_init( td2, keyString, keyStringSize, IV);

			if (yus<0) {
			 mcrypt_perror(yus);
			 exit(1);
				}


	 }






				int bufferSize = 512;		//edit this according to preference
				int i;
				char* s = malloc(bufferSize * sizeof(*s));
				struct pollfd pfds[2];

				pfds[0].fd = STDIN_FILENO;
				pfds[0].events = POLLIN;

				pfds[1].fd = sockfd;
				pfds[1].events = POLLIN;

				int continuepolling = 1;

				while( poll(pfds, 2, -1) && continuepolling) {

					if( (pfds[0].revents & POLLIN)) {


						//READ FROM STDIN AND SEND TO PIPE

						if( (i = read(0, s, bufferSize)) != 0)
						{

							if(i < 0)
							{
								//ERROR
								fprintf(stderr, "error in reading %s\n", strerror(errno));

								exit(1);
							}

							char *socketBuffer = malloc(sizeof(char)*i);
							int bufferCounter = 0;

							//TIME TO WRITE TO OUTPUT, MAKE SURE TO WRITE i BYTES
							int j =0;
							for(j = 0; j < i; j++){

								if(s[j] == 10 || s[j] == 13) {
									char *lol = strdup("   ");
									lol[0] = 13;
									lol[1] = 10;

									if( write(1, lol, 2) < 2) {
										fprintf(stderr, "error in writing to stdout %s\n", strerror(errno));
										exit(1);
									}


									lol[0] = 10; lol[1] = '\0';


									if(encryptionEnabled){
										mcrypt_generic (td1, lol, 1);
									}

									if( write(sockfd, lol, 1) < 0) {
										fprintf(stderr, "error in writing to child pipe %s\n", strerror(errno));
										exit(1);
									}

									socketBuffer[bufferCounter] = lol[0];
									bufferCounter++;

								}

								else	{

									char *lol = strdup(" ");
									lol[0] = s[j];

									if( write(1, lol, 1) < 1) {
										fprintf(stderr, "error in writing %s\n", strerror(errno));
										exit(1);
									}

									// socketBuffer[bufferCounter] = lol[0];
									// bufferCounter++;

									if(encryptionEnabled){
										mcrypt_generic (td1, lol, 1);
									}

									if( write(sockfd, lol, 1) < 0) {

										fprintf(stderr, "Couldn't write to child pipe %s\n", strerror(errno));
										exit(1);

									}


									socketBuffer[bufferCounter] = lol[0];
									bufferCounter++;




								}



							}

							// if(encryptionEnabled){
							// 	mcrypt_generic(td1, socketBuffer, bufferSize);
							// }
							//
							// if( write(sockfd, socketBuffer, bufferSize) < 0) {
							//
							// 	fprintf(stderr, "Couldn't write socket %s\n", strerror(errno));
							// 	exit(1);
							//
							// }
							//



							if(loggingEnabled){
								//i is number of bytes sent
								//s is actual message sent
								char tempBuf[30];

								int h = sprintf(tempBuf, "SENT %d BYTES: ", i);

								if(write(logFile, tempBuf, h) != h){
									fprintf(stderr, "Error in writing to log file!\n");
									exit(1);
								}

								if(write(logFile, socketBuffer, bufferCounter) != bufferCounter)
								{
									fprintf(stderr, "Error in writing to log file!\n");
									exit(1);
								}

								char poloBuf[1] = "\n";

								if(write(logFile, poloBuf, 1) != 1)
								{
									fprintf(stderr, "Error in writing to log file!\n");
									exit(1);
								}

							}

							 free(socketBuffer);

								//CHARACTER PROCESSING ENDED



						} //READING ENDED FOR PARENT


					} //STDIN DONE

					if( (pfds[0].revents & POLLERR)) {

						fprintf(stderr, "Error in polling stdin %s\n", strerror(errno));
						exit(1);

					} //STDIN ERROR DONE

					if( (pfds[1].revents & POLLERR)) {
						fprintf(stderr, "Error in polling socket %s\n", strerror(errno));
						exit(1);
					}  //PIPE ERROR DONE

					if( (pfds[1].revents & POLLHUP)) {
						// fprintf(stderr, "POLL HANGUP HAPPENED %s\n", strerror(errno));
						// exit(1);
						continuepolling = 0;
					}  //PIPE ERROR DONE


					//READING FROM SOCKET PIPE NOW
					if( (pfds[1].revents & POLLIN)) {
						if( (i = read(sockfd, s, bufferSize)) != 0)
						{


							if(i < 0)
							{
								//ERROR
								fprintf(stderr, "error in reading %s\n", strerror(errno));

								exit(1);
							}





							if(loggingEnabled){
								//i is number of bytes recieved
								//s is actual message recieved
								char tempBuf[30];

								//REMEMBER TO USE AMERICAN SPELLING OF RECIEVED
								int h = sprintf(tempBuf, "\nRECEIVED %d BYTES: ", i);

								if(write(logFile, tempBuf, h) != h){
									fprintf(stderr, "Error in writing to log file!\n");
									exit(1);
								}

								if(write(logFile, s, i) != i)
								{
									fprintf(stderr, "Error in writing to log file!\n");
									exit(1);
								}

								char poloBuf[1] = "\n";

								if(write(logFile, poloBuf, 1) != 1)
								{
									fprintf(stderr, "Error in writing to log file!\n");
									exit(1);
								}

							}



							if(encryptionEnabled){
								mdecrypt_generic (td2, s, i);
							}

							//TIME TO WRITE TO OUTPUT, MAKE SURE TO WRITE i BYTES
							int j =0;
							for(j = 0; j < i; j++){

								if(s[j] == 4 || s[j] == 3) {
									//fprintf(stderr, "THANK GOD EOF RECIEVED");
									continuepolling = 0;
									break;
								}

								else if(s[j] == 10 || s[j] == 13) {
									char *lol = strdup("  ");
									lol[0] = 13;
									lol[1] = 10;

									if( write(1, lol, 2) < 2) {
										fprintf(stderr, "error in writing to stdout %s\n", strerror(errno));
										exit(1);
									}

								}

								else	{

									char *lol = strdup(" ");
									lol[0] = s[j];

									if( write(1, lol, 1) < 1) {
										fprintf(stderr, "error in writing to stdout%s\n", strerror(errno));
										exit(1);
									}


								}



							}




								//CHARACTER PROCESSING ENDED



						} //READING ENDED FOR PARENT
						else {
							//READ GAVE 0 FOR PROCESS
							//fprintf(stderr, "EOF ACHIEVED????\n");
							continuepolling = 0;

						}

					}

				} //POLLING DONE

				free(s);


if(encryptionEnabled) {
	mcrypt_generic_end(td1);
	mcrypt_generic_end(td2);
}



	//Arguments successfully parsed if code has reached here

	return 0;
}


void sigpipeHandler(int signum)
{
	fprintf(stderr, "\nCAUGHT: SIGPIPE signal occured, but was caught.  %d\n ", signum);
	// int childStatus;
	// if(waitpid(0, &childStatus, 0)) {
	//
	// 	int signumber = childStatus & 0x007f;
	// 	int statusnumber = childStatus & 0xff;
	// 	fprintf(stderr,"SHELL EXIT SIGNAL=%d STATUS=%d",signumber, statusnumber );
	//
	//
	// 	exit(1);
	// }
	//

	//exit(1);
}
