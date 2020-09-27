#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reg_callback.h"

void register_callback(callback p_reg){
    printf("In the register function\n");
    (*p_reg)();
}
callback_int_t callit;

void register_callback_int(callback_int_t cit){
    printf("Registering int return\n");
    callit = cit;
}

int run_callit(int a, int b){
    return (*callit)(a,b);
}

callback_fun_t wrapper;
// wraper array to hold the functions built
int maxSize = 5;
int size = 0;
callback_fun_t array[5];

char * register_callback_func(callback_int_t cit, char *name){
    printf("Registering cit with name function return\n");
    memcpy(&(wrapper.name), &name, sizeof(struct callback_fun_t));
    wrapper.func = cit;
    return NULL;
}

char* register_callback_func_arr(callback_int_t cit, char *name){
    if (size+1 == maxSize){
        return "No space!";
    }
    // add wrapped function
    memcpy(&(array[size].name), &name, sizeof(struct callback_fun_t));
    array[size].func = cit;
    // increase size!
    size++;
    
    return NULL;
}

int arr_func(char *name, int a, int b){
    for (int i = 0; i < size; i++){
        if (strcmp(array[i].name, name)){
            return (*(array[i].func))(a,b);
        }
    }
    // indicating not found!
    return -1;
}

int run_func(int a, int b){
    if (wrapper.func == NULL){
        return -1;
    }
    return (*(wrapper.func))(a,b);
}

