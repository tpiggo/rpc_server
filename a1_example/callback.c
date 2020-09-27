#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "reg_callback.h"

int add(int a, int b){
    return a+b;
}

void my_call(void){
    printf("Inside my_call\n");
}

int main(){
    callback ptr_m = my_call;
    callback_int_t ptr_cit = add;
    char * ret = register_callback_func_arr(ptr_cit, "add");
    if (ret == NULL){
        int sum = arr_func("add", 1,3);
        printf("Sum: %d\n", sum);
    }else{
        printf("Error: %s", ret);
    }

    return 0;
}