// autores:
// David Cameijo Pinheiro 2021246865
// Guilherme Fernandes Figueiredo 2021236787

#include "dados.h"


#define USER_PIPE "/tmp/user_pipe"
#define BACK_PIPE "/tmp/back_pipe"

#define BUFFER_SIZE 124 

sem_t *log_sem; // Semaphore for synchronizing access to the log file

int count_char(const char *str, char ch) {
    int count = 0;
    while (*str) {
        if (*str == ch) {
            count++;
        }
        str++;
    }
    return count;
}

void clear_file(const char *filename) {
    FILE *file = fopen(filename, "w"); // Open file in write mode
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    fclose(file); // Close the file after truncating or creating
}

void write_log(char *s){

    if (sem_wait(log_sem) == -1) {
        perror("Error waiting on semaphore");
        exit(EXIT_FAILURE);
    }

	FILE* file = fopen("log.txt", "a"); 
    if (file == NULL) {
        perror("Erro ao abrir o ficheiro");
        exit(1);
    }
    
    time_t now;
    struct tm *local;
    time(&now);

    // Set the timezone to Portugal/Lisbon
    setenv("TZ", "Europe/Lisbon", 1); // Set to Portugal/Lisbon timezone
    tzset(); // Update timezone information


    local = localtime(&now);
    
    fprintf(file,"%02d:%02d:%02d %s\n", local->tm_hour, local->tm_min, local->tm_sec,s);
   
    
	printf("%02d:%02d:%02d %s\n", local->tm_hour, local->tm_min, local->tm_sec,s);
	
    fclose(file);

    if (sem_post(log_sem) == -1) {
        perror("Error releasing semaphore");
        exit(EXIT_FAILURE);
    }
   
}

void erro(char *msg) {
    perror( msg);
    write_log(msg);
    exit(1);
}

Configuration read_config(const char* filename) {
    Configuration config;

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        erro( "Erro ao abrir o arquivo de configuracao.\n");
    }

    fscanf(file, "%d\n", &config.mobile_users);
    if(config.mobile_users < 1){
        erro("Mobile users inferior a 1.\n");
    }
    fscanf(file, "%d\n", &config.queue_pos);
    if(config.queue_pos < 0){
        erro("queue_pos inferior a 0.\n");
    }
    fscanf(file, "%d\n", &config.auth_servers);
    if(config.auth_servers < 1){
        erro("auth_servers inferior a 1.\n");
    }
    fscanf(file, "%d\n", &config.auth_proc_time);
    if(config.auth_proc_time <= 0){
        erro("auth_proc_time inferior ou igual a 0.\n");
    }
    fscanf(file, "%d\n", &config.max_video_wait);
    if(config.max_video_wait <= 0){
        erro("max_video_wait inferior a 0.\n");
    }
    fscanf(file, "%d\n", &config.max_others_wait);
    if(config.max_others_wait <= 0){
        erro("max_others_wait inferior ou igual a 0.\n");
    }

    fclose(file);

    return config;
}


// Process commands based on their source
void process_user_command(const char *command) {
    printf("Processing user command: %s\n", command);
    if (count_char('#',command) == 1){

    }

    // Additional logic for user commands
}

void process_backoffice_command(const char *command) {
    printf("Processing backoffice command: %s\n", command);
    // Additional logic for backoffice commands
}

void monitor_engine() {
    pid_t monitor_process;
    monitor_process = fork();
    if (monitor_process == -1) {
        erro("Erro ao criar processo do Monitor Engine");
        exit(EXIT_FAILURE);
    }
    if (monitor_process == 0) {
        // Código do processo filho para o Monitor Engine
        write_log("PROCESS MONITOR ENGINE CREATED!.\n");
        // Aqui será implementada a lógica para monitorar o plafond de dados dos Mobile Users
        // e gerar alertas ou estatísticas conforme necessário
        exit(1);
    }
    waitpid(monitor_process,NULL,0);

}


void* sender_f(void* arg) {
    // Implement thread 1 logic here
    write_log("THREAD SENDER CREATED!\n");
    return NULL;
}

