/*
* Student: Timothy Piggott
* StudentID: 260855765
* Date: 9/30/2020
*/

#include <stdint.h>
#include "rpc.h" 

#ifndef FRONTEND_H
#define FRONTEND_H

/**
 *  Connect to an RPC server.
 *
 *  @params:
 *    host:   Host IP as a string.
 *    port:   Integer number for which port to bind to.
 *  @return:    On success, a connection is set up between the server and
 *              client processes. The function returns the RPC server struct
 *              If an error occurs, the function will NULL.
 */
rpc_t *RPC_Connect(char * host, int port);

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
 *  Calls an RPC service.
 *
 *  @params:
 *    r:        Pointer to a RPC Server connection.
 *    name:     Name of the function which can be found on the backend.
 *    args:     Arguments as a string. 
 *  @return:    On success, sends a command request to the RPC server.
 *              The function NULL as the errors will be handled within the function.
 */
void RPC_Call(rpc_t *r, char* name, char * args);

/**
 *  Parses a line into an easily understandable structure for the backend to understand.
 *
 *  @params:
 *    input:    String of input from the client.
 *  @return:    Builds a client message which can be sent over the connection.
 *              As mentioned by the TA's, the backend does not have to expose the frontend
 *              to the functions. Therefore, this simply builds a struct with 2 strings
 *              which will make parsing easier on the backend.
 */
client_msg parse_line(char *input);


#endif //FRONTEND_H