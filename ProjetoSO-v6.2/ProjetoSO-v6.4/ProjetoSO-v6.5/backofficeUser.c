// autores:
// David Cameijo Pinheiro 2021246865
// Guilherme Fernandes Figueiredo 2021236787


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/wait.h> // Para o uso de wait
#include <stdbool.h>
#include <sys/time.h>
#define MAXBUF 200

// Definição do ID do BackOffice User
#define BACKOFFICE_ID 1
#define BACK_PIPE "/tmp/back_pipe"
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
    int fd;

    // Nome do named pipe para comunicação com o Authorization Request Manager
    fd = open(BACK_PIPE, O_WRONLY);
    //ID_backoffice_user#[data_stats | reset]
    // Loop principal
    while (1) {
        char command[100];
        printf("Digite o comando (data_stats | reset): ");
        fgets(command, sizeof(command), stdin);
        char mensagem[200];
        snprintf(mensagem, MAXBUF, "%d#%s", BACKOFFICE_ID,command );
        if(strcmp(command, "data_stats\n") == 0) {
            write(fd, mensagem, strlen(mensagem) + 1);
            memset(mensagem,0,MAXBUF);
            
            
        } else if(strcmp(command, "reset\n") == 0) {
           write(fd, mensagem, strlen(mensagem) + 1);
           memset(mensagem,0,MAXBUF);
        }
        else {
            printf("Comando inválido\n");
        }

        

        // Remover o caractere de nova linha do final da string de comando
        command[strcspn(command, "\n")] = 0;

        // Construir a mensagem para enviar para o named pipe
        char message[100];
        printf("BackOfficeId:%d, comando:%s\n",BACKOFFICE_ID,command);

        



        
        // Abrir o named pipe para escrita
        
        // Escrever a mensagem no named pipe
        

        // Fechar o file pointer do named pipe
       

        // Esperar um pouco para a próxima iteração
        sleep(1); // Simulando uma pausa entre os comandos
    }

    return 0;
}
