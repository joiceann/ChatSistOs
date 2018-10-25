#include <json.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <sys/types.h>


struct userstruct{
    char *status;
    int uid;
    char *username;
    int socket;
    STAILQ_ENTRY(userstruct) entries;
};

STAILQ_HEAD(stailhead, userstruct) usr_head = STAILQ_HEAD_INITIALIZER(usr_head);

void *conn_handler(void *);
int cont = 1;


int find_socket(char *uid)
{
    int i = 0;
    struct userstruct *np;
    
    int uid_int = (int) strtol(uid, NULL, 10);
    
    STAILQ_FOREACH(np, &usr_head, entries)
    {
        if (np->uid == uid_int)
        {
            i = np->socket;
        }
    }
    return i;
}


void signal_all_users(int uid, char *signal_str)
{
    const char *string_response;
    char str[1024];
    int socket;
    
    char *username_t, *status_t;
        
    json_object *response, *response_child, *response_value;
        
    // Json contruction
    response = json_object_new_object();
    response_child = json_object_new_object();
        
    struct userstruct *np;
    
    STAILQ_FOREACH(np, &usr_head, entries)
    {
        if (np->uid == uid){
            username_t = np->username;
            status_t = np->status;
            
        }
    }
    
    STAILQ_FOREACH(np, &usr_head, entries)
    {
        if (np->uid != uid){
            
        // Json contruction
        response = json_object_new_object();
        response_child = json_object_new_object();
        
        sprintf(str, "%d", uid);
        
        response_value = json_object_new_string(str);
        json_object_object_add(response_child, "id", response_value);
            
        response_value = json_object_new_string(username_t);
        json_object_object_add(response_child, "name", response_value);
            
        response_value = json_object_new_string(status_t);
        json_object_object_add(response_child, "status", response_value);
        
        response_value = json_object_new_string(signal_str);
        json_object_object_add(response, "action", response_value);
        
        json_object_object_add(response, "user", response_child);
        
        string_response = json_object_to_json_string(response);
        
        sprintf(str, "%d", np->uid);
        
        socket = find_socket(str);
            
        // Send response
        send(socket, string_response, strlen(string_response), 0);
        }
    }
}


void change_status_response(char *uid, char *new_status)
{
    struct userstruct *np;
    int uid_int = (int) strtol(uid, NULL, 10);
    
    STAILQ_FOREACH(np, &usr_head, entries)
    {
        if(np->uid == uid_int )
        {
            np->status = new_status;
        }
    }
    
    signal_all_users(uid_int, "CHANGED_STATUS");
}


void list_user_response(int socket, int uid)
{
    const char *string_response;
    char str[1024];
    
    struct userstruct *np;
    json_object *response, *response_child, *response_value, *array;
    
    array = json_object_new_array();
    
    response = json_object_new_object();
    response_child = json_object_new_object();
    
    response_value = json_object_new_string("LIST_USER");
    json_object_object_add(response, "action", response_value);
    
   
    
    STAILQ_FOREACH(np, &usr_head, entries)
    {
        
        if (np->uid != uid)
        {
            // Json contruction
            sprintf(str, "%d", np->uid);
            
            response_value = json_object_new_string(str);
            json_object_object_add(response_child, "id", response_value);
            
            response_value = json_object_new_string(np->username);
            json_object_object_add(response_child, "name", response_value);
            
            response_value = json_object_new_string(np->status);
            json_object_object_add(response_child, "status", response_value);
            
            json_object_array_add(array, response_child);
        }
    }
    json_object_object_add(response, "users", array);
    
     // Json to String
    string_response = json_object_to_json_string(response);
    
    // Send response and Exit
    send(socket, string_response, strlen(string_response), 0);
}