void* receiver_f(void* arg) {
    // Implement thread 1receiver logic here
    write_log("THREAD RECEIVER CREATED!\n");

    int user_pipe_fd = open(USER_PIPE, O_RDONLY | O_NONBLOCK); // Open in non-blocking mode
    int back_pipe_fd = open(BACK_PIPE, O_RDONLY | O_NONBLOCK);

    if (user_pipe_fd == -1 || back_pipe_fd == -1) {
        perror("Error opening named pipes");
        return NULL;  // Exit if pipes can't be opened
    }

    fd_set read_fds;  // File descriptor set
    char buffer[1024];  // Buffer for reading data
    int buffer_pos = 0;  // Position in the buffer
    ssize_t bytes_read;  // Number of bytes read

    while (1) {  // Main loop
        FD_ZERO(&read_fds);  // Clear the set
        FD_SET(user_pipe_fd, &read_fds);  // Add USER_PIPE to the set
        FD_SET(back_pipe_fd, &read_fds);  // Add BACK_PIPE to the set

        int max_fd = (user_pipe_fd > back_pipe_fd) ? user_pipe_fd : back_pipe_fd;

        // Use select() to wait for data from either pipe
        int retval = select(max_fd + 1, &read_fds, NULL, NULL, NULL);  // Block until data arrives
        if (retval == -1) {
            perror("Error in select()");
            break;
        }

        // Check if there's data from USER_PIPE
        if (FD_ISSET(user_pipe_fd, &read_fds)) {
            bytes_read = read(user_pipe_fd, buffer + buffer_pos, sizeof(buffer) - buffer_pos - 1);  // Read data into the buffer
            if (bytes_read > 0) {
                buffer_pos += bytes_read;
                buffer[buffer_pos] = '\0';  // Null-terminate the buffer

                // Process complete messages separated by '\n'
                char *message_start = buffer;
                char *newline;
                while ((newline = strchr(message_start, '\n')) != NULL) {
                    *newline = '\0';  // Terminate the message
                    printf("Received from USER_PIPE: %s\n", message_start);  // Handle the message
                    int count=count_char(message_start,"#");
                    printf("\nnumero de #:%d \n",count);
                    message_start = newline + 1;  // Move to the next message
                }

                // Move remaining unprocessed data to the beginning of the buffer
                buffer_pos = strlen(message_start);
                memmove(buffer, message_start, buffer_pos);
            }
        }

        // Check if there's data from BACK_PIPE
        if (FD_ISSET(back_pipe_fd, &read_fds)) {
            bytes_read = read(back_pipe_fd, buffer + buffer_pos, sizeof(buffer) - buffer_pos - 1);  // Read data into the buffer
            if (bytes_read > 0) {
                buffer_pos += bytes_read;
                buffer[buffer_pos] = '\0';

                // Process complete messages separated by '\n'
                char *message_start = buffer;
                char *newline;
                while ((newline = strchr(message_start, '\n')) != NULL) {
                    *newline = '\0';  // Terminate the message
                    printf("Received from BACK_PIPE: %s\n", message_start);  // Handle the message
                    message_start = newline + 1;  // Move to the next message
                }

                // Move unprocessed data to the beginning of the buffer
                buffer_pos = strlen(message_start);
                memmove(buffer, message_start, buffer_pos);
            }
        }
    }

    // Cleanup
    close(user_pipe_fd);
    close(back_pipe_fd);

    return NULL;
}

// Function to create and attach to shared memory
UserData *create_sm(int *shmid, int num_users) {
    // Create a shared memory segment
    *shmid = shmget(IPC_PRIVATE, sizeof(UserData) * num_users, IPC_CREAT | 0777);
    if (*shmid == -1) {
        perror("shmget failed");
        return NULL;
    }

    printf("Shared memory segment created with ID: %d\n", *shmid);

    // Attach the shared memory segment to the process's address space
    UserData *shared_data = (UserData *)shmat(*shmid, NULL, 0);
    if (shared_data == (void *)-1) {
        perror("shmat failed");
        return NULL;
    }

    printf("Shared memory segment attached at address: %p\n", (void *)shared_data);

    return shared_data;
}


