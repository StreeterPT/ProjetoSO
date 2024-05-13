#include "dados.h"

// Initialize the queue with a specified size
int init_queue(Queue *queue, int size) {

    queue->data = (Pedido_User *)malloc(size * sizeof(Pedido_User));  // Allocate memory for the queue
    if (queue->data == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return -1;
    }

    queue->front = 0;
    queue->rear = -1;
    queue->count = 0;
    queue->max_size = size;

    
    
    return 0;  // Success
}

// Check if the queue is full
int is_full(const Queue *queue) {
    return queue->count == queue->max_size;
}

// Check if the queue is empty
int is_empty(const Queue *queue) {
    return queue->count == 0;
}

// Check if the queue is below or equal to 50% capacity
int is_below_or_equal_50(const Queue *queue) {
    return ((float)queue->count / queue->max_size) <= 0.5;  // Returns 1 if below or equal to 50%
}

// Enqueue an item into the queue
int enqueue(Queue *queue, Pedido_User pedido) {

    

    if (is_full(queue)) {
        fprintf(stderr, "Queue is full.\n");
        
    }

    queue->rear = (queue->rear + 1) % queue->max_size;  // Circular behavior
    queue->data[queue->rear] = pedido;
    queue->count++;
    
    return 0;  // Success
}

// Dequeue an item from the queue
Pedido_User dequeue(Queue *queue) {
    Pedido_User empty_pedido = {NULL, NULL, 0};

    if (is_empty(queue)) {
        fprintf(stderr, "Queue is empty.\n");
        return empty_pedido;
    }

    Pedido_User dequeued = queue->data[queue->front];
    queue->front = (queue->front + 1) % queue->max_size;
    queue->count--;
    return dequeued;
}

// Create a new Pedido_User
Pedido_User create_pedido_user(int user_id, int tipo, int quant_dados) {
    Pedido_User pedido;
    pedido.UserID = user_id;
    pedido.Tipo = tipo;
    pedido.QuantDados = quant_dados;
    pedido.StartTime = clock();
    return pedido;
}


// Function to print all items in a queue
void print_queue(const Queue *queue) {
    if (is_empty(queue)) {
        write_log("Queue is empty.\n");
        return;
    }

    write_log("Queue contents:\n");
    int index = queue->front;  // Start at the front of the queue
    for (int i = 0; i < queue->count; i++) {
        Pedido_User item = queue->data[index];
        printf("UserID: %d, Tipo: %d, QuantDados: %d\n", item.UserID, item.Tipo, item.QuantDados);

        // Move to the next index, considering circular behavior
        index = (index + 1) % queue->max_size;
    }
}

// Function to reset the statistics in shared memory
void reset_stats(SharedMemory* shared_mem) {
    if (!shared_mem) {
        fprintf(stderr, "Shared memory not initialized.\n");
        return;
    }

    // Lock the mutex to ensure exclusive access to the shared memory
    
    pthread_mutex_lock(&shared_mem->mutex_user_info);
    // Reset each service statistic to zero
    for (int i = 0; i < 3; i++) {
        shared_mem->stats[i].totaldata = 0;  // Reset total data used
        shared_mem->stats[i].totalreq = 0;   // Reset total number of requests
    }

    // Unlock the mutex after completing the reset
    pthread_mutex_unlock(&shared_mem->mutex_user_info);
}


void register_user(int user_id, int initial_plafond, SharedMemory* shared_mem) {
    if (!shared_mem) {
        fprintf(stderr, "Shared memory not initialized.\n");
        return;
    }

    // Lock the mutex for user information
    pthread_mutex_lock(&shared_mem->mutex_user_info);

    // Find an empty slot to register the new user
    for (int i = 0; i < shared_mem->num_users; i++) {
        if (shared_mem->Users[i].UserID == -1) {  // If the slot is empty
            shared_mem->Users[i].UserID = user_id;  // Assign the user ID
            shared_mem->Users[i].PlafondInicial = initial_plafond;  // Set initial plafond
            shared_mem->Users[i].SaldoAtualizado = initial_plafond;  // Current balance
            break;
        }
    }


    // Unlock the mutex after registration
    pthread_mutex_unlock(&shared_mem->mutex_user_info);
}


void consume_service(int user_id,int serviceid,int data, SharedMemory* shared_mem){
    if (!shared_mem) {
        fprintf(stderr, "Shared memory not initialized.\n");
        return;
    }
    pthread_mutex_lock(&shared_mem->mutex_user_info);

    for (int i = 0; i < shared_mem->num_users; i++) {
        if (shared_mem->Users[i].UserID == user_id){
            if(shared_mem->Users[i].SaldoAtualizado >= data){
                shared_mem->Users[i].SaldoAtualizado -= data;
                shared_mem->stats[serviceid-1].totaldata+=data;
                shared_mem->stats[serviceid-1].totalreq+=1;

            }
        }
    }
    pthread_mutex_unlock(&shared_mem->mutex_user_info);

}