void send_msg_response(char *from, char *to, char *str_send_msg)
{
    
    const char *string_response;
    int socket;
    
    json_object *response, *response_value;
    response = json_object_new_object();
    
    // Json contruction
    response_value = json_object_new_string("RECEIVE_MESSAGE");
    json_object_object_add(response, "action", response_value);
    
    response_value = json_object_new_string(from);
    json_object_object_add(response, "from", response_value);

    response_value = json_object_new_string(to);
    json_object_object_add(response, "to", response_value);
    
    response_value = json_object_new_string(str_send_msg);
    json_object_object_add(response, "message", response_value);
    
    // Json to String
    string_response = json_object_to_json_string(response);
    
    // Get the socket
    socket = find_socket(to);
    
    // Send response and Exit
    send(socket, string_response, strlen(string_response), 0);
    
}


void delete_from_list(int uid)
{
    struct userstruct *np;
    
    signal_all_users(uid, "USER_DISCONNECTED");
    
    STAILQ_FOREACH(np, &usr_head, entries) {
        if (np->uid == uid) {
            STAILQ_REMOVE(&usr_head, np, userstruct, entries);
            free(np);
        }
    }
    
    
}


void registration_response(int socket, int uid, char *username, char *status)
{
        const char *string_response;
        char str[1024];
        
        json_object *response, *response_child, *response_value;
        
        // Json contruction
        response = json_object_new_object();
        response_child = json_object_new_object();
        
        sprintf(str, "%d", uid);
        
        response_value = json_object_new_string(str);
        json_object_object_add(response_child, "id", response_value);
            
        response_value = json_object_new_string(username);
        json_object_object_add(response_child, "name", response_value);
            
        response_value = json_object_new_string(status);
        json_object_object_add(response_child, "status", response_value);
        
        response_value = json_object_new_string("OK");
        json_object_object_add(response, "status", response_value);
        
        json_object_object_add(response, "user", response_child);
        
        string_response = json_object_to_json_string(response);
            
        // Send response
        send(socket, string_response, strlen(string_response), 0);
        
        // All other users
        signal_all_users(uid, "USER_CONNECTED");
}


void *error_response(int socket, char *message)
{
    json_object *response, *response_value;
    const char *string_response;
    
    response = json_object_new_object();
    
    // Json contruction
    response_value = json_object_new_string("ERROR");
    json_object_object_add(response, "status", response_value);
    
    response_value = json_object_new_string(message);
    json_object_object_add(response, "message", response_value);
            
    // Json to String
    string_response = json_object_to_json_string(response);
            
    // Send response and Exit
    send(socket, string_response, strlen(string_response), 0);
    return NULL;
}


int find_in_list(char *username)
{
    int i = 0;
    struct userstruct *np;
    
    STAILQ_FOREACH(np, &usr_head, entries)
    {
        if (strcmp (np->username, username) == 0)
        {
            i = 1;
        }
    }
    return i;
}


struct userstruct *handshake_handler(int socket)
{
    // Variables 
    int flag;
    char buff[1024];
    //char str[1024];
    
    struct json_object *handshake, *rq_user;
    struct userstruct *user = malloc(sizeof(struct userstruct));
    
    // The message is received
    recv(socket, buff, sizeof(buff), 0);
    
    handshake = json_tokener_parse(buff);
    json_object_object_get_ex(handshake, "user", &rq_user);

    const char *username = json_object_get_string(rq_user);
    
    if (username != NULL)
    {
        flag = find_in_list((char *)username);
        
        // The username is in the list
        if (flag == 1)
        {
            char *message = "The user already exists on the server";
            printf("%s \n", message);
            error_response(socket, message);
            pthread_exit(0);
        }
        
        // User registration
        user->status = "active";
        user->uid = cont;
        user->username = (char *)username;
        user->socket = socket;
        
        
        STAILQ_INSERT_TAIL(&usr_head, user, entries);
        cont++;
        
        // Registration response
        registration_response(socket, user->uid, user->username, user->status);
        printf("The user: %s has registered correctly \n", user->username);

        
    }else
    {
        char *message = "The user did not define a username";
        printf("%s \n", message);
        error_response(socket, message);
    }

    return user;
}


