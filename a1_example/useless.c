#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
    //*** THis doesnt work. Segfault at wraps.ptr
    int ret = atoi("3.6");
    printf("ret=%d", ret);
    return 0;

}