#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

void sigusr1_handler(int sig) {
    write(STDOUT_FILENO, "SIGUSR1\n", 8);
}

void sigusr2_handler(int sig) {
    write(STDOUT_FILENO, "SIGUSR2\n", 8);
}

int main(void) {

    struct sigaction sa;
    sa.sa_handler = sigusr1_handler;
    sa.sa_flags = 0; // SA_RESTART
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    sa.sa_handler = sigusr2_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("pid: %d\n", getpid());

    // char s[200];
    while (1) {
        // if (fgets(s, sizeof(s), stdin) == NULL) {
        //     perror("error en fgets");
        // } else {
        //     printf("ingresaste: %s\n", s);
        // }
        // int sig;
        // int r = sigwait(&set, &sig);
        // printf("r = %d, sig = %d\n", r, sig);
    }

    return 0;
}