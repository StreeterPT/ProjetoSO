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

#define MAX_BUF 1024
#define NUM_SERVICES 3
#define TAMANHO_STRING 20

#define USER_PIPE "/tmp/user_pipe"

unsigned long seconds() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec;
}
int verificaServicoEmExecucao(int pid) {
    int status;
    // WNOHANG faz com que waitpid não bloqueie o programa
    pid_t result = waitpid(pid, &status, WNOHANG);
    if (result == 0) {
        // O processo ainda está em execução
        return 1;
    } else if (result == -1) {
        // Ocorreu um erro ao verificar o estado do processo
        return -1;
    } else {
        // O processo já terminou
        return 0;
    }
}
// Função para retornar o nome do serviço com base no índice
void *trataprocessor(int id,int tempo_pedido,char a[TAMANHO_STRING],int data_requested,int pid_mobile_user){
    
    printf("Mobile User %d solicitando autorizacao para servico %s,numero: %d - %d MB.\n", pid_mobile_user, a,id, data_requested);
    sleep(tempo_pedido);
    printf("encerrou o processo %d\n", id);
            // Encerrar o processo filho após a conclusão da solicitação
    
    
   
    
         
            
    return 0;
    
       
}
int verificaTempoServico(int intervals[NUM_SERVICES],char a[TAMANHO_STRING]){

    if(strcmp(a, "MUSICA") == 0) {
        return intervals[1];
    }
    else if(strcmp(a, "VIDEO") == 0) {
        return intervals[0];
    }
    else if(strcmp(a, "SOCIAL") == 0) {
        return intervals[2];
    }
    else {
        return 0;
    }
}
bool verificaServico(char a[TAMANHO_STRING]) {
    if(strcmp(a, "MUSICA") == 0) {
        return true;
    }
    else if(strcmp(a, "VIDEO") == 0) {
        return true;
    }
    else if(strcmp(a, "SOCIAL") == 0) {
        return true;
    }
    else {
        return false;
    }
}
/*
char* retorna_servico(int a) {
    char* servico = NULL;
    
    if (a == 0) {
        servico = strdup("VIDEO");
    } else if (a == 1) {
        servico = strdup("MUSICA");
    } else if (a == 2) {
        servico = strdup("SOCIAL");
    } else {
        servico = strdup("ERRO");
    }

    return servico;
}
*/


// Função para lidar com o sinal SIGINT (Ctrl+C)
void sigint_handler(int sig) {
    printf("Mobile User encerrado pelo usuário.\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    int fd;
    
    fd = open(USER_PIPE, O_WRONLY);
    if (argc != 7) {
        printf("Uso: %s <plafond_inicial> <max_pedidos> <intervalos_VIDEO_MUSIC_SOCIAL> <dados_reservar>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Registrar o handler para o sinal SIGINT
    signal(SIGINT, sigint_handler);

    // Inicializar a semente para a função rand()
    

    // Extrair argumentos da linha de comando
    int initial_balance = atoi(argv[1]);
    int max_requests = atoi(argv[2]);
    int intervals[NUM_SERVICES];
    //meter com atoi
    
    int intervaloVideo=atoi(argv[3]);
    int intervaloMusica=atoi(argv[4]);
    int intervaloSocial=atoi(argv[5]);


    int data_to_reserve= atoi(argv[6]);
    
    int contadorVideo=0;
    int contadorMusica=0;
    int contadorSocial=0;

    printf("Mobile User iniciado com plafond inicial de %d MB.\n", initial_balance);

    int total_requests = 0;
    int current_balance = initial_balance;
    pid_t id = getpid();
    fd = open(USER_PIPE, O_WRONLY);
    char mensagem[MAX_BUF];
    snprintf(mensagem, MAX_BUF, "%d#%d\n", id, initial_balance);


    // Escrever na pipe
    write(fd, mensagem, strlen(mensagem) + 1);

    // Loop para simular a geração de solicitações de autorização
    unsigned long startTime = seconds();

    while (total_requests < max_requests && current_balance > 0) {
        unsigned long endTime = seconds();
        if((endTime-startTime)==intervaloVideo*(contadorVideo+1)){
            printf("%lu\n",(endTime-startTime));
            char mensagem[MAX_BUF];
            snprintf(mensagem, MAX_BUF, "%d#Video#%d\n", id,data_to_reserve );
            // Escrever na pipe
            write(fd, mensagem, strlen(mensagem) + 1);
            contadorVideo++;
            
            total_requests++;
            if(total_requests==max_requests){
                break;
            }
        }
        if((endTime-startTime)==intervaloSocial*(contadorSocial+1)){
            printf("%lu\n",(endTime-startTime));
            char mensagem[MAX_BUF];
            snprintf(mensagem, MAX_BUF, "%d#Social#%d\n", id,data_to_reserve );
            // Escrever na pipe
            write(fd, mensagem, strlen(mensagem) + 1);
            contadorSocial++;
            total_requests++;
            if(total_requests==max_requests){
                break;
            }
        }
        if((endTime-startTime)==intervaloMusica*(contadorMusica+1)){
            printf("%lu\n",(endTime-startTime));
            char mensagem[MAX_BUF];
            snprintf(mensagem, MAX_BUF, "%d#Musica#%d\n", id,data_to_reserve );
            // Escrever na pipe
            write(fd, mensagem, strlen(mensagem) + 1);
            contadorMusica++;
            total_requests++;
            if(total_requests==max_requests){
                break;
            }

        }
    }
    wait(NULL);
    printf("Mobile User encerrado apos %d pedidos de autorizacao.\n",  total_requests);
    printf("numero de pedidos de Musica %d\n",contadorMusica);
    printf("numero de pedidos de Social %d\n",contadorSocial);
    printf("numero de pedidos de Video %d\n",contadorVideo);
    return 0;
}