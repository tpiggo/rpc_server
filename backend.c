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
#define MAXCLIENTS 2
#define PIPEBUF 64

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
	if (serv==NULL){
		// Memory issue!
		printf("Error: Out of memory!\n");
		return NULL;
	}
    serv->sockfd = sockfd;
    serv->shutdown = 0;
    return serv;
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

	if (parsed==NULL){
		// Memory issue!
		printf("Error: Out of memory!\n");
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
* Function for servicing the RPC call
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
   	server_msg serv_msg = {"","",0};
    char mess[BUFSIZE];
	if (strcmp(name, "add")==0){
		int length = 2;
		// Get the arguments seperated
        char ** parsed = parse_args(length, args);
        // Verify validity or return
        if (parsed == NULL){
          sprintf(serv_msg.error_msg, "Error: Invalid arguments. %s takes %d arguments", "add", length);
		  serv_msg.error = 1;
		  //No need to free.
        } else {
          int ret = addInts(atoi(parsed[0]), atoi(parsed[1]));
          sprintf(serv_msg.ret_val, "%d", ret);
          // free dynamically allocated memory
          for (int i = 0; i<length; i++){
            free(*(parsed+i));
          }
            free(parsed);
        }
	} else if (strcmp(name, "multiply")==0){
		int length = 2;
		// Get the arguments seperated
        char ** parsed = parse_args(length, args);
        // Verify validity or return error
        if (parsed == NULL){
          sprintf(serv_msg.error_msg, "Error: Invalid arguments. %s takes %d arguments", "multiply", length);
		  serv_msg.error = 1;
          //No need to free.
        } else {
          int ret = multiplyInts(atoi(parsed[0]), atoi(parsed[1]));
          sprintf(serv_msg.ret_val, "%d", ret);
          // free dynamically allocated memory
          for (int i = 0; i<length; i++){
            free(*(parsed+i));
          }
          free(parsed);
        }
	} else if (strcmp(name, "divide")==0){
		int length = 2;
		// Get the arguments seperated
        char ** parsed = parse_args(length, args);
        // Verify validity or return
        if (parsed == NULL){
          sprintf(serv_msg.error_msg, "Error: Invalid arguments. %s takes %d arguments", "divide", length);
		  serv_msg.error = 1;
          //No need to free.
        } else {
          float f1 = atof(parsed[0]);
          float f2 = atof(parsed[1]);
          if (f2==0){
            sprintf(serv_msg.error_msg, "Error: Division by zero!");
			serv_msg.error = 1;
          } else {
            float ret = divideFloats(f1, f2);
            sprintf(serv_msg.ret_val, "%f", ret);
          }
          // free the memory!
          // free dynamically allocated memory
          for (int i = 0; i<length; i++){
            free(*(parsed+i));
          }
          free(parsed);
        }
	} else if (strcmp(name, "factorial")==0){
		int length = 1;
		// Get the arguments seperated
        char ** parsed = parse_args(length, args);
        // Verify validity or return
        if (parsed == NULL){
          sprintf(serv_msg.error_msg, "Error: Invalid arguments. %s takes %d arguments", "factorial", length);
		  serv_msg.error = 1;
          //No need to free.
        } else {
          uint64_t ret = factorial(atoi(parsed[0]));
          sprintf(serv_msg.ret_val, "%ld", ret);
          // free the memory!
          // free dynamically allocated memory
          free(*(parsed));
          free(parsed);
        }
	} else if (strcmp(name, "sleep")==0){
		int length = 1;
		// Get the arguments seperated
        char ** parsed = parse_args(length, args);
        // Verify validity or return
        if (parsed == NULL){
          sprintf(serv_msg.error_msg, "Error: Invalid arguments. %s takes %d arguments", "sleep", length);
		  serv_msg.error = 1;
          //No need to free.
        } else {
          int time = atoi(parsed[0]);
          sleep(time);
          // find free mem above
          // free dynamically allocated memory
          free(*(parsed));
          free(parsed);
        }
	}else{
      // Not found! Error!!!
      sprintf(serv_msg.error_msg, "Error: \"%s\" not found!", name);
	  serv_msg.error = 1;
    }
    send_message(client, (char *)&serv_msg, BUFSIZE);
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
int handle_client(int client, rpc_t* server){
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
	  printf("Command: %s Args: %s\n", amess->cmd, amess->args);
      if (strcmp(amess->cmd, "quit") == 0){
        quit_shut = 1;
        break;
      } else if (strcmp(amess->cmd, "exit") == 0){
		  break;
	  }
      RPC_Serve(server, amess->cmd, amess->args, client);
    }
	server_msg return_msg = {"", "", 0};
    strcpy(return_msg.ret_val, "Goodbye!\n");
	send_message(client, (char *)&return_msg, BUFSIZE);
	// Close the client here!
	close(client);
    printf("Done serv_loop!\n");
    return quit_shut;
}

