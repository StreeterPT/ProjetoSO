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


// Função para lidar com o sinal SIGINT (Ctrl+C)
void sigint_handler(int sig) {
    printf("Mobile User encerrado pelo usuário.\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
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
    sscanf(argv[3], "%d %d %d", &intervals[0], &intervals[1], &intervals[2]);
    int data_to_reserve= atoi(argv[4]);
    int pid_musica;
    int pid_video;
    int pid_social;
    int pid_mobile_user=getpid();


    printf("Mobile User iniciado com plafond inicial de %d MB.\n", initial_balance);

    int total_requests = 0;
    int current_balance = initial_balance;
    

    // Loop para simular a geração de solicitações de autorização
    while (total_requests < max_requests && current_balance > 0) {
        // Fork para criar um novo processo para cada solicitação de autorização
        total_requests++;
        printf("digite o tipo de pedido que deseja executar: VIDEO, MUSICA ou SOCIAL\n");
        char tipo_servico[TAMANHO_STRING];
        scanf("%s", tipo_servico);
        if ((strcmp(tipo_servico, "VIDEO") == 0 && verificaServicoEmExecucao(pid_video) == 1) ||
            (strcmp(tipo_servico, "MUSICA") == 0 && verificaServicoEmExecucao(pid_musica)==1)||
            (strcmp(tipo_servico, "SOCIAL") == 0 && verificaServicoEmExecucao(pid_social)==1)) {
            printf("O processo numero %d de %s já está sendo executado.\n",pid_video, tipo_servico);
            continue;
        }
        pid_t pid = fork();
        if (pid < 0) {
            perror("Erro ao criar processo filho");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Código executado pelo processo filho
            // Gerar solicitação de autorização para um serviço aleatório
            int idprocess = getpid();
            int intervalo_servico = verificaTempoServico(intervals,tipo_servico);     
            bool verificacao_de_servico=verificaServico(tipo_servico);
            int data_requested = data_to_reserve;
            // Simular intervalos específicos para cada tipo de serviço
            
            // Verificar se há saldo suficiente para a solicitação
            if (current_balance >= data_requested && verificacao_de_servico == true) {
                
                trataprocessor(idprocess,intervalo_servico,tipo_servico,data_requested,pid_mobile_user);
                // Atualizar o saldo e o número total de solicitações
                current_balance -= data_requested;
                
            } 
            else if(verificacao_de_servico == false) {
                printf("Service %s not found\n", tipo_servico);}
            else {
                printf("Mobile User sem saldo suficiente para solicitar servico %s - %d MB.\n", tipo_servico, data_requested);
            }

            exit(0);
            
        } else {
            if (strcmp(tipo_servico, "MUSICA") == 0) {
                pid_musica = pid;
            } else if (strcmp(tipo_servico, "VIDEO") == 0) {
                pid_video = pid;
            } else if (strcmp(tipo_servico, "SOCIAL") == 0) {
                pid_social = pid;
            }
           
           
            
        }
    }
    wait(NULL);
    printf("Mobile User encerrado apos %d pedidos de autorizacao.\n",  total_requests);

    return 0;
}
