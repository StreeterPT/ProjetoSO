#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h> // for exit
#include <fcntl.h>

int main() {
    // Create a named semaphore
    sem_t *sem = sem_open("/my_semaphore2", O_CREAT, 0644, 5); // Initialized with 5
    
    // Create an unnamed pipe
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Fork a child process
    pid_t pid = fork();
    
    if (pid < 0) {
        // Error occurred
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process (Son)
        close(pipe_fd[1]); // Close the write end in the child process
        
        while(1) {
            // Wait for the message from the parent process
            int message; // Variable to store the received message
        
            // Read the message from the pipe
            read(pipe_fd[0], &message, sizeof(int));
            printf("Son: Received message from father: %d\n", message);

            
            sleep(5); // Simulate processing time
            
            // Signal the parent process that processing is done
            sem_post(sem);
        }
        close(pipe_fd[0]); // Close the read end in the child process
    } else {
        // Parent process (Father)
        close(pipe_fd[0]); // Close the read end in the parent process
        
        int i = 0;
        while(1){
            sem_wait(sem);
            // Send the message to the son process
            printf("Father: Sending message %d to son...\n", i);
            write(pipe_fd[1], &i, sizeof(int));
            
            // Increment message counter
            i++;
            
            // Wait for the son process to finish processing
            
        }
        close(pipe_fd[1]); // Close the write end in the parent process
    }

    // Close and unlink the semaphore when done
    sem_close(sem);
    sem_unlink("/my_semaphore2");

    return 0;
}