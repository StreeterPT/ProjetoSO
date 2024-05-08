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
        return -1;
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
Pedido_User create_pedido_user(const char *user_id, const char *tipo, int quant_dados) {
    Pedido_User pedido;
    pedido.UserID = strdup(user_id);
    pedido.Tipo = strdup(tipo);
    pedido.QuantDados = quant_dados;
    return pedido;
}

// Free memory associated with Pedido_User
void free_pedido_user(Pedido_User *pedido) {
    if (pedido->UserID) {
        free(pedido->UserID);  // Free allocated memory
    }
    if (pedido->Tipo) {
        free(pedido->Tipo);
    }
}