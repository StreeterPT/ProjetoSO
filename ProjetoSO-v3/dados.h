#ifndef DADOS_H
#define DADOS_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/wait.h>
#include <time.h>
#include <ctype.h>



typedef struct {
    int mobile_users; // número de Mobile Users que serão suportados pelo simulador
    int queue_pos; // número de slots nas filas que são utilizadas para armazenar os pedidos de autorização e os comandos dos utilizadores (>=0)
    int auth_servers; // número de Authorization Engines que devem ser lançados inicialmente (>=1)
    unsigned int auth_proc_time; // período (em ms) que o Authorization Engine demora para processar os pedidos
    unsigned int max_video_wait;  // tempo máximo (em ms) que os pedidos de autorização do serviço de vídeo podemaguardar para serem executados (>=1)
    unsigned int max_others_wait; //tempo máximo (em ms) que os pedidos de autorização dos serviços de música e de redes sociais, bem como os comandos podem aguardar para serem executados (>=1)
} Configuration;

typedef struct{
    int UserID;
    int PlafondInicial;
    int SaldoAtualizado;
} UserData;

typedef struct{
    int UserID;
    int Tipo;
    int QuantDados;
} Pedido_User;

// Definition of Queue

typedef struct {
    Pedido_User *data;  // Pointer to the queue's array
    int front;  // Front index
    int rear;  // Rear index
    int count;  // Number of items in the queue
    int max_size;  // Maximum capacity
    pthread_mutex_t mutex; // Mutex for synchronization
} Queue;

int init_queue(Queue *queue, int size);
int is_full(const Queue *queue);
int is_empty(const Queue *queue);
int is_below_or_equal_50(const Queue *queue);  // New function to check 50% capacity
int enqueue(Queue *queue, Pedido_User pedido);
Pedido_User dequeue(Queue *queue);
Pedido_User create_pedido_user(int user_id, int tipo, int quant_dados);
void free_pedido_user(Pedido_User *pedido);


#endif // DADOS_H