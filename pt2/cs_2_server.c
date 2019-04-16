//******************************************************************************
// File:        cs_2_server.c
// Author:      Wesley Ryder
// Date:        4-10-19
// Description: To use pipes and forks to create a server/client application
//              using pipes and forks this is the server implementation
//              *note* references were made on general info about pipes from
//              online and how they work these were online forums and general
//              documentation etc
//******************************************************************************

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>


int processes = 0;      // Child processess
#define MSG_LENGTH 100  // maximum length of message
#define READ 0          // Read Value
#define WRITE 1         // Write Value
#define MAX_BUF 1024    // Max buffer


// Type Enum as described in lecture
typedef enum {
  SEND,
  EXIT,
  STATUS,
  TIME,
  RETURN_STRING,
  RETURN_TIME
} cmd_type_t;

// Enum struct from lecture
typedef enum {
  REGULAR,
  COMMAND
} msg_type_t;

// Init Struct
typedef struct msg_init {
  int pid_num;
} pid_sent_t;

// Child Struct
typedef struct msg_request {
  msg_type_t type;                // Command
  cmd_type_t subtype;             // Time or status
  int status;                     // status
  int PID;                        // PID;
  char message_text[MSG_LENGTH];  // msg
} cargo_t;

//******************************************************************************
// Function:    unLink()
// Parameters:  int s
// Description: To unlink when user enters a control c command so we have clean
//              exit for server
//******************************************************************************
void unLink(int s)
{
  char *server_fifo = "tmp/server_np";
  unlink(server_fifo);
  exit(0);
}

//******************************************************************************
// Function:    handleD
// Parameters:  int s
// Description: If ctr+c is entered handle
//******************************************************************************
void handleD(int s)
{
	pid_t pid;
	pid = wait(NULL);
}


//******************************************************************************
// Function:    main()
// Parameters:  None
// Description: To run the main of the server side
//******************************************************************************
int main()
{
  //initialization of pipe and buffers
  char *server_fifo = "/tmp/server_np";
  char buf[MAX_BUF];
	char PID_sent[MAX_BUF];   //Pid sent char array
  int ret_f;                // ret_f
  int fd;                   //File Dir
  int fSend;                //fSend
  int fRec;                 //fRec

  // implemented for Ctr C case so we have clean exit
  signal(SIGCHLD, handleD);
  signal(SIGINT, unLink);

	int server_pid = getppid();
  mkfifo(server_fifo, 0666);

  // While true and pid == server pid run loop iniftitly
  while(1 && getppid() == server_pid)
  {
	  // check for clients sending info
    fd = open(server_fifo, O_RDONLY);
		pid_sent_t _init;

		 //read pid sent by client
    while(read(fd, &_init, sizeof(pid_sent_t)) > 0)
    {
      processes++;
      printf("Recieved: %i\n", _init.pid_num);
			sprintf(PID_sent, "%i", _init.pid_num);
			ret_f = fork();
    }

    // If the for is the child then do the proceeding block
		if(ret_f == 0)
		{
      // Declare Buffer arrays for server
      char bufOne[MAX_BUF];
      char bufTwo[MAX_BUF];


			sprintf(bufOne, "/tmp/%s_send", buf);
			char *fifoSend = bufOne;

			sprintf(bufTwo, "/tmp/%s_receive", buf);
			char *fifoRec = bufTwo;

        // infinite loop
		    while(1)
		    {
          // Declaring mesgOne
          struct msg_request msgOne;

          // check for client sending
          fSend = open(fifoSend, O_RDONLY);

				//read struct sent by client
		    while(read(fSend, &msgOne, sizeof(cargo_t)) > 0)
        {

          // Commands same idea as last time in terms of matching the only difference
          // now being that we have to keep opening and closing pipes while also
          // giving the structs corrpsonding values for the structs variables
          if (msgOne.type == COMMAND)
          {
            // SEND case
            // Displaying the pid from server and client and the msg passed and reiveid
            // along with their corrospoinding pids
            if (msgOne.subtype == SEND)
            {
              printf("server (PID: %d) Recieved string %s from client (PID: %d)\n",
                        getpid(),
                        msgOne.message_text,
                        msgOne.PID);
            }
            // EXIT case
            // This is the same as part one implementation idea
            else if (msgOne.subtype == EXIT)
            {
              // deincremnt the count of cleints open
              // and then rpint the reiveid msg and the pid from the client that
              // is being closed
              processes--;
              printf("Recieved: %s from client (PID: %d), client now exiting...\n",
                    msgOne.message_text,
                    msgOne.PID);
              _exit(0);

            }
            // STATUS CASE
            else if(msgOne.subtype == STATUS)
						{
              // close pipe
							close(fSend);
		          // Open
							fRec = open(fifoRec, O_WRONLY);

              // Declaring struct return_stat
							cargo_t return_stat;

              // Setting the corropsonding values of status and subtype
              return_stat.status = processes;
							return_stat.subtype = RETURN_STRING;

              // Coping the mssg to display to user the content
						  strcpy(return_stat.message_text, "Number of processes currently running: ");

              // Now we write back to client and displaying error if fail
						  if( write(fRec, &return_stat, sizeof(cargo_t)) < 0)
              {
                fprintf(stderr, "Error: write to client error\n");
              }
							else
              {
                close(fRec);
              }
						}
						// Time Case
						else if (msgOne.subtype == TIME)
						{
              // close
							close(fSend);

              // General getting time application found on standard
              // forum post. Pretty common found on StackOVerflow.
              // get_time_of day guide
              /////////////////////////////////////////////////////////////////////////
							time_t tmp_time;
							struct tm *tmp_local_time;
							tmp_time = time (NULL);
							tmp_local_time = localtime (&tmp_time);
              /////////////////////////////////////////////////////////////////////////

              // Opening pipe
							fRec = open(fifoRec, O_WRONLY);

              // Declare the time to be set for value return
							cargo_t time_val;
              // Setting time for that said struct
							time_val.subtype = RETURN_TIME;

              // General formating for displaying time to user
              strftime (time_val.message_text, 150, "Current time: [%I:%M] %p.",
                          tmp_local_time);

              // Write to client, if error display error
						  if( write(fRec, &time_val, sizeof(cargo_t)) < 0)
              {
                fprintf(stderr, "Error: Write to client error my dude\n");
              }
							else
              {
                close(fRec);
              }
						}
					}

        }
        close(fSend);
      }
			return(0);
    }
    // Else we continue and the fork of the parent will return
    else
    {
      continue;
    }
      // close that pipeee
      close(fd);
  }
  unlink(server_fifo);
  return 0;
}
