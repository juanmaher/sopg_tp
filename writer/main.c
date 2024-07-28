#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"

#define BUFFER_SIZE 1024

static void sigusr1_handler(int sig);
static void sigusr2_handler(int sig);

volatile sig_atomic_t fifo_opened = FALSE;

static void sigusr1_handler(int sig) {
    if (TRUE == fifo_opened) {
        if (-1 == write(STDOUT_FILENO, "SIGUSR1\n", 8)) {
            perror("write");
            exit(1);
        }
    }
}

static void sigusr2_handler(int sig) {
    if (TRUE == fifo_opened) {
        if (-1 == write(STDOUT_FILENO, "SIGUSR2\n", 8)) {
            perror("write");
            exit(1);
        }
    }
}

int main(void) {
    int fd = 0;
    char *s = NULL, *start = NULL;
    size_t buffer_size = BUFFER_SIZE;
    struct sigaction sa;
    struct stat st;

    sa.sa_handler = sigusr1_handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);

    if (-1 == sigaction(SIGUSR1, &sa, NULL)) {
        perror("sigaction");
        exit(1);
    }

    sa.sa_handler = sigusr2_handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);

    if (-1 == sigaction(SIGUSR2, &sa, NULL)) {
        perror("sigaction");
        exit(1);
    }

    printf("pid: %d\n", getpid());

    if (0 == stat(NAMED_FIFO, &st)) {
        if (S_ISFIFO(st.st_mode)) {
            printf("Named FIFO '%s' already exists.\n", NAMED_FIFO);
        } else {
            printf("A file named '%s' exists but it is not a FIFO.\n", NAMED_FIFO);
            exit(1);
        }
    } else {
        if (errno == ENOENT) {
            if (0 == mkfifo(NAMED_FIFO, 0666)) {
                printf("Named FIFO '%s' created successfully.\n", NAMED_FIFO);
            } else {
                perror("Failed to create named FIFO");
                exit(1);
            }
        } else {
            perror("stat");
            exit(1);
        }
    }

    puts("Waiting for readers...");
    fd = open(NAMED_FIFO, O_WRONLY);
    puts("Enter text (CTRL+D to finish):");
    fifo_opened = TRUE;

    if (NULL == (s = (char *) malloc(sizeof(char) * buffer_size))) {
        perror("malloc");
        exit(1);
    }

    while (1) {
        start = s;
        while (1) {
            *s = (char) fgetc(stdin);
            switch (*s) {
                case '\n':
                    s++;
                    *s = '\0';
                    if (-1 == write(fd, start, s - start)) {
                        perror("write");
                        exit(1);
                    }
                    memset(start, 0, buffer_size);
                    s = start; // Reset s to the start of the buffer
                    break;
                case EOF:
                    free(start);
                    close(fd);
                    unlink(NAMED_FIFO);
                    return 0;
                default:
                    s++;
                    if (s - start >= buffer_size - 1) { // Reallocate if needed
                        buffer_size *= 2;
                        if (NULL == (start = (char *) realloc(start, buffer_size))) {
                            perror("realloc");
                            exit(1);
                        }
                        s = start + (buffer_size / 2); // Adjust s to the new position
                    }
                    continue;
            }
            break;
        }
    }

    free(start);
    close(fd);
    unlink(NAMED_FIFO);

    return 0;
}