void auth_requests_manager(Configuration config) {
    pid_t auth_manager_process;
    pthread_t receiver, sender;
    int res1, res2;
    int max_size = config.queue_pos;
    auth_manager_process = fork();
    if (auth_manager_process == -1) {
        erro("ERROR CREATING PROCESS AUTHORIZATION REQUESTS MANAGER.\n");
        exit(EXIT_FAILURE);
    }
    if (auth_manager_process == 0) {
        // Child process code for Authorization Requests Manager
        write_log("PROCESS AUTHORIZATION REQUESTS MANAGER CREATED.\n");

        Queue video_streaming_queue;
        Queue other_services_queue;

        

        // Initialize the queues
        if (init_queue(&video_streaming_queue, max_size) != 0) {
        fprintf(stderr, "Failed to initialize video streaming queue.\n");
        return -1;
        }

        if (init_queue(&other_services_queue, max_size) != 0) {
        fprintf(stderr, "Failed to initialize other services queue.\n");
        return -1;
        }

        write_log("QUEUE VIDEO STREAMING CREATED!\n");
        write_log("QUEUE OTHER SERVICES CREATED!\n");

        // Tentar criar USER_PIPE
        if (mkfifo(USER_PIPE, 0666) == -1) {
            if (errno != EEXIST) { // Se o erro não for por já existir
                perror("Error creating USER_PIPE");
                exit(EXIT_FAILURE);
                }
            else{write_log("USER_PIPE already exists!\n");}
        } else {
            write_log("USER_PIPE created.\n");
        }   

        // Tentar criar BACK_PIPE
        if (mkfifo(BACK_PIPE, 0666) == -1) {
            if (errno != EEXIST) { // Se o erro não for por já existir
                perror("Error creating BACK_PIPE");
                exit(EXIT_FAILURE);
                }
            else{write_log("BACK_PIPE ALREADY EXISTS!!\n");}
            }
        else {
            write_log("BACK_PIPE created.\n");
            }   

        // Create thread Receiver
        res1 = pthread_create(&receiver, NULL, receiver_f, NULL);
        if (res1 != 0) {
            erro("Error creating thread receiver");
            exit(EXIT_FAILURE);
        }

        // Create thread Sender
        res2 = pthread_create(&sender, NULL, sender_f, NULL);
        if (res2 != 0) {
            erro("Error creating thread sender");
            exit(EXIT_FAILURE);
        }

        

        // Wait for threads to finish before exiting
        pthread_join(receiver, NULL);
        pthread_join(sender, NULL);

        // Exit child process
        exit(EXIT_SUCCESS);
    }

    // Parent process continues here
    waitpid(auth_manager_process, NULL, 0); // Wait for child process to finish
}




int main(int argc, char *argv[]){
    //limpa o ficheiro log 
    clear_file("log.txt");
    
    //Abre o semaforo 
    log_sem = sem_open("/log_semaphore", O_CREAT | O_EXCL, 0644, 1);

    if (log_sem == SEM_FAILED) {
        if (errno == EEXIST) {
            // If the semaphore already exists, open it without creating
            log_sem = sem_open("/log_semaphore", 0);
            if (log_sem == SEM_FAILED) {
                perror("Error opening semaphore");
                exit(EXIT_FAILURE);
            }
        } else {
            // If the error is different from "File exists", report it
            perror("Error creating/opening semaphore");
            exit(EXIT_FAILURE);
        }
    }

    write_log("5G_AUTH_PLATFORM SIMULATOR STARTING...\n");


    write_log("Process System_Manager Created\n");



    Configuration config=read_config("config.txt");

    write_log("CONFIGURATION READ SUCCESSFULY\n");

    

    
    write_log("Creating shared memory... \n");


    // Create shared memory
	
    int shmid;
    UserData *Users;
    int num_users = config.mobile_users  ; // Example number of users

    
    // Create and attach to shared memory
    Users = create_sm(&shmid, num_users);

    if (Users == NULL) {
        fprintf(stderr, "Failed to create and attach shared memory.\n");
        return EXIT_FAILURE;
    }

    // Initialize the shared memory data
    for (int i = 0; i < num_users; i++) {
        Users[i].UserID =-1;
        Users[i].PlafondInicial = -1; // Example value
        Users[i].SaldoAtualizado= -1;
    }
    monitor_engine();

    auth_requests_manager(config);
    

    write_log("5G_AUTH_PLATFORM SIMULATOR CLOSING...\n");

    if (sem_unlink("/log_semaphore") == -1) {
        perror("Error unlinking semaphore");
        exit(EXIT_FAILURE);
    }
    
    // Detach the shared memory segment when done
    if (shmdt(Users) == -1) {
        perror("shmdt failed");
        return EXIT_FAILURE;
    }

    printf("Shared memory segment detached.\n");
    
    return 0;
}