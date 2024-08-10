#include "main.h"

int main(void) {
    int fd = 0;
    FILE *fdata = NULL, *fsign = NULL;
    char *buf = NULL;
    char start[BUFFER_SIZE] = {0};
    ssize_t num = 0, data_read = 0;

    // Open output files
    if (NULL == (fdata = fopen(DATA_FILE, "w")) || NULL == (fsign = fopen(SIGNALS_FILE, "w"))) {
        perror("fopen");
        exit(1);
    }

    // Open FIFO
    if (-1 == (fd = open(NAMED_FIFO, O_RDONLY))) {
        perror("open");
        fclose(fdata);
        fclose(fsign);
        exit(1);
    }

    // Main loop to read new data and store it
    while (1) {
        // Read new data
        puts("Reading new data");
        while (1) {
            // Check if we have an error
            if (-1 == (num = read(fd, start, BUFFER_SIZE))) {
                perror("read");
                goto ret;
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

        // Store data in output files
        if (0 == strncmp(buf, DATA_HEADER, strlen(DATA_HEADER))) {
            if (-1 == fprintf(fdata, "%s", buf + strlen(DATA_HEADER))) {
                perror("fprintf");
                goto ret;
            }
        } else if (0 == strncmp(buf, SIGN_HEADER, strlen(SIGN_HEADER))) {
            if (-1 == fprintf(fsign, "%s", buf + strlen(SIGN_HEADER))) {
                perror("fprintf");
                goto ret;
            }
        }

        // Clear buffer
        memset(buf, 0, data_read);
        data_read = 0;
    }

ret:
    free(buf);
    close(fd);
    fclose(fdata);
    fclose(fsign);
    return errno ? errno : 0;
}