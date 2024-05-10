#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// Define a simple structure to send through a pipe
typedef struct {
    int UserID;
    int Tipo;
    int QuantDados;
} Pedido_User;

// Function to read from a pipe without closing the other file descriptor
void read_from_pipe(int read_fd) {
    Pedido_User request;
    ssize_t bytes_read;

    // Keep reading from the pipe until you detect EOF or other termination condition
    while (1) {
        bytes_read = read(read_fd, &request, sizeof(request));
        
        if (bytes_read == -1) {  // Handle read error
            perror("Error reading from pipe");
            exit(EXIT_FAILURE);
        } else if (bytes_read == 0) {  // EOF (if write-end is closed)
            printf("Pipe is closed or empty.\n");
            break;  // Exit the loop
        } else if (bytes_read < sizeof(request)) {  // Incomplete read
            fprintf(stderr, "Incomplete data received.\n");
            continue;  // Continue reading
        }
        
        // Process the data read from the pipe
        printf("Received request: UserID = %d, Tipo = %d, QuantDados = %d\n",
               request.UserID, request.Tipo, request.QuantDados);
    }
}

int main() {
    int pipe_fd[2];  // Array to hold pipe file descriptors
    
    // Create a pipe
    if (pipe(pipe_fd) == -1) {
        perror("Error creating pipe");
        return EXIT_FAILURE;
    }

    // Fork a child process to write to the pipe
    pid_t pid = fork();
    if (pid == -1) {
        perror("Error during fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {  // Child process (writer)
        for(int i=0;i<5;i++){
        Pedido_User request = {i, i+3, i+100};  // Create a sample request
        write(pipe_fd[1], &request, sizeof(request));  // Write to the pipe
    }
        
        // Keep the write-end open (no explicit close)
        
        exit(EXIT_SUCCESS);  // Exit child process
    } else {  // Parent process (reader)
        read_from_pipe(pipe_fd[0]);  // Read from the read-end
        
        wait(NULL);  // Wait for the child process to finish
        
        close(pipe_fd[1]);  // Now close the write-end
        close(pipe_fd[0]);  // Close the read-end
    }

    return 0;  // Success
}