// Processes a user request while ensuring synchronization
void process_request(Pedido_User request, SharedMemory* shared_mem, Auth_Engine_Manager* auth_engine_manager,int engine_id) {
    if (!shared_mem) {
        fprintf(stderr, "Shared memory not initialized.\n");
        return;
    }
    pthread_mutex_lock(&auth_engine_manager->mutex_flags);
    
    auth_engine_manager->auth_engine_flags[engine_id]=1;
    
    pthread_mutex_unlock(&auth_engine_manager->mutex_flags);

    if (request.UserID == 1) {   
        if (request.Tipo == 0) {
            reset_stats(shared_mem); // Resets the shared memory statistics
        } else if (request.Tipo == 1) {
            show_data_stats(shared_mem); // Displays current data statistics
        } else {
            printf("Invalid request for BackOffice.\n");
        }
        
    } else {     
        if (request.Tipo == 0) {
            // Register a new user
            register_user(request.UserID, request.QuantDados, shared_mem);
        } else if (request.Tipo == 1) {
            // Consume video data
            consume_service(request.UserID,request.Tipo,request.QuantDados, shared_mem);
        } else if (request.Tipo == 2) {
            // Consume music data
            consume_service(request.UserID,request.Tipo,request.QuantDados, shared_mem);
        } else if (request.Tipo == 3) {
            // Consume social data
            consume_service(request.UserID,request.Tipo,request.QuantDados, shared_mem);
        } else {
            printf("Invalid request for Mobile User.\n");
        }
        
        pthread_mutex_unlock(&shared_mem->Users[user_index].mutex_user_info); // Unlock the user info mutex
    }
}