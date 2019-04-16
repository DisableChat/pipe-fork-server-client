//******************************************************************************
// File:        cs_2_client.c
// Author:      Wesley Ryder
// Date:        4-3-19
// Description: To use pipes and forks to create a server/client application
//              using pipes and forks this is the client implementation
//              *note* references were made on general info about pipes from
//              online and how they work these were online forums and general
//              documentation etc
//******************************************************************************

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>

#define MSG_LENGTH 100  // maximum length of message
#define READ 0          // Read Value
#define WRITE 1         // Write Value
#define MAX_BUF 1024    // Max Buffer


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
  cmd_type_t subtype;             // Time or Status
  int status;                     // status
  int PID;                        // pid
  char message_text[MSG_LENGTH];  // mssg
} cargo_t;

//******************************************************************************
// Function:    main()
// Parameters:  None
// Description: To run the main of the server side
//******************************************************************************
int main()
{
  // Variable Declarations Needed
  char buf[MAX_BUF];
  char bufOne[MAX_BUF];
  char bufTwo[MAX_BUF];
  int fd;
  int fSend;
  int fRec;

  // Setting up fifo dir and write to server
  char *server_fifo = "/tmp/server_np";
  fd = open(server_fifo, O_WRONLY);

	// Send Init
	pid_sent_t _init;
  // Set PID
	_init.pid_num = getpid();

  // Write _init but if fail dispaly init failure
	if( write(fd, &_init, sizeof(pid_sent_t)) < 0)
  {
    fprintf(stderr, "Error: Init failure\n");
  }
  close(fd);

	// Setting up pipes
	sprintf(bufOne, "/tmp/%s_send", buf);
	char *fifoSend = bufOne;
	mkfifo(fifoSend, 0666);

	sprintf(bufTwo, "/tmp/%s_receive", buf);
	char *fifoRec = bufTwo;
	mkfifo(fifoRec, 0666);

	//read input from terminal for message requests
	while(1)
	{
		//open pipe and declare variables
		fSend = open(fifoSend, O_WRONLY);
    // Set pointer to NULL
		char *cmd = NULL;
    // Set buffer_size = to max buff
		size_t buffer_size = MAX_BUF;
    // Declare str
    char str[MSG_LENGTH];
    // Declare message one
		struct msg_request msgOne;

    // Display to user the command options
    printf("\nEnter a command:\n1) send: followed by a message\n2) exit\n3) status\n4) time\n");
    // Get input from user
		scanf("%s", str);

		// This is for the send command case
		if ( strstr(str, "Send:" ) != NULL || strstr( str, "send:" ) != NULL )
		{
			// Get the inputed Send: command string
			size_t characters = getline(&cmd, &buffer_size, stdin);
      // Removing the newline and replacing with null
			cmd[ strlen(cmd) - 1 ] = '\0';
			memmove(str, str + 1, strlen(str));
			// Set corrosposnding type and subtype
      msgOne.subtype = SEND;
			msgOne.type = COMMAND;
      msgOne.PID = getpid();

      // Copy the message txt from the msg
			strcpy(msgOne.message_text, cmd);
			// Now we write to server or print error if failure
		  if(write(fSend, &msgOne, sizeof(cargo_t)) < 0)
			{
		    fprintf(stderr, "Error: Write to server failure\n");
				close(fSend);
			}
      // Succesfull
			else{close(fSend);}
		}
    // EXIT COMMAND
    else if (strstr(str, "Exit") != NULL || strstr(str, "exit") != NULL)
    {
      // Setting corrosponding struct values
      msgOne.subtype = EXIT;
      msgOne.type = COMMAND;

      // Copy the proceding text from the mssg
      strcpy(msgOne.message_text, str);

      if(write(fSend, &msgOne, sizeof(cargo_t)) < 0)
      {
        fprintf(stderr, "Error: Write to server error my guy\n");
        close(fSend);
      }
      // Successfull so close pipeeeeee
      else
      {
        close(fSend);
      }

      exit(0);
    }
    //TIME COMMAND
    else if (strstr(str, "Time") != NULL || strstr(str, "time") != NULL)
    {
      // Set corrsponding type and subtype
      msgOne.subtype = TIME;
      msgOne.type = COMMAND;

      // Copy the string from  message
      strcpy(msgOne.message_text, str);

      //send struct to server
      if(write(fSend, &msgOne, sizeof(cargo_t)) < 0)
      {
        fprintf(stderr, "Error: Write error my dude\n");
        close(fSend);
      }
      else
      {
        close(fSend);
        //open pipe to get struct
        fRec = open(fifoRec, O_RDONLY);

        // Declation of cargo
        cargo_t ret_time;

        //read from server
        while(read(fRec, &ret_time, sizeof(cargo_t))>0)
        {
          if(ret_time.subtype == RETURN_TIME)
          {
            printf("%s\n", ret_time.message_text);
          }
          close(fRec);
        }
      }
    }
		// Status input case
		else if (strstr(str, "Status") != NULL || strstr(str, "status") != NULL)
		{
			// Set corrosponding type and sutype
      msgOne.subtype = STATUS;
			msgOne.type = COMMAND;

      // Copy the message txt from the msg
			strcpy(msgOne.message_text, str);
			//write to server
			if(write(fSend, &msgOne, sizeof(cargo_t)) < 0)
			{
		    fprintf(stderr, "Error: Write to server error my dude\n");
				close(fSend);
			}
			//get status back from server after written
			else
			{
				// Close pipe because we were succesfull
				close(fSend);
				// Open frec
				fRec = open(fifoRec, O_RDONLY);

        // Declare Return_status
        cargo_t return_stat;

        // Retrieve back from server with value corrosposnding to status value
				while(read(fRec, &return_stat, sizeof(cargo_t)) > 0)
				{
          // Display to user the status
					if(return_stat.subtype == RETURN_STRING)
					{
						printf("%s%d\n", return_stat.message_text, return_stat.status);
					}
          // close pipe
					close(fRec);
				}
			}
		}
    // This is the catch all for bad imputs
		else{printf("%s%s", str, "Input was not a valid command...\n");}
	}
    return 0;
}
