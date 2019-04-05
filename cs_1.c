//******************************************************************************
// File:        cs_1.c
// Author:      Wesley Ryder
// Date:        4-3-19
// Description: To use pipes and forks to create a server/client application
//              which in this case is strictly for local use and in this
//              specific example only tests the use of pipe and fork for
//              displaying the mssg sent fromthe client back to the server
//              on the same terminal
//******************************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#define MSG_LENGTH 100  // maximum length of message
#define READ 0          // Read Value
#define WRITE 1         // Write Value

// Enum struct
typedef enum {
  REGULAR,
  COMMAND
}msg_type_t;

// Msg struct
typedef struct msg {
  msg_type_t type;
  char message_text[MSG_LENGTH];
}msg_t;

int main()
{
  // client pid value
  pid_t clientPID;
  // buffer
  char buffer[MSG_LENGTH];
  // declaring msg_t var
  msg_t msg;

  // Pipe[0] corrosponds to read and pipe[1] corrosponds to write
  int p[2];

  // Create pipe and error if failure
  if(pipe(p) == -1)
  {
    perror("Error: Could not create pipe");
    exit(1);
  }

  // Create client fork and error if failure
  if((clientPID = fork()) == -1)
  {
    perror("Error: Could not fork");
    exit(1);
  }

  // If client is read value then loop
  else if(clientPID == READ)
  {
    // The follwing is clients implmentation
    while(1)
    {
      // Display to user the command options
      printf("\nEnter a command:\n1) send: followed by a message\n2) exit\n");

      //cearing old memory of char array
      memset(msg.message_text, 0, sizeof(msg));

      // read in the new msg
      read(READ,(char*) &msg.message_text, MSG_LENGTH);

      // If instruction is a send command set the required type and message
      // along with it then sleep
      if(msg.message_text[0] == 's' &&
            msg.message_text[1] == 'e' &&
            msg.message_text[2] == 'n' &&
            msg.message_text[3] == 'd' &&
            msg.message_text[4] == ':')
      {
        msg.type = REGULAR;
        msg.message_text[strlen(msg.message_text)-1] = '\0';
        write(p[WRITE],(char*)&msg, sizeof(msg));
        close(p[READ]);
        sleep(.1);
      }
      // If instruction is a exit command set the required type and message
      // and then exit that process
      else if(msg.message_text[0] == 'e' &&
            msg.message_text[1] == 'x' &&
            msg.message_text[2] == 'i' &&
            msg.message_text[3] == 't')
      {
        msg.type = COMMAND;
        write(p[WRITE],&msg, sizeof(msg_t));
        exit(0);
      }
      // Error Non recognized command
      else
      {
        printf("\nError: entered uknown command");
      }
    }
  }
  // Server
  else
  {
    // The server just loops, reciving messages from the client untill it
    // recieves an exit command in which case it will then exit
    while(1)
    {
      // Read the buffer from client
      read(p[READ], &msg, sizeof(msg_t));
      if(msg.type == COMMAND)
      {
        // Format string
        sprintf(buffer,"server (PID:%d) exiting\n",getpid());
        // Write
        write(WRITE, &buffer, strlen(buffer));
        exit(0);
      }
      else
      {
          // Format string
          sprintf(buffer,"server (PID:%d): received string %s from client (PID:%d)\n",
              getpid(),
              msg.message_text,
              clientPID);
          // Write
          write(WRITE,(char*) &buffer, strlen(buffer));
          fflush(stdout);
          sleep(.1);
      }
    }
  }
  return 0;
}
