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

    // Initialize the mutex
    if (pthread_mutex_init(&queue->mutex, NULL) != 0) {
        fprintf(stderr, "Failed to initialize mutex.\n");
        return -1;
    }
    
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

    pthread_mutex_lock(&queue->mutex);

    if (is_full(queue)) {
        fprintf(stderr, "Queue is full.\n");
        pthread_mutex_unlock(&queue->mutex); // Unlock before returning
        return -1;
    }

    queue->rear = (queue->rear + 1) % queue->max_size;  // Circular behavior
    queue->data[queue->rear] = pedido;
    queue->count++;

    // Unlock the mutex after modifying the queue
    pthread_mutex_unlock(&queue->mutex);
    
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

void cleanup_queue(Queue* queue) {
    if (queue->data != NULL) {
        free(queue->data);
        queue->data = NULL;
    }

    // Destroy the mutex to free its resources
    pthread_mutex_destroy(&queue->mutex);
}