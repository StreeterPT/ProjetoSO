#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>

// Named pipes definitions
#define USER_PIPE "/tmp/user_pipe"
#define BACK_PIPE "/tmp/back_pipe"

// Function to write messages to a named pipe
void write_to_pipe(const char *pipe_name, const char *message) {
    int pipe_fd = open(pipe_name, O_WRONLY);  // Open the pipe for writing
    if (pipe_fd == -1) {
        perror("Error opening pipe");
        exit(EXIT_FAILURE);
    }

    size_t message_length = strlen(message);
    ssize_t bytes_written = write(pipe_fd, message, message_length);

    if (bytes_written == -1) {
        perror("Error writing to pipe");
    } else {
        printf("Message written to %s: %s\n", pipe_name, message);
    }

    close(pipe_fd);  // Close the pipe
}

int main() {
    // Ensure the pipes exist (create them if they don't)
    if (mkfifo(USER_PIPE, 0666) == -1 && errno != EEXIST) {
        perror("Error creating USER_PIPE");
        exit(EXIT_FAILURE);
    }

    if (mkfifo(BACK_PIPE, 0666) == -1 && errno != EEXIST) {
        perror("Error creating BACK_PIPE");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();  // Create a child process
    if (pid == -1) {
        perror("Error during fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  // Child process
        const char *child_messages_user[] = {
            "12#VIDEO#100\n",
            "10#20\n"
        };

        const char *child_messages_back[] = {
            "1#reset\n",
            "1#data_stats\n"
        };

        // Send messages to USER_PIPE
        for (int i = 0; i < 2; i++) {
            write_to_pipe(USER_PIPE, child_messages_user[i]);
            sleep(1);  // Delay for demonstration purposes
        }

        // Send messages to BACK_PIPE
        for (int i = 0; i < 2; i++) {
            write_to_pipe(BACK_PIPE, child_messages_back[i]);
            sleep(1);  // Delay for demonstration purposes
        }

        exit(0);  // Child exits after sending messages
    } else {  

        // Wait for the child process to complete
        wait(NULL);

        printf("Parent process finished.\n");
    }

    return 0;  // Success
}
