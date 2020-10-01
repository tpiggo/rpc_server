/*
* Student: Timothy Piggott
* StudentID: 260855765
* Date: 9/30/2020
*/
#ifndef RPC_H
#define RPC_H

#define CMD_ARGS    256
#define ARGS_LENGTH  256
/*
    Type defining the struct of the client->server message passing interface.
    char* cmd: name of the function you wish to call.
    char* args: arguments passed to the function
*/ 
typedef struct client_msg{
    char cmd[CMD_ARGS];
    char args[ARGS_LENGTH];
} client_msg;
/* 
    Type defining the struct of the server->client message passing interface.
    char* ret_value: returned value to the client
    int error: boolean representing if an error occurred or not. If so, ret_val will be garbage.
    char* error_msg: returned error message to the client
*/
typedef struct server_msg{
    char ret_val[1024];
    int error;
    char error_msg[1024];
} server_msg;
/*
    Interface for representing the functions offered in the service.
    char* name: name given to the function in order to call
    char* params: parameter type of the function
    int values: number of values passed.
*/
typedef struct functions{
    char name[50];
    char params[50];
    int values;
}functions;

/*
    Interface for representing the rpc type
    This is the big type which connects the front and backend of the server with
    generic services provided by the backend stored in functions

    int sockfd: interger representing the socket file descriptor
    int shutdown: interger representing state of the rpc_t
    struct functions: function representation in order to normalize and serialize for
    passing to the client. These will be seen by the client who can use them.
*/
typedef struct rpc_t{
    int sockfd;
    struct functions funcs[5];
    int shutdown;
}rpc_t;

/*
    Required functions
    TA specified they should be here.
*/

rpc_t *RPC_Init(char * host, int port);
rpc_t *RPC_Connect(char * host, int port);
void RPC_Close(rpc_t *r);
void RPC_Call(rpc_t *r, char* name, char* args);

#endif