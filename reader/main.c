#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "common.h"

#define BUFFER_SIZE 1024

int main(void) {
    int fd, num;
    char buf[BUFFER_SIZE];

    if (-1 == (fd = open(NAMED_FIFO, O_RDONLY))) {
        perror("open");
        exit(1);
    }

    while (1) {
        if (-1 == (num = read(fd, buf, sizeof(BUFFER_SIZE)))) {
            perror("read");
            exit(1);
        }
        buf[num] = '\0';
        write(STDIN_FILENO, buf, num);
    }

    close(fd);
    return 0;
}