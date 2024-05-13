#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define USER_PIPE "/tmp/user_pipe"
#define MAX_BUF 1024
#define NUM_SERVICES 3
#define TAMANHO_STRING 20
int contagem_pedidos;
int max_requests;
int fd;

struct ServiceRequest {
    char service[TAMANHO_STRING];
    int data_to_reserve;
    int pid_mobile_user;
    int time_sleep;
};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *service_handler(void *arg) {
    struct ServiceRequest *request = (struct ServiceRequest *)arg;
    
   

    
    
    while (contagem_pedidos<max_requests)

    {   
        
        
        char message[MAX_BUF];
        usleep((request->time_sleep*1000)); 
        if (contagem_pedidos >= max_requests) {
            pthread_exit(NULL);
        }
        pthread_mutex_lock(&mutex); 
        
        contagem_pedidos++;
        pthread_mutex_unlock(&mutex);

        // Bloquear o acesso ao pipe
        
        
    
        
        
    
        snprintf(message, MAX_BUF, "%d#%s#%d\n", request->pid_mobile_user, request->service, request->data_to_reserve);
        printf("%s",message);
        ssize_t bytes_written=write(fd, message, strlen(message));
         if (bytes_written <= 0) {
        perror("Erro ao escrever no pipe");
        
        close(fd);
        
        
        
       
        }
         
        
    }
    

    


   
    free(request);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    contagem_pedidos=0;
   
    int initial_balance, data_to_reserve;
    int intervals[NUM_SERVICES];
    pid_t id = getpid();
    pthread_t threads[NUM_SERVICES];

    if (argc != 7) {
        printf("Uso: %s <plafond_inicial> <max_pedidos> <intervalos_VIDEO_MUSIC_SOCIAL> <dados_reservar>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Inicializar variáveis
    initial_balance = atoi(argv[1]);
    max_requests = atoi(argv[2]);
    intervals[0] = atoi(argv[3]);
    intervals[1] = atoi(argv[4]);
    intervals[2] = atoi(argv[5]);
    data_to_reserve = atoi(argv[6]);

    // Inicializar o pipe
    char message[MAX_BUF];

    // Loop principal
   fd = open(USER_PIPE, O_WRONLY);
  
   sprintf(message, "%d#%d\n",id, initial_balance);
    printf("%s\n",message);
    ssize_t bytes_written=write(fd, message, strlen(message));
         if (bytes_written <= 0) {
        perror("Erro ao escrever no pipe");
        
        close(fd);
        
        
        
       
        }
        for (int i = 0; i < NUM_SERVICES; i++) {
            
                struct ServiceRequest *request = malloc(sizeof(struct ServiceRequest));
                if (i == 0)
                    strcpy(request->service, "VIDEO");
                else if (i == 1)
                    strcpy(request->service, "MUSICA");
                    
                else
                    strcpy(request->service, "SOCIAL");
                request->data_to_reserve = data_to_reserve;
                request->pid_mobile_user = id;
                request->time_sleep = intervals[i];
                pthread_create(&threads[i], NULL, service_handler, (void *)request);
            
        }

        // Aguardar a conclusão de todas as threads
        for (int i = 0; i < NUM_SERVICES; i++) {
            for (int j = 0; j < 3; j++) {
                pthread_join(threads[i], NULL);
            }
        }

        // Verificar se atingiu o número máximo de solicitações
        if (contagem_pedidos==max_requests) {
            
        }
    

    // Limpeza e encerramento do programa
   
    return 0;
}

