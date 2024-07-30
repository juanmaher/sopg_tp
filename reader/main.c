#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "common.h"

#define BUFFER_SIZE 5
#define DATA_FILE "out/log.txt"
#define SIGNALS_FILE "out/signals.txt"

int main(void) {
    int fd = 0;
    FILE *fdata = NULL;
    FILE *fsign = NULL;
    char *buf = NULL;
    char start[BUFFER_SIZE] = {0};
    ssize_t num = 0, data_read = 0;

    if (NULL == (fdata = fopen(DATA_FILE, "w"))) {
        perror("fopen");
        exit(1);
    }

    if (NULL == (fsign = fopen(SIGNALS_FILE, "w"))) {
        perror("fopen");
        exit(1);
    }

    // Open FIFO
    if (-1 == (fd = open(NAMED_FIFO, O_RDONLY))) {
        perror("open");
        exit(1);
    }

    while (1) {
        // Read new data
        puts("Reading new data");
        while (1) {
            num = read(fd, start, BUFFER_SIZE);
            if (num == -1 || num == 0) {
                perror("read");
                free(buf);
                close(fd);
                fclose(fdata);
                fclose(fsign);
                exit(1);
            }
            printf("num: %ld\n", num);
            char * tmp = realloc(buf, data_read + num);
            if (NULL == tmp) {
                perror("realloc");
                free(buf);
                close(fd);
                exit(1);
            }
            buf = tmp;
            memcpy(buf + data_read, start, num);
            data_read += num;
            if (buf[data_read-1] == '\n' || buf[data_read-2] == '\n') {
                break;
            }
        }
        data_read = 0;

        // Store data
        if (strncmp(buf, "DATA:", 5) == 0) {
            if (-1 == fprintf(fdata, "%s", buf+5)) {
                perror("fprintf");
                free(buf);
                close(fd);
                fclose(fdata);
                fclose(fsign);
                exit(1);
            }
        } else if (strncmp(buf, "SIGN:", 5) == 0) {
            if (-1 == fprintf(fsign, "%s", buf+5)) {
                perror("fprintf");
                free(buf);
                close(fd);
                fclose(fdata);
                fclose(fsign);
                exit(1);
            }
        }
    }

    free(buf);
    close(fd);
    fclose(fdata);
    fclose(fsign);
    return 0;
}