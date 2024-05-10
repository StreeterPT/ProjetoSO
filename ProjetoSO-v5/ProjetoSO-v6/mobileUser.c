#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>

// Named pipe for sending requests to the Authorization Requests Manager
#define USER_PIPE "/tmp/user_pipe"


// Log file
#define LOG_FILE "log.txt"

int create_pipe(const char *pipe_name) {
    // Create the named pipe (FIFO) if it does not exist
    if (mkfifo(pipe_name, 0666) == -1) {
        perror("Failed to create named pipe");
        return -1;
    }
    return 0;
}

// Function to write logs to both console and file
void write_log(const char *msg) {
    FILE *log_fp = fopen(LOG_FILE, "a");
    if (log_fp != NULL) {
        fprintf(log_fp, "%s\n", msg);
        fclose(log_fp);
    }
    printf("%s\n", msg);
}

// Global variables for signal handling
int exit_flag = 0;

// Signal handler for SIGINT
void sigint_handler(int sig) {
    (void)sig;  // Unused parameter
    write_log("Mobile User: SIGINT received, exiting.");
    exit_flag = 1;
}

// Mobile user function
void mobile_user(int plafond, int max_requests, int video_interval, int music_interval, int social_interval, int data_to_reserve) {

   

    // Open the named pipe for writing
    int user_pipe_fd = open(USER_PIPE, O_WRONLY);
    if (user_pipe_fd < 0) {
        write_log("Mobile User: Failed to open named pipe.");
        exit(EXIT_FAILURE);
    }

    // Create a unique identifier for this mobile user
    pid_t user_id = getpid();

    // Register the mobile user with the initial plafond
    char registration_message[64];
    sprintf(registration_message, "%d#%d\n", user_id, plafond);
    if (write(user_pipe_fd, registration_message, strlen(registration_message)) < 0) {
        write_log("Mobile User: Failed to send registration message.");
        close(user_pipe_fd);
        exit(EXIT_FAILURE);
    }
    write_log("Mobile User: Registration message sent.");


    // Main loop for sending authorization requests
    int request_count = 0;
    while (request_count < max_requests && !exit_flag) {
        // Send authorization requests for each service based on intervals
        if (request_count < max_requests) {
            char request_message[64];
            sprintf(request_message, "%d#VIDEO#%d\n", user_id, data_to_reserve);
            if (write(user_pipe_fd, request_message, strlen(request_message)) < 0) {
                write_log("Mobile User: Failed to send video authorization request.");
                break;
            }
            request_count++;
            sleep(video_interval);
        }

        if (request_count < max_requests) {
            char request_message[64];
            sprintf(request_message, "%d#MUSIC#%d\n", user_id, data_to_reserve);
            if (write(user_pipe_fd, request_message, strlen(request_message)) < 0) {
                write_log("Mobile User: Failed to send music authorization request.");
                break;
            }
            request_count++;
            sleep(music_interval);
        }

        if (request_count < max_requests) {
            char request_message[64];
            sprintf(request_message, "%d#SOCIAL#%d\n", user_id, data_to_reserve);
            if (write(user_pipe_fd, request_message, strlen(request_message)) < 0) {
                write_log("Mobile User: Failed to send social authorization request.");
                break;
            }
            request_count++;
            sleep(social_interval);
        }

        printf("%d requests enviados",request_count);
        
    }

    // Cleanup
    close(user_pipe_fd);
    write_log("Mobile User: Exiting.");
}

int main(int argc, char *argv[]) {
    if (argc != 7) {
        fprintf(stderr, "Usage: %s <initial plafond> <max requests> <video interval> <music interval> <social interval> <data to reserve>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int plafond = atoi(argv[1]);
    int max_requests = atoi(argv[2]);
    int video_interval = atoi(argv[3]);
    int music_interval = atoi(argv[4]);
    int social_interval = atoi(argv[5]);
    int data_to_reserve = atoi(argv[6]);

    mobile_user(plafond, max_requests, video_interval, music_interval, social_interval, data_to_reserve);

    return 0;
}
