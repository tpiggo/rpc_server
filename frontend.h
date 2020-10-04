/*
* Student: Timothy Piggott
* StudentID: 260855765
* Date: 9/30/2020
*/

#include <stdint.h>
#include "rpc.h" 

#ifndef FRONTEND_H
#define FRONTEND_H

rpc_t *RPC_Connect(char * host, int port);
void RPC_Close(rpc_t *r);
void RPC_Call(rpc_t *r, char* name, char * args);
client_msg parse_line(char *input);


#endif //FRONTEND_H