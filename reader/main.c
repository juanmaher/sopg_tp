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
#define DATA_FILE "out/log.txt"
#define SIGNALS_FILE "out/signals.txt"

int main(void) {
    int fd = 0;
    FILE *fdata = NULL;
    FILE *fsign = NULL;
    char *s = NULL, *start = NULL;
    ssize_t num = 0, data_read = 0;
    ssize_t buffer_size = BUFFER_SIZE;

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

    // Allocate memory
    if (NULL == (s = (void *) malloc(sizeof(char) * BUFFER_SIZE))) {
        perror("malloc");
        exit(1);
    }

    start = s;

    while (1) {
        // Read new data
        while (1) {
            num = read(fd, s + data_read, buffer_size);
            if (-1 == num) {
                perror("read");
                free(s);
                close(fd);
                fclose(fdata);
                fclose(fsign);
                exit(1);
            }

            // EOF
            if (0 == num) {
                free(s);
                close(fd);
                fclose(fdata);
                fclose(fsign);
                return 0;
            }

            data_read += num;

            if (num < buffer_size)
                break;

            buffer_size *= 2;
            if (NULL == (start = (char *) realloc(start, buffer_size))) {
                perror("realloc");
                exit(1);
            }
            s = start;
        }

        // Store data
        if (strncmp(s, "DATA:", 5) == 0) {
            if (-1 == fprintf(fdata, "%s", s)) {
                perror("fprintf");
                free(s);
                close(fd);
                fclose(fdata);
                fclose(fsign);
                exit(1);
            }
        } else if (strncmp(s, "SIGN:", 5) == 0) {
            if (-1 == fprintf(fsign, "%s", s)) {
                perror("fprintf");
                free(s);
                close(fd);
                fclose(fdata);
                fclose(fsign);
                exit(1);
            }
        }

        // Reset buffer
        memset(start, 0, buffer_size);
        s = start;
        data_read = 0;
    }

    free(s);
    close(fd);
    fclose(fdata);
    fclose(fsign);
    return 0;
}