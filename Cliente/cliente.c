#include <stdio.h>
#include <sys/socket.h> //For Sockets
#include <stdlib.h>
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <json.h>

int fd; 
const char * id_own; 

void * exit_(){
    //Envio del request
    const char *request_string = "BYE";
    printf("%s \n", request_string);
    send(fd, request_string, strlen(request_string), 0);
    printf("Enviado\n");

    return NULL;

}

void * list_user (){
    struct json_object  *param1, *param2;
    char * action;
    char id[3];
    action = "LIST_USER";


    printf("Ingrese el el id del usuario\n");
    scanf("%s", id);

    struct json_object *request;
    request = json_object_new_object();
    param1 = json_object_new_string(action);
    param2 = json_object_new_string(id);

    json_object_object_add(request, "action", param1);
    json_object_object_add(request, "id", param2);

    //Envio del request
    const char *request_string = json_object_get_string(request);
    printf("%s \n", request_string);
    send(fd, request_string, strlen(request_string), 0);
    printf("Enviado\n");

    return NULL;

}

void * list_users (){
    struct json_object  *param1;
    char * action;
    action = "LIST_USER";
    struct json_object *request;
    
    request = json_object_new_object();
    param1 = json_object_new_string(action);

    json_object_object_add(request, "action", param1);

    //Envio del request
    const char *request_string = json_object_get_string(request);
    printf("%s \n", request_string);
    send(fd, request_string, strlen(request_string), 0);
    printf("Enviado\n");

    return NULL;

}

void * change_status(){
    struct json_object  *param1, *param2, *param3;
    char * action;
    char status[10];
    struct json_object *request;

    action = "CHANGE_STATUS";
    request = json_object_new_object();
    param1 = json_object_new_string(action);
    param2= json_object_new_string(id_own);
    


    printf("Ingrese el numero del estado al que desea cambiar  \n 1.Active\n 2.Busy \n3.Inactive\n");
    scanf("%s", status);

    if (strcmp(status, "1")==0){
        param3= json_object_new_string("active");
    }
    else if (strcmp(status, "2")==0){
        param3= json_object_new_string("busy");
    }
    else if (strcmp(status, "3")==0){
        param3= json_object_new_string("inactive");
    }


    json_object_object_add(request, "action", param1);
    json_object_object_add(request, "user", param2);
    json_object_object_add(request, "status", param3);

    //Envio del request
    const char *request_string = json_object_get_string(request);
    printf("%s \n", request_string);
    send(fd, request_string, strlen(request_string), 0);
    printf("Enviado\n");

    return NULL;



}

void * send_message(){
    struct json_object  *param1, *param2, *param3, *param4;
    char message[140], to[5];
    char *action;
    struct json_object *request;


    action = "SEND_MESSAGE";

    printf("Ingrese su mensaje \n");
    scanf("%s", message);

    printf("Ingrese el id \n");
    scanf("%s", to);

    request = json_object_new_object();
    param1 = json_object_new_string(action);
    param2 = json_object_new_string(id_own);
    param3 = json_object_new_string(to);
    param4 = json_object_new_string(message);

    json_object_object_add(request, "action", param1);
    json_object_object_add(request, "from", param2);
    json_object_object_add(request, "to", param3);
    json_object_object_add(request, "message", param4);

    //Envio del request
    const char *request_string = json_object_get_string(request);
    printf("%s \n", request_string);
    send(fd, request_string, strlen(request_string), 0);
    printf("Enviado\n");

    return NULL;
}

void * listen_server(){
    while(1){
        printf("INGRESA AL NUEVO THREAD\n");
        //Espera response del servidor
        char listener_mssg[2048];
        recv(fd, listener_mssg, sizeof(listener_mssg), 0);
        //printf(" Este es lo que escucha %s\n", listener_mssg);
        struct json_object *action, *action_json;        action = json_tokener_parse(listener_mssg);
        if (json_object_object_get_ex(action,"action", &action_json)) {
            const char *action_todo = json_object_get_string(action_json);
             printf("%s \n",action_todo );
            if (strcmp(action_todo, "RECEIVE_MESSAGE")==0){
                struct json_object *from_json, *message_json;
                json_object_object_get_ex(action,"from", &from_json);
                const char *from =json_object_get_string(from_json);
                json_object_object_get_ex(action,"message", &message_json);
                const char *message =json_object_get_string(message_json);

                printf("NUEVO MENSAJE DE %s : %s\n", from, message);
            }
            else if (strcmp(action_todo, "LIST_USER")==0){
                struct json_object *usuario_actual_json, *usuario_id_json, *usuario_name_json, *usuario_status_json;
                struct json_object *usuarios;
                json_object_object_get_ex(action,"users", &usuarios);
                int size = json_object_array_length(usuarios);
                printf("Los usuarios conectados son: \n");
                int i;
                for ( i = 0; i < size; i++) {
                usuario_actual_json = json_object_array_get_idx(usuarios, i);
                json_object_object_get_ex(usuario_actual_json, "id", &usuario_id_json);
                json_object_object_get_ex(usuario_actual_json, "name", &usuario_name_json);
                json_object_object_get_ex(usuario_actual_json, "status", &usuario_status_json);
                const char *usuario_id =json_object_get_string(usuario_id_json);
                const char *usuario_name =json_object_get_string(usuario_name_json);
                const char *usuario_status =json_object_get_string(usuario_status_json);
                printf("ID: %s \n Nombre: %s \n status: %s \n ------------------------------\n", usuario_id, usuario_name,usuario_status);
                }

            }
            else if (strcmp(action_todo, "CHANGED_STATUS")==0){
                struct json_object *usuario;
                struct json_object *usuario_id_json, *usuario_name_json, *usuario_status_json;
                json_object_object_get_ex(action,"user", &usuario);
                json_object_object_get_ex(usuario, "id", &usuario_id_json);
                json_object_object_get_ex(usuario, "name", &usuario_name_json);
                json_object_object_get_ex(usuario, "status", &usuario_status_json);
                const char *usuario_id =json_object_get_string(usuario_id_json);
                const char *usuario_name =json_object_get_string(usuario_name_json);
                const char *usuario_status =json_object_get_string(usuario_status_json);

                printf("El usuario %s cambio de estado a  %s\n",  usuario_name, usuario_status);

            }
            else if (strcmp(action_todo, "USER_CONNECTED")==0){
            	struct json_object *usuario;
            	struct json_object *usuario_id_json, *usuario_name_json, *usuario_status_json;
            	json_object_object_get_ex(action,"user", &usuario);
                json_object_object_get_ex(usuario, "id", &usuario_id_json);
                json_object_object_get_ex(usuario, "name", &usuario_name_json);
                json_object_object_get_ex(usuario, "status", &usuario_status_json);

                const char *usuario_id =json_object_get_string(usuario_id_json);
                const char *usuario_name =json_object_get_string(usuario_name_json);
                const char *usuario_status =json_object_get_string(usuario_status_json);

                printf("El usuario %s se conecto con id %s \n", usuario_name , usuario_id);


            }

            else if (strcmp(action_todo, "USER_DISCONNECTED")==0){
            	struct json_object *usuario;
            	struct json_object *usuario_id_json, *usuario_name_json, *usuario_status_json;
            	json_object_object_get_ex(action,"user", &usuario);
                json_object_object_get_ex(usuario, "id", &usuario_id_json);
                json_object_object_get_ex(usuario, "name", &usuario_name_json);
                json_object_object_get_ex(usuario, "status", &usuario_status_json);

                const char *usuario_id =json_object_get_string(usuario_id_json);
                const char *usuario_name =json_object_get_string(usuario_name_json);
                const char *usuario_status =json_object_get_string(usuario_status_json);

                printf("El usuario %s se desconecto  \n", usuario_name );


            }


        }

    }

}

