#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include "a1_lib.h"

#define BUFSIZE   1024
#define MAXCLIENTS 2


int position(int clients[], int length, int id){
  for(int i =0; i<length; i++){
    if (clients[i] == id){
      return i;
    }
  }
  return -1;
}

int numberOfClients(int clients[], int length){
  int tot = 0;
  for(int i = 0; i<length; i++){
    if(clients[i]>0)
      tot++;
  }
  return tot;
}
int isFull(int clients[], int length){
  // return if the number of clients takes up the whole space
  return numberOfClients(clients, length)<length;
}

int addClient(int clients[], int length, int id){
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
*   We are taking the original example server and adding the ability to add more than
*   one client access to it using fork.
*/
int main(void) {
  int sockfd, clientfd, childPID;
  char msg[BUFSIZE];
  const char *greeting = "hello, world\n";
  int running = 1;
  int connection = 0;
  // table holding all the clients
  int clients[MAXCLIENTS];
  int pos;
  int ret;

  if (create_server("0.0.0.0", 10000, &sockfd) < 0) {
    fprintf(stderr, "server main(): oh no\n");
    return -1;
  }
  for( int i = 0; i<MAXCLIENTS; i++){
    // fill the table with -1's indicating free space
    clients[i] = -1;
  }
  //Server has been created and is listening
  // While loop for listening!
  int rval;
  while(running){
    // Wait for new client connection
    // This blocks the parent on the connection waiting.
    // If there is no space left, do not even allow a connection
    if (isFull(clients, MAXCLIENTS)){
      // Only block if there is free number of clients
      if (accept_connection(sockfd, &clientfd) < 0) {
        fprintf(stderr, "whileloop: oh no\n");
        // kill the process
        return -1;
      }
      // using connection number to hold where we are
      connection++;
      addClient(clients, MAXCLIENTS, connection);
      // start the child
      if((childPID = fork()) == 0){
        // Should have the number of clients
        // In the child
        // Close the socket
        close(sockfd);
        // Now listen for input
        while (strcmp(msg, "quit\n")) {
          memset(msg, 0, sizeof(msg));
          ssize_t byte_count = recv_message(clientfd, msg, BUFSIZE);
          if (byte_count <= 0) {
            break;
          }
          printf("Client %d: %s\n", connection, msg);
          send_message(clientfd, greeting, strlen(greeting));
        }
        // kill the process here:
        return connection;
      }
    }else{
      printf("Max clients!");
      // force a hang here!
      ret = waitpid(-1, &rval, WSTOPPED);
      if ((pos = position(clients, MAXCLIENTS, WEXITSTATUS(rval))) >= 0){
        // problem: We have to wait for the parent to get freed from listening for a connection to get a returned value
        // a child has exited and we need to reduce the counter
        printf("Connection %d has exited\n", WEXITSTATUS(rval));
        removeClient(clients, MAXCLIENTS, pos);
      }

    }
    // check if any children are done
    ret = waitpid(-1, &rval, WNOHANG);
    if ((pos = position(clients, MAXCLIENTS, WEXITSTATUS(rval))) >= 0){
      // problem: We have to wait for the parent to get freed from listening for a connection to get a returned value
      // a child has exited and we need to reduce the counter
      printf("Connection %d has exited\n", WEXITSTATUS(rval));
      removeClient(clients, MAXCLIENTS, pos);
    }else{
      printf("array: ");
      for(int i =0; i<MAXCLIENTS; i++){
        printf("%d ", clients[i]);
      }
      printf("\n");
      printf("Going around! No client exited.\n");
    }
  }
  close(clientfd);
  return 0;
}

