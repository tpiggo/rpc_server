/*
* Student: Timothy Piggott
* StudentID: 260855765
* Date: 9/30/2020
*/

#include <stdint.h>
#include "rpc.h" 

#ifndef BACKEND_H
#define BACKEND_H

rpc_t *RPC_Init(char * host, int port);
void RPC_Close(rpc_t *r);
void RPC_Serve(rpc_t *r, char* name, char * args, int client);

int addInts(int x, int y);
int multiplyInts(int x, int y);
float divideFloats(float x, float y);
uint64_t factorial(int x);

#endif