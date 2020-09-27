typedef void (*callback)(void);
void register_callback(callback p_reg);

typedef int (*callback_int_t)(int,int);
void register_callback_int(callback_int_t cit);
int run_callit(int a, int b );

typedef struct callback_fun_t{
    char name[30];
    callback_int_t func;
} callback_fun_t;

char * register_callback_func(callback_int_t cit, char *name);
int run_func(int a, int b);
char * register_callback_func_arr(callback_int_t cit, char *name);
int arr_func(char *name, int a, int b);