void *conn_handler (void *client_socket)
{
    
    int socket = *(int *) client_socket;
    char buff[1024];
    
    // Action variables
    char *action;
    struct json_object *message, *rq_action;
    
    // Send message variables
    char  *from, *to, *str_send_msg;
    struct json_object *user_from, *user_to, *send_msg;
    
    // Change status variables
    char *userid_str, *new_s_str;
    struct json_object *userid, *new_s;
    
    
    struct userstruct *user = malloc(sizeof(struct userstruct));
    
    
    // Handshake between client and server
    user = handshake_handler(socket);
    //printf(" afuera del while: %d\n", user->uid);

    // Client-Server communication
    while(recv(socket, buff, sizeof(buff), 0) > 0)
    {
        // Parse the message
        message = json_tokener_parse(buff);
        
        // The client sent a disconnection request
        if(strcmp(buff, "BYE") == 0)
        {
            printf("The user: %s has disconnected \n", user->username);
            delete_from_list(user->uid);
            send(socket, "BYE", strlen("BYE"), 0);
            pthread_exit(NULL);
        }
        
        
    
        if (json_object_object_get_ex(message, "action", &rq_action))
        {
            // Check action
            action = json_object_get_string(rq_action);
            printf("%s \n", action);
            
            if (strcmp(action, "SEND_MESSAGE") == 0)
            {
                json_object_object_get_ex(message, "from", &user_from);
                json_object_object_get_ex(message, "to", &user_to);
                json_object_object_get_ex(message, "message", &send_msg);
            
                from = json_object_get_string(user_from);
                to = json_object_get_string(user_to);
                str_send_msg = json_object_get_string(send_msg);
                
                printf("The user %s is sending a message \n", user->username);
            
                send_msg_response(from, to, str_send_msg);
                
            } 
            
            else if (strcmp(action, "LIST_USER") == 0)
            {
                //printf("dentro del while: %d\n", user->uid);
                printf("the user %s is listing all other users \n", user->username);
                
                list_user_response(socket, user->uid);
            }
            
            else if(strcmp(action, "CHANGE_STATUS") == 0)
            {
                printf("entro a cambiar status \n");
                json_object_object_get_ex(message, "user", &userid);
                json_object_object_get_ex(message, "status", &new_s);
                
                userid_str = json_object_get_string(userid);
                new_s_str = json_object_get_string(new_s);
                
                change_status_response(userid_str, new_s_str);
                printf("the user %s has changed status \n", user->username);
                
            }
        
        }else{
            char *error = "The action request is empty or does not have the correct syntax";
            printf("%s \n", error);
            error_response(socket, error);
        }
        
        bzero(buff, sizeof(buff));
    }
    return NULL;
}   


int main (int argc, char *argv[])
{

	int server_socket, client_socket;
    socklen_t c;
	struct sockaddr_in server, client;
    
    STAILQ_INIT(&usr_head);
    
    memset(&server, 0, sizeof(struct sockaddr_in));
    memset(&client, 0, sizeof(struct sockaddr_in));
    
    pthread_t tid;
    
    // Get the specified port number
	char *puerto = argv[1];
    
    // Socket Creation
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	if(server_socket == -1)
    {
		printf("The socket could not be created \n");
	}

	printf("The socket was created \n");
	
    
    // Prepare the structure sockaddr_in
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
    
    int iport = (int) strtol(puerto, NULL, 10);
	server.sin_port = htons(iport);
    
    // Bind
	if( bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("A local socket could not be assigned to a socket identifier \n");
	}
	
	printf("A local socket was assigned to a socket identifier \n");
    
    // Listen
    listen(server_socket, 10);
    printf("Waiting for new connections ... \n");
    
    c = sizeof(struct sockaddr_in);
    
    
    // Accept
    while((client_socket = accept(server_socket, (struct sockaddr *)&client, &c)))
    {
        printf("A connection was accepted \n");
    
        if(pthread_create(&tid, NULL, conn_handler, (void *)&client_socket) != 0)
        {
            printf("Thread creation failed \n");
        }
    }
}
