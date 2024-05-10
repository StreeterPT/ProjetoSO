#include "dados.h"


void process_request(Pedido_User request,shared_mem_Stats, shared_mem_users){
    //acessa memoria partilhada
    if(request.UserID == 1){
        //mensagem do backoffice reset tipo 0  datastats tipo 1
        if(request.Tipo == 0){
            reset_stats(shared mem stats);
        }
        else if(request.Tipo == 1){
            show_data_stats(shared mem stats);
        }
        else{printf("Invalid request\n");}
        }
    else{ //mobile user -- tipo 0 mensagem registo  tipo 1 video 2 musica 3 social
        if(request.Tipo == 0){
            register_user(request.UserID,request.QuantDados,sharedmemUsers,shared mem stats);
        }
        if(request.Tipo == 1){
            consume_video(request.UserID,request.QuantDados,sharedmemUsers,shared mem stats);
        }
        if(request.Tipo == 2){
            consume_music(request.UserID,request.QuantDados,sharedmemUsers,shared mem stats);
        }
        if(request.Tipo == 2){
            consume_social(request.UserID,request.QuantDados,sharedmemUsers,shared mem stats);
        }
    }

}