/*
* Student: Timothy Piggott
* StudentID: 260855765
* Date: 9/30/2020
*/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "rpc.h"
#include "a1_lib.h"

#define BUFSIZE   1024

/*
* Connecting to a remote RPC. returns the pointer to the remote rpc.
* Params:
* host: string which contains the server ip number
* port: integer representing the port which the ip is hosting the server. Bind.
*/
rpc_t *RPC_Connect(char *host, int port){
    // defining our variables
    int sockfd;
    char message[BUFSIZE];
    // Connect to the server
    if (connect_to_server(host, port, &sockfd) < 0) {
        fprintf(stderr, "RPC_Connect(): oh no\n");
        return NULL;
    }
    // Retreive the server rpc from the backend
    int ret = recv_message(sockfd, message, BUFSIZE);
    rpc_t* extra = (rpc_t *)message;
    rpc_t *serv = (struct rpc_t*)malloc(sizeof(struct rpc_t));
    serv->sockfd = sockfd;
    for (int i = 0; i < 5; i++){
        serv->funcs[i] = extra->funcs[i];
    }
    serv->shutdown = 0;
    return serv;
    
}


/*
* closing the remote connection to the server, deallocating the space.
* Params:
* r: pointer to the rpc server.
*/
void RPC_Close(rpc_t *r){
    // close the connection to the socket from the clients end. 
    close(r->sockfd);
    // free the space allocated to hold the backend.
    free(r);
}

/*
* closing the remote connection to the server, deallocating the space.
* Params:
* r: pointer to the rpc server.
* name: string with the name of the command we want to call
* args: arguments which are to be sent.
*/
void RPC_Call(rpc_t *r, char* name, char * args){
    char server_mes[BUFSIZE] = { 0 };
    // send the input to server
    client_msg send;
    strcpy(send.cmd, name);
    strcpy(send.args, args);
    send_message(r->sockfd, (char *)&send, BUFSIZE);

    // receive a msg from the server
    ssize_t byte_count = recv_message(r->sockfd, server_mes, sizeof(server_mes));
    
    printf("%s\n", server_mes);
}

/*
* Creating a client message to be sent over the server.
* Params:
* input: string of input which needs to be parsed 
*/
client_msg parse_line(char *input){
    client_msg command;
    // fix the end of the string (take off new line char)
    *(input+strlen(input)-1) = '\0';
    // check if the first element is space or not
    if (*(input) == ' ' && strlen(input) > 1){
        input = input+1;
    }
    // tokenize the string
    char *token = strtok(input, " ");
    int i = 0;
    // Run through the tokens
    while(token != NULL){
        if (i == 0){
            strcpy(command.cmd, token);
        } else if(i>0){
            if (i == 1){
                strcpy(command.args, token);
            } else{
                strcat(command.args, " ");
                strcat(command.args, token);
            }
        }
        i++;
        token = strtok(NULL, " ");
    }
    if (i == 0){
        strcpy(command.cmd, "No Command");
        strcpy(command.args, "");
    }
    if (i == 1){
        // only one word. Garbage in command.args. FIX!
        strcpy(command.args, ""); 
    }
    return command;
}


int main(int argc, char *argv[]){
    if (argc < 3){
        printf("ERROR: Too few arguments, failed to start. Takes 2 arguments, IP and port.\n");
        return -1;
    } else if (argc > 3){
      printf("ERROR: Too many arguments, failed to start. Takes 2 arguments, IP and port.\n");
        return -1;
    }
    // Initializing variables
    rpc_t *backend;
    int shutdown = 0;
    char *myIP;
    int port;
    char user_input[BUFSIZE] = { 0 };
    char user_input2[BUFSIZE] = { 0 };
    // create a server message passer
    client_msg cmd;
    // Set the IP and ports
    memcpy(&myIP, &argv[1], sizeof(char*));
    port = atoi(argv[2]);
    // start the server;
    backend = RPC_Connect(myIP, port);
    if (backend == NULL){
        printf("ERROR: cannot connect!");
        return -1;
    }
    while (backend->shutdown == 0) {
        // receive user input.
        memset(user_input, 0, sizeof(user_input));
        printf(">>");
        // read user input from command line
        char * ret = fgets(user_input, BUFSIZE, stdin);

        //parse the results into a cmd message
        cmd = parse_line(user_input);
        // send the input to server
        RPC_Call(backend, cmd.cmd, cmd.args);
        // receive a msg from the server
        if (strcmp(user_input, "quit") == 0){
            break;
        }
        fflush(stdout);
    }
    RPC_Close(backend);
    return 0;
}