void * menu(){
    printf("                 MENU\n  -Ingrese el numero de la opcion que desea realizar-\n1.Chatear con usuarios.\n2.Cambiar de status.\n3.Listar los usuarios conectados al sistema de chat y su información.\n4.Desplegarinformación de un usuario en particular.\n5.Salir.\n");
    
}

void * handshake(char* user, char* origin, char* host, int fd){
    
    //Creado json del request 
    struct json_object  *param1, *param2, *param3;
    struct json_object *request;
    request = json_object_new_object();
    param1 = json_object_new_string(user);
    param2 = json_object_new_string(origin);
    param3 = json_object_new_string(host);

    json_object_object_add(request, "user", param1);
    json_object_object_add(request, "origin", param2);
    json_object_object_add(request, "host", param3);


    //Envio del request
    const char *request_string = json_object_get_string(request);
    printf("%s \n", request_string);
    send(fd, request_string, strlen(request_string), 0);
    printf("Enviado\n");

    
    //Espera response del servidor
    char response_string[1024];
    recv(fd, response_string, sizeof(response_string), 0);
    printf(" Este es el response %s\n", response_string );

    //Verificar status del response 
    struct json_object *response, *response_status, *response_user, *response_id;
    response = json_tokener_parse(response_string);
    
    json_object_object_get_ex(response,"status", &response_status);
    json_object_object_get_ex(response,"user", &response_user);
    json_object_object_get_ex(response_user,"id", &response_id);
    const char *status = json_object_get_string(response_status);
    id_own = json_object_get_string(response_id);

    if (strcmp(status,"OK") == 0){

        printf("Conectado %s con id %s\n", status, id_own );
        //crea un nuevo thread para escuchar lo del server
        pthread_t tid;
        pthread_create(&tid, NULL, listen_server, NULL);
        pthread_detach(tid);
        // char opcion[100];
        char *opcion;
        while(1) {
            // opcion = "";
            opcion = malloc(100);
            printf("Dentro while \n");
            menu();
            printf("PROBANDO \n");
            fflush(stdout);
            scanf("%s", opcion);
            printf("antes de \n");
            //printf("%s\n", &opcion);
            if (strcmp(opcion,"1") == 0){
                printf("entra\n");
                send_message();
            }
            else if (strcmp(opcion,"2") == 0){
                change_status();
            }
            else if (strcmp(opcion,"3") == 0){
                printf("Entra a opcion 3\n");
                list_users();
            }
            else if (strcmp(opcion,"4") == 0){
                list_user();
            }
            else if (strcmp(opcion,"5") == 0){
                exit_();
                break;
            }

            
       }


    }else{
        printf("Error al conectar%s\n", status);
        return NULL;
    }

    // pthread_join()


    return NULL;
}



int main(int argc, char *argv[]){

    //creacion del Socket
    struct sockaddr_in server; //This is our main socket variable.
    int conn; //This is the connection file descriptor that will be used to distinguish client connections.
    //char message[100] = ""; //This array will store the messages that are sent by the server

    if (argc < 4) {
        printf("El uso del cliente es: %s [ip] [puerto] [nombre]\n", argv[0]);
        return 0;
    }    
    
    fd = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons((int) strtol(argv[2], NULL, 10));
    
    // inet_pton(AF_INET, "10.10.9.21", &server.sin_addr); //This binds the client to localhost
    

    //This connects the client to the server.
    int error = connect(fd, (struct sockaddr *)&server, sizeof(server));
    if (error < 0) {
        perror("Error");
        return 0;
    }
    
    printf("Socket Creado \n");

    //Hace el handshake

    handshake(argv[3], "dos", "tres", fd);




    return 0 ;
}