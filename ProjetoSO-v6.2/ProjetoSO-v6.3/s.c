#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>

// Struct representing a user request (example)
typedef struct {
    int user_id;
    int service_type; // Example: 0 = Video, 1 = Music, etc.
    int data_amount;  // Data amount for the request
} Pedido_User;

// Function for each authorization engine with select()
void authorization_engine(int read_fd) {
    fd_set read_set;
    Pedido_User request;

    while (1) {
        // Initialize the fd_set and add the read_fd to it
        FD_ZERO(&read_set);
        FD_SET(read_fd, &read_set);

        

        // Use select() to check if there's data available to read
        int ready = select(read_fd + 1, &read_set, NULL, NULL,NULL);

        if (ready < 0) {
            perror("Error during select");
            break;}
        

        // If data is ready, read from the pipe
        if (FD_ISSET(read_fd, &read_set)) {
            ssize_t bytes_read = read(read_fd, &request, sizeof(request));
            if (bytes_read < 0) {
                perror("Error reading from pipe");
                break;
            } else if (bytes_read == 0) {
                printf("Authorization Engine: No more data to read. Exiting.\n");
                break;
            }

            // Process the request (this could be a placeholder logic)
            printf("Authorization Engine received request from user %d, service type %d, data amount %d\n", 
                   request.user_id, request.service_type, request.data_amount);

            // Example processing logic (expanding as needed)
            printf("Processing request: User %d, Service %d, Data %d\n",
                   request.user_id, request.service_type, request.data_amount);

            // You can continue processing in the loop or exit, depending on your requirements
        }
    }

    close(read_fd); // Clean up
    printf("Authorization Engine: Exiting and closing file descriptor.\n");
    exit(EXIT_SUCCESS);
}

int main() {
    // Example usage of authorization_engine (e.g., using an unnamed pipe)
    int pipe_fds[2];
    if (pipe(pipe_fds) == -1) {
        perror("Error creating pipe");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Error during fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) { // Child process (authorization engine)
        close(pipe_fds[1]); // Close the write end
        authorization_engine(pipe_fds[0]); // Run the engine with the read end
    }

    // Parent process: send a test request through the pipe
    close(pipe_fds[0]); // Close the read end in parent
    Pedido_User test_request = { 1, 0, 100 }; // Example user request
    Pedido_User test_request2 = { 12, 0, 1002 }; // Example user request
    write(pipe_fds[1], &test_request, sizeof(test_request));
    write(pipe_fds[1], &test_request2, sizeof(test_request2));

    // Wait for the child process to finish
    int status;
    waitpid(pid, &status, 0);

    printf("Parent: Authorization Engine process completed.\n");

    return EXIT_SUCCESS;
}