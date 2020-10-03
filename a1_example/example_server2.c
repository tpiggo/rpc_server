#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include "a1_lib.h"
#include <fcntl.h>
#include <errno.h>

#define BUFSIZE   1024
#define MAXCLIENTS 5
typedef struct child_t{
	int childPID;
	int clientfd;
}child_t;
const char *greeting = "hello, world\n";

int handleclients(int clientfd){
	char msg[BUFSIZE];
	int out = 0;
	while (1) {
		memset(msg, 0, sizeof(msg));
		ssize_t byte_count = recv_message(clientfd, msg, BUFSIZE);
		if (byte_count <= 0) {
			out = 1;
			break;
		} else if (strcmp(msg, "quit\n") == 0){
			out = -1;
			break;
		} else if (strcmp(msg, "end\n") == 0){
			printf("Here.");
			out = 1;
			break;
		}
		printf("Client: %s\n", msg);
		send_message(clientfd, greeting, strlen(greeting));
	}
	send_message(clientfd, "Goodbye!", BUFSIZE);
	return out;
}


int isempty(child_t arr[], int length){
	for(int i= 0; i < length; i++){
		if (arr[i].childPID >= 0){
			return 0;
		}
	}
	return 1;
}

int isfull(child_t arr[], int length){
	for(int i= 0; i < length; i++){
		if (arr[i].childPID == -1){
			return 0;
		}
	}
	return 1;
}

void addclient(child_t arr[], int length, pid_t pid, int clientfd){
	for (int i = 0; i < length; i++ ){
		if (arr[i].childPID == -1){
			arr[i].clientfd = clientfd;
			arr[i].childPID = pid;
			return;
		}
	}
}

int removeclientPID(child_t arr[], int length, pid_t pid){
	for (int i = 0; i < length; i++ ){
		if (arr[i].childPID == pid){
			int hold = arr[i].clientfd;
			arr[i].clientfd = -1;
			arr[i].childPID = -1;
			return hold;
		}
	}
	return 0;
}


int main(void) {
	int sockfd, clientfd;
	char msg[BUFSIZE];
	int running = 1, ret, retstat;
	pid_t child;

	child_t clients[MAXCLIENTS];
	for (int i =0; i<MAXCLIENTS; i++){
		clients[i].childPID = -1;
		clients[i].clientfd = -1;
	}

	if (create_server("127.0.0.1", 8080, &sockfd) < 0) {
		fprintf(stderr, "oh no\n");
		return -1;
	}
	
	int no_shutdown = 1;

	while (1){
		printf("Arr: ");
		for (int i =0; i<MAXCLIENTS; i++){
			printf(" %d;%d ", clients[i].childPID, clients[i].clientfd );
		}
		printf("\n");

		int stat = accept_connection(sockfd, &clientfd);
		if (stat < 0){
			return -1;
		}
		printf("checking conditions\n");
		if (isfull(clients, MAXCLIENTS)){
			ret = wait(NULL);
			if ( WIFEXITED(retstat) && ret ){
				ret = removeclientPID(clients, MAXCLIENTS, ret);
				if (WEXITSTATUS(retstat) == 1){
					no_shutdown = 0;
				}
				// close clientfd
				close(ret);
			}
		}

		// Not mutually exclusive. Check again.
		if (isempty(clients, MAXCLIENTS) == 0){
			for(int i = 0; i < MAXCLIENTS; i++){
				ret = waitpid(clients[i].childPID, &retstat, WNOHANG);
				if ( WIFEXITED(retstat) && ret > 0){
					// get bakc the client fd
					ret = removeclientPID(clients, MAXCLIENTS, ret);
					if (WEXITSTATUS(retstat) == 255){
						no_shutdown = 0;
					}
					printf("Closing connection with %d\n", ret);
					// close clientfd
					close(ret);
				}
			}
		}
		
		// Make sure the child was not told to terminate backend prior to forking
		if (!no_shutdown){
			// Killing new client bc not handling
			close(clientfd);
			break;
		}

		if ((child = fork())==0){
			close(sockfd);
			int ret2 = handleclients(clientfd);
			printf("Here! You've ended or quit with return status %d\n", ret2);
			close(clientfd);
			return ret2;
		} else{
			// Just add the client to the list of children
			addclient(clients, MAXCLIENTS, child, clientfd);
		}
	}
	printf("Exited Main Loop\n");
	while(isempty(clients, MAXCLIENTS) == 0){
		ret = waitpid(-1, &retstat, WNOHANG);
		if ( WIFEXITED(retstat) && ret > 0){
			// get bakc the client fd
			ret = removeclientPID(clients, MAXCLIENTS, ret);
			printf("Closing connection with %d\n", ret);
			// close clientfd
			close(ret);
		}
	}
	printf("\n\nArr: ");
	for (int i =0; i<MAXCLIENTS; i++){
		printf(" %d;%d ", clients[i].childPID, clients[i].clientfd );
	}
	printf("\n");
	printf("isEmpty: %d", isempty(clients, MAXCLIENTS));

	return 0;
}

