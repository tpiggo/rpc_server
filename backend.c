/*
* Student: Timothy Piggott
* StudentID: 260855765
* Date: 9/30/2020
*/


#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdint.h> 
#include <unistd.h>
#include <stdlib.h>
#include "a1_lib.h"
#include "rpc.h"

#define BUFSIZE 1024
#define MAXCLIENTS 10

/*
    Functions to be used by RPC.
    How to implement sleep(int x)??
*/
int addInts(int x, int y){
    return x+y;
}
int multiplyInts(int x, int y){
    return x*y;
}
float divideFloats(float x, float y){
    return x/y;
}
uint64_t factorial(int x){
    uint64_t start = 1;
    for (int i = 1; i<=x; i++){
        start = start*i;
    }
    return start;
}
/* 
    Functions for RPC and its activities.
*/

rpc_t *RPC_Init(char *host, int port){
    // Connect to the server
    int sockfd;
    if(create_server(host, port, &sockfd) < 0){
        fprintf(stderr, "RPC_Init(): oh no\n");
        return NULL;
    }
    // return the pointer to the server
    rpc_t* serv = (rpc_t *)malloc(sizeof(struct rpc_t));
    serv->sockfd = sockfd;
    serv->shutdown = 0;
    return serv;
}

int register_functions(rpc_t *serv){
    // TA said either skip this step or you can do it without dynamic allocation since we know the functions
    // Want to do this dynmically but with different inputs and output type, I was unable to find the answer
    serv->funcs[0].values=2;
    strcpy(serv->funcs[0].name, "add");
    strcpy(serv->funcs[0].params, "int");
    serv->funcs[1].values=2;
    strcpy(serv->funcs[1].name, "multiply");
    strcpy(serv->funcs[1].params, "int");
    serv->funcs[2].values=2;
    strcpy(serv->funcs[2].name, "divide");
    strcpy(serv->funcs[2].params, "float");
    serv->funcs[3].values=1;
    strcpy(serv->funcs[3].name, "factorial");
    strcpy(serv->funcs[3].params, "int");
    serv->funcs[4].values=1;
    strcpy(serv->funcs[4].name, "sleep");
    strcpy(serv->funcs[4].params, "int");
    return 0;
}


void RPC_Close(rpc_t *r){
    // close the server and free its resources.
    close(r->sockfd);
    free(r);
}

/*
  Seperating the arguments received by the server in the message.
  Creates a dynamic array of strings.
  int length: is the number of strings you want to have
  char *args: The arguments which need parsing.

*/

char ** parse_args(int length, char *args){
    char ** parsed = (char**)malloc(sizeof(char*)*length);
    for (int i = 0; i <length; i++){
        *(parsed+i) = (char*)malloc(sizeof(char));
    }
    // Fix the arguments
    char *token = strtok(args, " ");
    int i = 0;
    while(token != NULL && i<length){
        strcpy(*(parsed+i), token);
        token = strtok(NULL, " ");
        i++;
    } if (token!=NULL || i != length ){
      /* If the tocken is not NULL or the number of arguments
      * you have parsed is less than the length; You didn't
      * receive enough arguments. Could take that condition
      * out since we are going to be parsing on the front-end.
      * Return NULL if either are true as you have a problem.
      */ 
        return NULL;
    }

    return parsed;
}

/*
* Function for serving the RPC call
* rpc_t r: pointer to the server
* char *name: name of the function we need to call
* char *args: argument for said function
* int client: the clients socket fd. 
*/


