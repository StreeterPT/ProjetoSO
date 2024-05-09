#include <stdio.h>
#include <string.h>

void extract_data(const char *message, char *id, char *plafond) {
    // Copy the message to avoid modifying the original string
    char message_copy[100];  // Make a copy
    strncpy(message_copy, message, sizeof(message_copy));
    message_copy[sizeof(message_copy) - 1] = '\0';  // Ensure null-termination

    // Tokenize the message using `#` as a delimiter
    char *token = strtok(message_copy, "#");
    if (token != NULL) {
        strcpy(id, token);  // Store the first part in `id`
    }

    token = strtok(NULL, "#");  // Get the next token
    if (token != NULL) {
        strcpy(plafond, token);  // Store the second part in `plafond`
    }
}

int main() {
    const char *message = "ID_12345#800";  // Example message
    char id[50];  // Buffer for storing the ID
    char plafond[50];  // Buffer for storing the Plafond

    // Extract data from the message
    extract_data(message, id, plafond);

    printf("ID: %s\n", id);
    printf("Plafond Inicial: %s\n", plafond);

    return 0;
}