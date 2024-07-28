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

int read_data(int fd, char ** buf) {
    int num = 0, data_read = 0;
    char *start = NULL;
    size_t buffer_size = BUFFER_SIZE;

    if (NULL == buf) {
        perror("null buf");
        exit(1);
    }

    start = *buf;
    while (1) {
        num = read(fd, *buf, sizeof(char) * BUFFER_SIZE);
        printf("num: %d\n", num);
        data_read += num;
        switch (num) {
            case -1:
                perror("read");
                exit(1);
            case BUFFER_SIZE:
                puts("Buffer full");
                buffer_size *= 2;
                if (NULL == (start = (void *) realloc(*buf, sizeof(char) * buffer_size))) {
                    perror("realloc");
                    exit(1);
                }
                *buf = start + (buffer_size / 2);
                break;
            default:
                return data_read;
        }
    }
    return 0;
}

int main(void) {
    int fd = 0;
    char *s = NULL;

    if (-1 == (fd = open(NAMED_FIFO, O_RDONLY))) {
        perror("open");
        exit(1);
    }

    if (NULL == (s = (void *) malloc(sizeof(char) * BUFFER_SIZE))) {
        perror("malloc");
        exit(1);
    }

    while (1) {
        if (read_data(fd, &s)) {
            puts("Read done");
            if (-1 == write(STDOUT_FILENO, s, strlen(s))) {
                perror("write");
                exit(1);
            }
            memset(s, 0, strlen(s));
        }
    }

    free(s);
    close(fd);
    return 0;
}