void RPC_Serve(rpc_t *r, char* name, char * args, int client){
    /* Using an if else structure of code for handling each different
    * function. Wanted to dynamically link the functions to the rpc_t
    * but never found an easy way since each function has a different
    * signature.
    */
    server_msg message;
    char mess[BUFSIZE];
    if (strcmp(name, r->funcs[0].name) == 0){
        //Handling add here
        int length = r->funcs[0].values;
        // Get the arguments seperated
        char ** parsed = parse_args(length, args);
        // Verify validity or return
        if (parsed == NULL){
          sprintf(mess, "Error: Invalid arguments. %s takes %d arguments", r->funcs[0].name, length);
          free(parsed);
        } else {
          int ret = addInts(atoi(parsed[0]), atoi(parsed[1]));
          sprintf(mess, "%d", ret);
          // free dynamically allocated memory
          for (int i = 0; i<length; i++){
            free(*(parsed+i));
          }
            free(parsed);
        }
    } else if (strcmp(name, r->funcs[1].name) == 0){
        // Handling multiply.
        int length = r->funcs[1].values;
        // Split the arguments received. 
        char ** parsed = parse_args(length, args);
        // Verify validity or return error
        if (parsed == NULL){
          sprintf(mess, "Error: Invalid arguments. %s takes %d arguments", r->funcs[1].name, length);
          free(parsed);
        } else {
          int ret = multiplyInts(atoi(parsed[0]), atoi(parsed[1]));
          sprintf(mess, "%d", ret);
          // free dynamically allocated memory
          for (int i = 0; i<length; i++){
            free(*(parsed+i));
          }
          free(parsed);
        }
    } else if (strcmp(name, r->funcs[2].name) == 0){
      // Handling divide here
        int length = r->funcs[2].values;
        char ** parsed = parse_args(length, args);
        // Verify validity or return error
        if (parsed == NULL){
          sprintf(mess, "Error: Invalid arguments. %s takes %d arguments", r->funcs[2].name, length);
          free(parsed);
        } else {
          float f1 = atof(parsed[0]);
          float f2 = atof(parsed[1]);
          if (f2==0){
            sprintf(mess, "Error: Division by zero!");
          } else {
            float ret = divideFloats(f1, f2);
            sprintf(mess, "%f", ret);
          }
          // free the memory!
          // free dynamically allocated memory
          for (int i = 0; i<length; i++){
            free(*(parsed+i));
          }
          free(parsed);
        }
    } else if (strcmp(name, r->funcs[3].name) == 0){
      // Handling factorial here
        int length = r->funcs[3].values;
        char ** parsed = parse_args(length, args);
        // Verify validity or return error
        if (parsed == NULL){
          sprintf(mess, "Error: Invalid arguments. %s takes %d arguments", r->funcs[3].name, length);
          free(parsed);
        } else {
          uint64_t ret = factorial(atoi(parsed[0]));
          sprintf(mess, "%ld", ret);
          // free the memory!
          // free dynamically allocated memory
          free(*(parsed));
          free(parsed);
        }
    } else if (strcmp(name, r->funcs[4].name) == 0){
      // Handling sleep here
        int length = r->funcs[4].values;
        char ** parsed = parse_args(length, args);
        // Verify validity or return error
        if (parsed == NULL){
          sprintf(mess, "Error: Invalid arguments. %s takes %d arguments", r->funcs[4].name, length);
          free(parsed);
        } else {
          int time = atoi(parsed[0]);
          sleep(time);
          sprintf(mess, "slept for %d seconds!", time);
          // find free mem above
          // free dynamically allocated memory
          free(*(parsed));
          free(parsed);
        }
    } else{
      // Not found! Error!!!
      sprintf(mess, "Error: \"%s\" not found!", name);
    }
    send_message(client, mess, BUFSIZE);
}

/*
  User defined functions for handling server connections
*/
/*
* Finding the position in the list of children for a specified child. Will return -1
* to indicate process not found.
* Params:
* clients[]: array of child processes
* length: integer of the length of the array
* id: process id in question.
*/
int accept_on_server_socket(rpc_t *serv){
    int clientfd;
    if (accept_connection(serv->sockfd, &clientfd)){
        fprintf(stderr, "accept_on_server(): oh no\n");
        // kill the process
        return -1;
    }
    send_message(clientfd, (const char*)serv, BUFSIZE);
    return clientfd;
}
/*
* Servee the client! Handling the connection for a single
* client within this loop. 
* Params:
* clients: client socket
* server: pointer to the server
*/
int serv_client(int client, rpc_t* server){
  printf("Started serv with server->shutdown=%d\n", server->shutdown);
  int quit_shut = 0;
  // enter the loop if the server shutdown is not the current setting.
  // If it is, cancels closes the server.
    while (server->shutdown == 0) {
      char msg[BUFSIZE];
      memset(msg, 0, sizeof(msg));
      // Receive the message and send it to the
      ssize_t byte_count = recv_message(client, msg, BUFSIZE);
      if (byte_count <= 0) {
        break;
      }
      client_msg* amess = (client_msg*)msg;
      if (strcmp(amess->cmd, "quit") == 0){
        quit_shut = 1;
        break;
      }
      RPC_Serve(server, amess->cmd, amess->args, client);
    }
    char ret[BUFSIZE];
    strcpy(ret, "Goodbye!\n");
    send_message(client, ret, strlen(ret));
    printf("Done serv_loop!\n");
    return quit_shut;
}
/*
* User defined functions for handling concurrent running and maintenance of child
* processes 
*/

/*
* Finding the position in the list of children for a specified child. Will return -1
* to indicate process not found.
* Params:
* clients[]: array of child processes
* length: integer of the length of the array
* id: process id in question.
*/
int position(int clients[], int length, pid_t id){
  for(int i =0; i<length; i++){
    if (clients[i] == id){
      return i;
    }
  }
  return -1;
}

/*
* Number of client processes(children) the server has running.
* Params:
* clients[]: array of child processes
* length: integer of the length of the array
*/
int numberOfClients(int clients[], int length){
  int tot = 0;
  for(int i = 0; i<length; i++){
    if(clients[i]>0)
      tot++;
  }
  return tot;
}

/*
* Returning the status of the fullness of the list. If the client list has reached 
* its maximum or not.
* Params:
* clients[]: array of child processes
* length: integer of the length of the array
*/
int isFull(int clients[], int length){
  // return if the number of clients takes up the whole space
  return numberOfClients(clients, length)==length-1;
}

