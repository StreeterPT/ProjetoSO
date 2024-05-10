#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

// Named pipes paths
#define USER_PIPE "/tmp/user_pipe"
#define BACK_PIPE "/tmp/back_pipe"

// Function to drain and print contents of a named pipe
void drain_and_print_pipe(const char *pipe_path) {
    char buffer[1024]; // Buffer for reading data
    int fd = open(pipe_path, O_RDONLY | O_NONBLOCK); // Open in non-blocking mode
    if (fd == -1) {
        perror("Error opening named pipe for draining");
        return;
    }

    printf("Draining data from pipe: %s\n", pipe_path);

    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0'; // Null-terminate the buffer to safely print it
        printf("Read from pipe: %s\n", buffer); // Print the contents read from the pipe
    }

    if (bytes_read == -1 && errno != EAGAIN) { // EAGAIN is expected in non-blocking mode when there's no more data
        perror("Error reading from pipe");
    }

    close(fd); // Close the file descriptor
}

int main() {
    // Drain and print the contents of both pipes
    drain_and_print_pipe(USER_PIPE); 
    drain_and_print_pipe(BACK_PIPE);

    printf("Draining complete.\n");
    return 0;
}