int serv_client(int clientfd, int writepipe[]){
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
	// variable space
	char *myIP;
	int port, clientfd;
	memcpy(&myIP, &argv[1], sizeof(char*));
	port = atoi(argv[2]);
	rpc_t* server;
	pid_t child;
	struct child_t clients[MAXCLIENTS];
	// Note here: pipe_PC is parent to child. pipe_CP is child to parent
	int pipe_PC[2*MAXCLIENTS];
	int pipe_CP[2*MAXCLIENTS];
	char readmsg[PIPEBUF];
	int child_num;
	// Start the server.
	server = RPC_Init(myIP, port);
	if (server == NULL){
		// If the server was not started we have a null type
		printf("Server failed! Ending!\n");
		return 1;
	} else{
		// Everything went well
		printf("Server listening on %s:%d\n", myIP, port);
	}

	//Create the pipe. If it fails; cancel!
	if(pipe(pipe_PC) < 0){
		printf("Can't create pipe_PC!\n");
		return 1;
	}
	if(pipe(pipe_CP) < 0){
		printf("Can't create pipe_PC!\n");
		return 1;
	}


	for (int i = 0; i < MAXCLIENTS; i++){
		child = fork();
		if (child == 0){
			close(pipe_PC[2*i+1]); // close unwanted write side
			close(pipe_CP[2*i]); // close unwanted read side
			child_num = i;
			break;
		} else {
			close(pipe_PC[2*i]); // close unwanted read side
			close(pipe_CP[2*i+1]); // close unwanted write side
			clients[i].childPID = child;
			clients[i].clientfd = -1; 
		}
	}


	if (child == 0){
		printf("I am %d child! Now wait for a message!\n", child_num);
		read(pipe_PC[2*child_num], readmsg, PIPEBUF);
		printf("Received Message: %s", readmsg);
		write(pipe_CP[2*child_num+1], "Clear as Day", PIPEBUF);
		printf("Dying now!");
		exit(0);
	}

	while (server->shutdown != 1){
		clientfd = accept_on_server_socket(server);
		// This is where we check to see if someone has returned!
		// Read all the pipes and fined out if anyone has become free but more importantly
		//If anyone has called for the server to die.
		printf("Writing to children!\n");
		write(pipe_PC[1], "Hello First My child!", PIPEBUF);
		write(pipe_PC[3], "Hello Second My child!", PIPEBUF);
		read(pipe_PC[0], readmsg, PIPEBUF);
		printf("Child said: %s\n", readmsg);
		read(pipe_PC[2], readmsg, PIPEBUF);
		printf("Child said: %s\n", readmsg);
		server->shutdown = handle_client(clientfd, server);
	}
	// printing the array of pid's *************
	printf("array: ");
	for(int i =0; i<MAXCLIENTS; i++){
		printf("{%d;%d} ", clients[i].childPID, clients[i].clientfd);
	}
	printf("\n");
	// ****************

    RPC_Close(server);
    printf("Done main loop!\n");
    return 0;

}