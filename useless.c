
// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

#define BUF 64

typedef struct nested{
    char name[50];
    int size;
}nested;

typedef struct holder{
    int num;
    struct nested structure[2];
}holder;

typedef struct message{
    char returned[1024];
    int error;
    char error_m[1024];
}message;

void print2(char* hold){
    struct message* m = (struct message *)hold;
    printf("%s %d %s", m->returned, m->error, m->error_m);
}

void print(const char* hold){
    holder* y = (holder*) hold;
    printf("Holder length=%d\n", y->num);
    printf("Lines1: %s %d\n", y->structure[0].name, y->structure[0].size) ;
    printf("Lines2: %s %d\n", y->structure[1].name, y->structure[1].size );
    
}
char ** parse_args(int length, char args[]){
    char ** parsed = (char**)malloc(sizeof(char*)*length);
    for (int i = 0; i <length; i++){
        *(parsed+i) = (char*)malloc(sizeof(char));
    }
    // Fix the arguments
    char *token = strtok(args, " ");
    int i = 0;
    while(token != NULL && i<length){
        strcpy(*(parsed+i), token);
        token = strtok(NULL, " ");
        i++;
    } if (token!=NULL){
        printf("String too long for input!\n");
        return NULL;
    }
    return parsed;
}


  
int understand_process() 
{ 
    // We use two pipes 
    // First pipe to send input string from parent 
    // Second pipe to send concatenated string from child 
  
    int fd1[2];  // Used to store two ends of first pipe 
    int fd2[2];  // Used to store two ends of second pipe 
  
    char fixed_str[] = "forgeeks.org"; 
    char input_str[100]; 
    pid_t p; 
  
    if (pipe(fd1)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    if (pipe(fd2)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
  
    scanf("%s", input_str); 
    p = fork(); 
  
    if (p < 0) 
    { 
        fprintf(stderr, "fork Failed" ); 
        return 1; 
    } 
  
    // Parent process 
    else if (p > 0) 
    { 
        char concat_str[100]; 
  
        close(fd1[0]);  // Close reading end of first pipe 
  
        // Write input string and close writing end of first 
        // pipe. 
        write(fd1[1], input_str, strlen(input_str)+1); 
        close(fd1[1]); 
  
        // Wait for child to send a string 
        wait(NULL); 
  
        close(fd2[1]); // Close writing end of second pipe 
  
        // Read string from child, print it and close 
        // reading end. 
        read(fd2[0], concat_str, 100); 
        printf("Concatenated string %s\n", concat_str); 
        close(fd2[0]); 
    } 
  
    // child process 
    else
    { 
        close(fd1[1]);  // Close writing end of first pipe 
  
        // Read a string using first pipe 
        char concat_str[100]; 
        read(fd1[0], concat_str, 100); 
  
        // Concatenate a fixed string with it 
        int k = strlen(concat_str); 
        int i; 
        for (i=0; i<strlen(fixed_str); i++) 
            concat_str[k++] = fixed_str[i]; 
  
        concat_str[k] = '\0';   // string ends with '\0' 
  
        // Close both reading ends 
        close(fd1[0]); 
        close(fd2[0]); 
  
        // Write concatenated string and close writing end 
        write(fd2[1], concat_str, strlen(concat_str)+1); 
        close(fd2[1]); 
  
        exit(0); 
    } 
} 



int main(int argc, char* argv[]){
    int pipe_PC[2*2];
	int pipe_CP[2*2];
	char readmsg[BUF];
    
    /*
    //Create the pipe. If it fails; cancel!
	if(pipe(pipe_PC) < 0){
		printf("Can't create pipe_PC!\n");
		return 1;
	}
	if(pipe(pipe_CP) < 0){
		printf("Can't create pipe_PC!\n");
		return 1;
	}
    pid_t child;
    for (int i = 0; i<1; i++){
        child=fork();
        if (child==0){
            break;
        }
    }
    if (child==0){
        printf("Starting child");
        close(pipe_PC[1]);// close write from parent
        close(pipe_CP[0]);// close read from child
        read(pipe_PC[0], readmsg, BUF);
        printf("msg child got is=%s\n", readmsg);
        write(pipe_CP[1], "Hello Parent", BUF);
        exit(0);
    } else {
        close(pipe_PC[0]);// close write from child
        close(pipe_CP[1]);// close read from parent
        sleep(2);
        write(pipe_PC[1], "Hello child", BUF);
        read(pipe_CP[0], readmsg, BUF);
        printf("msg parent got is=%s\n", readmsg);
    }
    
    return 0;*/
    //Create the pipe. If it fails; cancel!
	for(int i = 0; i<2; i++){
        pipe(&pipe_PC[2*i]);
        pipe(&pipe_CP[2*i]);
    }
    pid_t child;
    int child_num;
    for (int i = 0; i < 2; i++){
		child = fork();
		if (child == 0){
			close(pipe_PC[2*i+1]); // close unwanted write side
			close(pipe_CP[2*i]); // close unwanted read side
			child_num = i;
			break;
		} else {
			close(pipe_PC[2*i]); // close unwanted read side
			close(pipe_CP[2*i+1]); // close unwanted write side
		}
	}

    if (child == 0){
		printf("I am %d child! Now wait for a message!\n", child_num);
		read(pipe_PC[2*child_num], readmsg, BUF);
		printf("Received Message: %s\n", readmsg);
        fflush(stdout);
		write(pipe_CP[2*child_num+1], "Clear as Day", BUF);
		printf("Dying now!\n");
        close(pipe_PC[child_num*2]);
        close(pipe_CP[child_num*2+1]);
		exit(0);
	} else{
        int k = 0;
        printf("Writing to children!\n");
        write(pipe_PC[1], "Hello First My child!", BUF);
        write(pipe_PC[3], "Hello Second My child!", BUF);
        read(pipe_CP[0], readmsg, BUF);
        printf("Child said 1: %s\n", readmsg);
        fflush(stdout);
        memset(readmsg, 0, BUF);
        read(pipe_CP[2], readmsg, BUF);
        printf("Child said 2: %s\n", readmsg);
        close(pipe_PC[1]);
        close(pipe_PC[3]);
        close(pipe_CP[0]);
        close(pipe_CP[2]);
    }
    return 0;
    
    //understand_process();

}