/*
* Returning the status of the emptiness of the list. If the client list has reached 
* its maximum or not.
* Params:
* clients[]: array of child processes
* length: integer of the length of the array
*/
int isEmpty(int clients[], int length){
  return numberOfClients(clients, length) == 0;
}
/*
* Adding a client process to the list.
* Params:
* clients[]: array of child processes
* length: integer of the length of the array
* id: process id, interger.
*/
int addClient(int clients[], int length, pid_t id){
	if (position(clients, length, id) >=0){
		return 0;
	}
	for (int i = 0; i<length; i++){
		if(clients[i] == -1){
		clients[i] = id;
		break;
		}
	}
  return 1;
}

int removeClient(int clients[], int length, int pos){
	if (pos==-1 || pos>length-1){
		printf("removeclient(): out of bounds!");
		return -1;
	}
	else if (clients[pos] == -1){
		printf("removeclient(): postion empty!");
		return -1;
	}
	clients[pos] = -1;
	return 0;
}

/*
*    Main function for the body for the server.
*/

int main(int argc, char* argv[]){
    // Standard process checking for the inputs of the file.
    // We need a ip and a port. Without it, it will fail to
    // start.
	if (argc <= 2){
		printf("ERROR: Too few arguments, failed to start. Takes 2 arguments, IP and port.\n");
		return -1;
	} else if (argc > 3){
		printf("ERROR: Too many arguments, failed to start. Takes 2 arguments, IP and port.\n");
		return -1;
	}
	// variables.
	char *myIP;
	int port, clientfd;
	memcpy(&myIP, &argv[1], sizeof(char*));
	port = atoi(argv[2]);
	// start the server;
	rpc_t* server;
	server = RPC_Init(myIP, port);
	if (server == NULL){
		// If the server was not started we have a null type
		printf("Server failed! Ending!\n");
		return -1;
	} else{
		// Everything went well
		printf("Server listening on %s:%d\n", myIP, port);
	}
    // Now register all the functions
    // TA said we didn't have to register like in the assignment since different return types makes it very difficult for the scope of this course
    // We will use an if else to serve these, as TA suggested.
    register_functions(server);

    // Create the holder list for the clients
	pid_t clients[MAXCLIENTS];
	for (int i = 0; i<MAXCLIENTS; i++){
		clients[i] = -1;
	}
    pid_t child;
	printf("parent pid=%d\n\n", getpid());
    while (server->shutdown!=1) {
		// Checking to see if we can handle another client or not.
		if (!isFull(clients, MAXCLIENTS)){
			printf("Waitin to accept\n");
			clientfd = accept_on_server_socket(server);
			printf("ss3=%d ppid=%d\n", server->shutdown, getpid());
			if (clientfd < 0){
				// There is a problem. Break
				break;
			}
			child=fork();
			if (child == 0){
				// This is the child process. Will run the server.
				// Close the socket to therefore not have any children of the children
				close(server->sockfd);
				int ret = serv_client(clientfd, server);
				// The return val used by WEXITSTATUS
				// Will be either -1 or 1; using 8 sig bits -> 1, 255
				exit(ret);
			} else{
				addClient(clients, MAXCLIENTS, child);
				// Check if any children have finished and update shutdown accordingly.
				// check if any children are done
				for (int i = 0; i<MAXCLIENTS; i++){
					if (clients[i]>0){
						int ret, rval;
						pid_t find = clients[i];
						ret = waitpid(find, &rval, WNOHANG);
						if(ret==clients[i]){
							removeClient(clients, MAXCLIENTS, i);
							server->shutdown = WEXITSTATUS(rval);
						}
					}
				}
			}
		} else{
			// Maximum Clients have been reached!
			int rval, pos;
			int ret = wait(&rval);
			if ((pos=position(clients,MAXCLIENTS,ret)) >= 0){
				// This is a valid PID we are waiting to finish. 
				printf("Process %d has exited\n", ret);
				server->shutdown = WEXITSTATUS(rval);
        		removeClient(clients, MAXCLIENTS, pos);
			}
		}
    }

	while(!isEmpty(clients, MAXCLIENTS)){
		for (int i = 0; i<MAXCLIENTS; i++){
			if (clients[i]>0){
				int ret, rval;
				pid_t find = clients[i];
				ret = waitpid(find, &rval, WNOHANG);
				if(ret==clients[i]){
					printf("Process %d has exited with ret=%d\n", clients[i], ret);
					printf("EXITSTAT=%d\n", WEXITSTATUS(rval));
					removeClient(clients, MAXCLIENTS, i);
					server->shutdown = WEXITSTATUS(rval);
				}
			}
		}
	}
	// printing the array of pid's *************
	printf("array: ");
	for(int i =0; i<MAXCLIENTS; i++){
		printf("%d ", clients[i]);
	}
	printf("\n");
	// ****************

    RPC_Close(server);
    printf("Done main loop!\n");
    return 0;

}