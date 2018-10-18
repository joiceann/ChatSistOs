#include <stdio.h>
#include <sys/socket.h> //For Sockets
#include <stdlib.h>
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <json.h>

char user[10];
char opcion [3];

struct json_object * create_handshake(char* uno, char* origin, char* host){
	printf("Entra:  %s %s %s \n", uno , origin,host);

	struct json_object  *param1, *param2, *param3;
	    struct json_object *response;
	   	response = json_object_new_object();
	   	param1 = json_object_new_string(uno);
	   	param2 = json_object_new_string(origin);
	   	param3 = json_object_new_string(host);
	   	json_object_object_add(response, "user", param1);
		json_object_object_add(response, "origin", param2);
		json_object_object_add(response, "host", param3);

		/*const char *respuesta1 = json_object_get_string(response);
		printf("%s",respuesta1);*/

	return response;

}



int do_handshake(){
	struct sockaddr_in server; //This is our main socket variable.
	int fd; //This is the socket file descriptor that will be used to identify the socket
	int conn; //This is the connection file descriptor that will be used to distinguish client connections.
	//char message[100] = ""; //This array will store the messages that are sent by the server
	
	printf("antes del socket\n");
	fd = socket(AF_INET, SOCK_STREAM, 0);
	server.sin_family = AF_INET;
	server.sin_port = htons(8096);
	printf("inet\n");
	inet_pton(AF_INET, "192.168.0.101", &server.sin_addr); //This binds the client to localhost
	printf("primero");
    //This connects the client to the server.
	int error = connect(fd, (struct sockaddr *)&server, sizeof(server));
    if (error < 0) {
        perror("Error");
    }
	printf("segundo\n");


	char *userr = user;
	char *origin = "dos";
	char *host = "tres";

	struct json_object *respuesta = create_handshake(user,origin,host);
	const char *resp = json_object_get_string(respuesta);

	//send_message(fd, message, resp);
	/*while(1) {
		printf("Enter a message: \n");
		fgets(message, 100, stdin);
		send(fd, message, strlen(message), 0);
		//An extra breaking condition can be added here (to terminate the while loop)
	}*/

	return 0;
}

void * send_message(int new_socket_fd, struct sockaddr *cl_addr, void * message) {
	sendto(new_socket_fd, message, 100, 0, (struct sockaddr *) &cl_addr, sizeof cl_addr);
	return;
}

int login(){
	printf("\nIngrese su nombre de usuario\n");
	char otherString[10];
	fgets(otherString, 10, stdin);
	strncpy(user, otherString, 11);
	printf("\n \n \n*******************************************************************\n		         Bienvenido al Chat CJ \n           			%s\n*******************************************************************\n", user);
	return 0;
}



/*char **/ menu(){
	printf("\n		         MENU\n  -Ingrese el numero de la opcion que desea realizar-\n1.Chatear con usuarios.\n2.Cambiar de status.\n3.Listarlos usuarios conectados al sistema de chat y su información.\n4.Desplegarinformación de un usuario en particular.\n5.Salir.\n");
	char opcion [3];
	fgets(opcion,3,stdin);
	//opc = opcion;
	printf("ESTO %s \n",opcion);
	//return opcion;
}

	
int main(){
	login();
	/*char *userr = user;
	char *origin = "dos";
	char *host = "tres";
	struct json_object *respuesta = create_handshake(user,origin,host);*/
	do_handshake();
	menu();
	printf("La opcion seleccionada es %s \n", opcion);
	
	
	/*struct sockaddr_in server; //This is our main socket variable.
	int fd; //This is the socket file descriptor that will be used to identify the socket
	int conn; //This is the connection file descriptor that will be used to distinguish client connections.
	char message[100] = ""; //This array will store the messages that are sent by the server
	
	printf("antes del socket\n");
	fd = socket(AF_INET, SOCK_STREAM, 0);
	server.sin_family = AF_INET;
	server.sin_port = htons(8096);
	printf("inet\n");
	inet_pton(AF_INET, "127.0.0.1", &server.sin_addr); //This binds the client to localhost
	printf("primero");
    //This connects the client to the server.
	int error = connect(fd, (struct sockaddr *)&server, sizeof(server));
    if (error < 0) {
        perror("Error");
    }
	printf("segundo\n");
	while(1) {
		printf("Enter a message: \n");
		fgets(message, 100, stdin);
		send(fd, message, strlen(message), 0);
		//An extra breaking condition can be added here (to terminate the while loop)
	}*/
	 
	 
	return 0;
}

