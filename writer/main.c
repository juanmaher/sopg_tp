#include "main.h"

// Flag to indicate if we should send SIGUSR1 or SIGUSR2
volatile sig_atomic_t __sigusr_flag = 0;

/**
 * @brief Signal handler for SIGUSR1 and SIGUSR2
 * 
 * @param sig Signal number
 */
static void __sigusr_handler(int sig) {
    if (sig == SIGUSR1 || sig == SIGUSR2)
        __sigusr_flag = sig;
}

/**
 * @brief Send signal message
 * 
 * @param fd File descriptor
 * @param content Content of the message
 */
void __send_signal_message(int fd, const char * content) {
    // Send signal message through the FIFO
    if (-1 == write(fd, content, strlen(content) * sizeof(char))) {
        perror("write");
        exit(1);
    }
}

/**
 * @brief Send signal
 * 
 * @param fd File descriptor
 * @param sig Signal number
 */
static void __send_signal(int fd, int sig) {
    if (sig == SIGUSR1) {
        __send_signal_message(fd, SIGNAL_1_MSG);
    } else if (sig == SIGUSR2) {
        __send_signal_message(fd, SIGNAL_2_MSG);
    } else {
        fprintf(stderr, "Unknown signal: %d\n", sig);
        exit(1);
    }
}

/**
 * @brief Initialize signaling
 */
static void __init_signaling() {
    struct sigaction sa;

    sa.sa_handler = __sigusr_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (-1 == sigaction(SIGUSR1, &sa, NULL) || -1 == sigaction(SIGUSR2, &sa, NULL)) {
        perror("sigaction");
        exit(1);
    }

    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (-1 == sigaction(SIGPIPE, &sa, NULL)) {
        perror("sigaction");
        exit(1);
    }
}

static void __create_fifo() {
    struct stat st;

    // Check if FIFO exists
    if (0 != stat(NAMED_FIFO, &st)) {
        // Create FIFO
        if (0 != mkfifo(NAMED_FIFO, 0666)) {
            perror("Failed to create named FIFO");
            exit(1);
        }
    }
}

int main(void) {
    int fd = 0;
    char *buf = NULL;
    char start[BUFFER_SIZE] = {0};
    ssize_t num = 0, data_read = 0, bytes_written = 0;

    __init_signaling();

    printf("pid: %d\n", getpid());

    __create_fifo();

    puts("Waiting for readers...");

    // Open FIFO
    if (-1 == (fd = open(NAMED_FIFO, O_WRONLY))) {
        perror("open");
        unlink(NAMED_FIFO);
        exit(1);
    }

    puts("Enter text (CTRL+D to finish or ENTER to send):");

    // Allocate memory for the buffer
    if (NULL == (buf = (char *) malloc(sizeof(char) * strlen(DATA_HEADER)))) {
        perror("malloc");
        close(fd);
        unlink(NAMED_FIFO);
        exit(1);
    }

    // Main loop to read new data from stdin and send it through the FIFO
    while (1) {
        // Add header in the buffer
        memcpy(buf, DATA_HEADER, sizeof(char) * strlen(DATA_HEADER));
        data_read += sizeof(char) * strlen(DATA_HEADER);

        // Read new data
        puts("Send a new message");

        // Read new data from stdin
        while (1) {
            num = read(STDIN_FILENO, start, BUFFER_SIZE);

            // Check if we have an error
            if (-1 == num && errno != EINTR) {
                perror("read");
                goto ret;
            }

            // Check if we should send SIGUSR1 or SIGUSR2
            if (0 != __sigusr_flag) {
                __send_signal(fd, __sigusr_flag);
                __sigusr_flag = 0;
                continue;
            }

            // Check if we read EOF
            if (0 == num) {
                puts("EOF reached, exiting...");
                goto ret;
            }

            // Add new data in the buffer
            char * tmp = realloc(buf, data_read + num);
            if (NULL == tmp) {
                perror("realloc");
                goto ret;
            }
            buf = tmp;
            memcpy(buf + data_read, start, num);
            data_read += num;

            // Check if the message is finished
            if (memchr(buf, '\n', data_read)) {
                break;
            }
        }

        // Send new data through the FIFO
        bytes_written = write(fd, buf, data_read);
        if (bytes_written == -1) {
            perror("write");
            if (errno == EPIPE) {
                fprintf(stderr, "No readers on the other side of the FIFO (EPIPE)\n");
            }
            goto ret;
        }

        // Clear buffer
        data_read = 0;
    }

ret:
    free(buf);
    close(fd);
    unlink(NAMED_FIFO);
    return errno ? errno : 0;
}