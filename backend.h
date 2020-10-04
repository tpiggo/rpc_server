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

// User defined functions
char ** parse_args(int length, char *args);
int accept_on_server_socket(rpc_t *serv);
int serv_client(int client, rpc_t *server);
int isfull(child_t arr[], int length);
int isempty(child_t arr[], int length);
void addclient(child_t arr[], int length, pid_t pid, int clientfd);
int removeclientPID(child_t arr[], int length, pid_t pid);

#endif