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


/*
void auth_engine(){
    ssize_t bytes_read;
    Pedido_User request;

    while (1) {
        // Read from unnamed pipe to get requests
        bytes_read = read(unnamed_pipe_read_fd, &request, sizeof(request));
        if (bytes_read == -1) {
            perror("Error reading from unnamed pipe");
            break;
        }

        if (bytes_read == sizeof(request)) {
            // Process the request
            process_authorization_request(request, shared_memory, msg_queue_id, AUTH_PROC_TIME);
        } else {
            printf("Incomplete data received.\n");
        }
    }
}*/