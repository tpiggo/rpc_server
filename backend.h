/*
* Student: Timothy Piggott
* StudentID: 260855765
* Date: 9/30/2020
*/

#include <stdint.h>
#include "rpc.h" 

#ifndef BACKEND_H
#define BACKEND_H

/**
 *  Create a RPC server.
 *
 *  @params:
 *    host:     Host IP as a string.
 *    port:     Integer number for which port to bind to.
 *  @return:    On success, a connection is set up between the server and
 *              client processes. The function returns the RPC server struct
 *              If an error occurs, the function will NULL.
 */
rpc_t *RPC_Init(char * host, int port);

/**
 *  Closes connection to an RPC server.
 *
 *  @params:
 *    r:        Pointer to a RPC Server connection.
 *  @return:    On success, a connection is closed between the server and
 *              client processes.
 */
void RPC_Close(rpc_t *r);

/**
 *  Serves an RPC client the service.
 *
 *  @params:
 *    r:        Pointer to a RPC Server connection.
 *    name:     Name of the function which can be found on the backend.
 *    args:     Arguments as a string.
 *    client:   Pointer to the client being handled. 
 *  @return:    Handles a command and sends a server message back to the client.
 *             
 */
void RPC_Serve(rpc_t *r, char* name, char * args, int client);

/* Functions for the backend as assignment required.
*/
int addInts(int x, int y);
int multiplyInts(int x, int y);
float divideFloats(float x, float y);
uint64_t factorial(int x);
// Only function not with the same name as assignment; a wrapper for sleep.
int sleep_rpc(int x);

// User defined functions
/**
 *  Seperating the arguments received by the server in the message.
    Creates a dynamic array of strings.
 *
 *  @params:
 *    r:        Host IP as a string.
 *    length:   Number of strings arguments required.
 *    args:     Arguments as a string. 
 *  @return:    On success, parses and returns a dynamically allocated 2d array of all the arguments.
 *              Returns NULL on errors.
 */
char ** parse_args(int length, char *args);

/**
 *  Accept a new client connection on the server side.
 *
 *  @params:
 *    serv:     Pointer to the RPC server. 
 *  @return:    On success, returns an integer value of the file descriptor pointing to the client
 *              Returns -1 if there was an error accepting.
 */
int accept_on_server_socket(rpc_t *serv);

/**
 *  Connect to an RPC server.
 *
 *  @params:
 *    server:   Pointer to the RPC server.
 *    client:   File descriptor to the client connection.
 *  @return:    On handling of the client connection, the sever will listen for the commands passed to it
 *              and handle accordingly. The exit types from this function are -1, 1 and 5. Each integer
 *              return value signifies a different end state: -1 = quit, 1 = cancelled client and 5 = client
 *              ended.
 */
int serv_client(int client, rpc_t *server);

/**
 *  Checking if an array is full.
 *
 *  @params:
 *    arr:      Array for checking its status.
 *    length:   Length of the array. 
 *  @return:    Returns 1 for full; 0 if not.
 */
int isfull(child_t arr[], int length);

/**
 *  Checking if an array is empty.
 *
 *  @params:
 *    arr:      Array for checking its status.
 *    length:   Length of the array. 
 *  @return:    Returns 1 for empty; 0 if not.
 */
int isempty(child_t arr[], int length);

/**
 *  Adding a client to the array of clients.
 *
 *  @params:
 *    arr:      Array for checking its status.
 *    length:   Length of the array.
 *    pid:      pid of the child process.
 *    clientfd: File descriptor for the new connection which we are adding. 
 *  @return:    adds the client to the array; returns nothing.
 */
void addclient(child_t arr[], int length, pid_t pid, int clientfd);

/**
 *  Removing a client to the array of clients.
 *
 *  @params:
 *    arr:      Array for checking its status.
 *    length:   Length of the array.
 *    pid:      pid of the child process.
 *  @return:    Returns the file descriptor for the client.
 */
int removeclientPID(child_t arr[], int length, pid_t pid);

#endif