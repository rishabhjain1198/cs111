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

struct termios saved_attributes;

void sigpipeHandler(int signum);

void reset_input_mode(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &saved_attributes);
}


int main(int argc, char **argv)
{

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


	//CODE TO PARSE ARGUMENTS
	static struct option long_options[] =
	{
		{ "shell", no_argument, 0, 's'},
        {NULL, 0, 0, 0},
	};

	int c, option_index;

	int to_child_pipe[2];
	int from_child_pipe[2];
	pid_t child_pid = -1;

	int shellActivated = 0;


	while((c = getopt_long(argc, argv, "s", long_options, &option_index)) != -1)
	{
		switch(c){

		case 's':


			shellActivated = 1;

			if( pipe(to_child_pipe) == -1) {
				fprintf(stderr, "pipe() failed! %s\n", strerror(errno));
				exit(1);
			}

			if( pipe(from_child_pipe) == -1) {
				fprintf(stderr, "pipe() failed! %s\n", strerror(errno));
				exit(1);
			}

			child_pid = fork();

			if( child_pid > 0) {
				close(to_child_pipe[0]);
				close(from_child_pipe[1]);


				int bufferSize = 512;		//edit this according to preference
				int i;
				char* s = malloc(bufferSize * sizeof(*s));
				struct pollfd pfds[2];

				pfds[0].fd = STDIN_FILENO;
				pfds[0].events = POLLIN;

				pfds[1].fd = from_child_pipe[0];
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

							//TIME TO WRITE TO OUTPUT, MAKE SURE TO WRITE i BYTES
							int j =0;
							for(j = 0; j < i; j++){

								if(s[j] == 3) {
									//fprintf(stderr, "Kill signal sent to process\n");
									kill(child_pid, SIGKILL);
									//NEED TO WRITE MORE CODE
								}

								else if(s[j] == 4) {
									// char *lol = strdup(" ");
									// lol[0] = 4;
									// if( write(to_child_pipe[1], lol, 1) < 0){
									// 	fprintf(stderr, "error in writing to pipe %s\n", strerror(errno));
									// 	exit(1);
									// }
									//fprintf(stderr, "^D Pressed");
									if(close(to_child_pipe[1]) != 0){
										fprintf(stderr, "Error in closing pipe to process\n");
										exit(1);
									}
									//exit(0);
									// exit(0);

								}

								else if(s[j] == 10 || s[j] == 13) {
									char *lol = strdup("   ");
									lol[0] = 13;
									lol[1] = 10;

									if( write(1, lol, 2) < 2) {
										fprintf(stderr, "error in writing to stdout %s\n", strerror(errno));
										exit(1);
									}

									lol[0] = 10; lol[1] = '\0';

									if( write(to_child_pipe[1], lol, 1) < 0) {
										fprintf(stderr, "error in writing to child pipe %s\n", strerror(errno));
										exit(1);
									}

								}

								else	{

									char *lol = strdup(" ");
									lol[0] = s[j];

									if( write(1, lol, 1) < 1) {
										fprintf(stderr, "error in writing %s\n", strerror(errno));
										exit(1);
									}

									if( write(to_child_pipe[1], lol, 1) < 0) {

										fprintf(stderr, "Couldn't write to child pipe %s\n", strerror(errno));
										exit(1);

									}

								}



							}

								//CHARACTER PROCESSING ENDED



						} //READING ENDED FOR PARENT


					} //STDIN DONE

					if( (pfds[0].revents & POLLERR)) {

						fprintf(stderr, "Error in polling stdin %s\n", strerror(errno));
						exit(1);

					} //STDIN ERROR DONE

					if( (pfds[1].revents & POLLERR)) {
						fprintf(stderr, "Error in polling pipe %s\n", strerror(errno));
						exit(1);
					}  //PIPE ERROR DONE

					if( (pfds[1].revents & POLLHUP)) {
						// fprintf(stderr, "POLL HANGUP HAPPENED %s\n", strerror(errno));
						// exit(1);
						continuepolling = 0;
					}  //PIPE ERROR DONE


					//READING FROM CHILD PIPE NOW
					if( (pfds[1].revents & POLLIN)) {
						if( (i = read(from_child_pipe[0], s, bufferSize)) != 0)
						{


							if(i < 0)
							{
								//ERROR
								fprintf(stderr, "error in reading %s\n", strerror(errno));

								exit(1);
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

				int childStatus;

				if(waitpid(child_pid, &childStatus, 0) == child_pid) {
				//	fprintf(stderr, "CHILD STATUS ENTIRE NUMBER:\n%d\nTHATS IT\n", childStatus);
					int signumber = childStatus & 0x007f;
					int statusnumber = childStatus >> 8;
					fprintf(stderr,"SHELL EXIT SIGNAL=%d STATUS=%d",signumber, statusnumber );


					exit(0);
				}
				else {
					fprintf(stderr, "Error in exit of shell, %s\n", strerror(errno));
					exit(1);
				}


			}//CODE ENDED FOR PARENT

			else if(child_pid == 0) {

				close(to_child_pipe[1]);
				close(from_child_pipe[0]);
				dup2(to_child_pipe[0], STDIN_FILENO);
				dup2(from_child_pipe[1], STDOUT_FILENO);
				close(to_child_pipe[0]);
				close(from_child_pipe[1]);

				//TO EXECUTE BASH SHELL

				char *execvp_argv[2];
				char execvp_filename[] = "/bin/bash";
				execvp_argv[0] = execvp_filename;
				execvp_argv[1] = NULL;

				//fprintf(stderr, "EXECUTING BASH");

				if(execvp(execvp_filename, execvp_argv) == -1) {
					fprintf(stderr, "execvp failed! %s\n", strerror(errno));
					exit(1);
				}

				//CODE FOR CHILD HERE

			}
			else {
				fprintf(stderr, "Fork failed! %s\n", strerror(errno));
				exit(1);
			}





			break;

			case '?':
				fprintf(stderr, "Unknown argument found %x\n", optopt);
				exit(1);
				break;
		}


		//DETECT BOGUS ARGUMENTS


	}




	//Arguments successfully parsed if code has reached here


	if(!shellActivated) {
	//TIME TO READ FROM INPUT AND WRITE TO OUTPUT
	int bufferSize = 512;		//edit this according to preference
	int i;
	char* s = malloc(bufferSize * sizeof(*s));

	while( (i = read(0, s, bufferSize)) != 0)
	{

		if(i < 0)
		{
			//ERROR
			fprintf(stderr, "error in reading %s\n", strerror(errno));

			exit(1);
		}

		//TIME TO WRITE TO OUTPUT, MAKE SURE TO WRITE i BYTES
		int j =0;
		for(j = 0; j < i; j++){

			if(s[j] == 3 || s[j] == 4) {
				exit(0);
			}
			else if(s[j] == 10 || s[j] == 13) {
				char *lol = strdup("   ");
				lol[0] = 13;
				lol[1] = 10;

				if( write(1, lol, 2) < 2) {
					fprintf(stderr, "error in writing %s\n", strerror(errno));
					exit(1);
				}

			}

			else	{

				char *lol = strdup(" ");
				lol[0] = s[j];

				if( write(1, lol, 1) < 1) {
					fprintf(stderr, "error in writing %s\n", strerror(errno));
					exit(1);
				}

			}



		}

			//CHARACTER PROCESSING ENDED



	}
	//READING ENDED

	free(s);
} //shell not program ended

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
