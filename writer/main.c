#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#define BUFFER_SIZE 1024

void sigusr1_handler(int sig) {
    write(STDOUT_FILENO, "SIGUSR1\n", 8);
}

void sigusr2_handler(int sig) {
    write(STDOUT_FILENO, "SIGUSR2\n", 8);
}

int main(void) {
    char *s, *start;
    size_t buffer_size = BUFFER_SIZE;
    struct sigaction sa;

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
                    *s++ = '\0';
                    write(STDOUT_FILENO, start, s - start);
                    memset(start, 0, buffer_size);
                    s = start; // Reset s to the start of the buffer
                    break;
                case EOF:
                    free(start);
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

    return 0;
}