// autores:
// David Cameijo Pinheiro 2021246865
// Guilherme Fernandes Figueiredo 2021236787

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

// Definição do ID do BackOffice User
#define BACKOFFICE_ID 1
void function_dataStats(){
    printf("vai ser executado o dataStats\n");
    return;
}
void function_reset(){
    printf("vai ser executado o functionReset\n");
    return;

}
// Função para lidar com os sinais de interrupção (SIGINT)
void handle_sigint(int sig) {
    printf("Received SIGINT. Exiting...\n");
    exit(EXIT_SUCCESS);
}

int main() {
    // Registrar o tratamento de sinal para SIGINT
    signal(SIGINT, handle_sigint);

    // Nome do named pipe para comunicação com o Authorization Request Manager
    

    // Loop principal
    while (1) {
        char command[100];
        printf("Digite o comando (data_stats | reset): ");
        fgets(command, sizeof(command), stdin);

        // Remover o caractere de nova linha do final da string de comando
        command[strcspn(command, "\n")] = 0;

        // Construir a mensagem para enviar para o named pipe
        char message[100];
        printf("BackOfficeId:%d, comando:%s\n",BACKOFFICE_ID,command);
        if(strcmp(command, "1#data_stats") == 0) {
            function_dataStats();
        } else if(strcmp(command, "1#reset") == 0) {
            function_reset();
        }



        
        // Abrir o named pipe para escrita
        
        // Escrever a mensagem no named pipe
        

        // Fechar o file pointer do named pipe
       

        // Esperar um pouco para a próxima iteração
        sleep(1); // Simulando uma pausa entre os comandos
    }

    return 0;
}
