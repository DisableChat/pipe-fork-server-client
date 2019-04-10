#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_BUF 1024

int main()
{
    int fd;
    char * myfifo = "/tmp/myserverfifo";
    char buf[MAX_BUF];

    /* create the FIFO (named pipe) */
    mkfifo(myfifo, 0666);


    /* open, read, and display the message from the FIFO */
    while(1)
    {
        fd = open(myfifo, O_RDONLY);

        while(read(fd, buf, MAX_BUF)>0)
        {
            printf("Received: %s\n", buf);
        }
        
        close(fd);
    }

    /* remove the FIFO */
    unlink(myfifo);

    return 0;
}