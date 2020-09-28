#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include "a1_lib.h"

#define BUFSIZE   1024
#define MAXCLIENTS 5


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
  int sockfd, clientfd;
  char msg[BUFSIZE];
  const char *greeting = "hello, world\n";
  int running = 1;
  int connection = 0;
  // table holding all the clients
  int clients[MAXCLIENTS];
  int pos;
  int ret;
  pid_t childPID;
  int terminate = 0;

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
      // start the child
      if((childPID = fork()) == 0){
        // Should have the number of clients
        // In the child
        // Close the socket
        close(sockfd);
        int tb;
        // Now listen for input
        while (strcmp(msg, "quit\n") && strcmp(msg, "terminate backend\n")) {
          memset(msg, 0, sizeof(msg));
          ssize_t byte_count = recv_message(clientfd, msg, BUFSIZE);
          if (byte_count <= 0) {
            break;
          }
          printf("Client %d: %s\n", getpid(), msg);
          send_message(clientfd, greeting, strlen(greeting));
          
        }
        if (strcmp(msg, "terminate backend\n"))
          return -1;
        return 1;
      }
      else{
        // here childPID is set to the pid of the nwly created process
        addClient(clients, MAXCLIENTS, childPID);
      }
    }else{
      printf("Max clients!");
      // force a hang here!
      ret = wait(&rval);
      if ((pos = position(clients, MAXCLIENTS, ret)) >= 0){
        // problem: We have to wait for the parent to get freed from listening for a connection to get a returned value
        // a child has exited and we need to reduce the counter
        printf("Process %d has exited\n", ret );
        terminate = WEXITSTATUS(rval);
        removeClient(clients, MAXCLIENTS, pos);
      }
    }
    // check if any children are done
    for (int i = 0; i<MAXCLIENTS; i++){
      if (clients[i]>0){
        pid_t find = clients[i];
        ret = waitpid(find, &rval, WNOHANG);
        if(ret==clients[i]){
          printf("Process %d has exited with ret=%d\n", clients[i], ret);
          removeClient(clients, MAXCLIENTS, i);
          terminate = WEXITSTATUS(rval);
        }
      }
    }
    printf("array: ");
    for(int i =0; i<MAXCLIENTS; i++){
      printf("%d ", clients[i]);
    }
    printf("\n");
    //check if we need to terminate
    if (terminate == 1){
      for (int i = 0; i<MAXCLIENTS; i++){
      if (clients[i]>0){
        // Killing client i
        printf("Killing client %d", clients[i]);
        pid_t find = clients[i];
        kill(find, SIGKILL);
        removeClient(clients, MAXCLIENTS, i);
        }
      }
      // end the while loop
    running = running-terminate;
    }
  }
  close(clientfd);
  return 0;
}

