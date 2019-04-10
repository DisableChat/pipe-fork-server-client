#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#define MAX_BUF 1024

int main()
{
    int fd, ret;
    char buf[MAX_BUF];
    char * myfifo = "/tmp/myserverfifo";

    /* write PID to the FIFO */
    fd = open(myfifo, O_WRONLY);
    sprintf(buf, "%d", getpid());
    if( write(fd, buf, sizeof(buf)) < 0)
        fprintf(stderr, "Error\n");
    close(fd);

    return 0;
}