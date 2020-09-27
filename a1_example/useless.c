#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int addab(int a, int b){
    return a+b;
}
int add12(){
    return 1+2;
}
typedef struct Function_wrap{
    char name[30];
    int (*ptr)();
} Function_wrap;


int register_callback(){
    return 0;
}

int main(){
    //*** THis doesnt work. Segfault at wraps.ptr
    Function_wrap wraps;
    memcpy(&(wraps.name), "add", sizeof(struct Function_wrap));
    wraps.ptr=addab;
    int sum = wraps.ptr(1,2);
    printf("sum: %d", sum);
    free(wraps.ptr);
    return 0;

}