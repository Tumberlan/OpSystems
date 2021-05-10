#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <poll.h>
#include <sys/mman.h>

#define TO_THE_START 0
#define TO_THE_CURR 1
#define TO_THE_END 2
#define OPEN_FILE_FAIL 3
#define FILL_TABLE_FAIL 4
#define PRINT_LINE_FAIL 5
#define CLOSE_FILE_FAIL 7
#define NO_ERRORS 9
#define FILE_OPEN_READ_CLOSE_ERROR -1
#define ADD_TO_TABLE_ERROR 10
#define NO_MEMORY 11
#define NEW_LINE_SYMB '\n'
#define BREAK 12
#define FGETS_ERR -3
#define POLL_ERROR -1
#define CONTINUE -2
#define TIMES_OUT 0
#define FSTAT_ERROR -1
#define MAP_ERROR 13
#define MAX_WAITING_TIME 5000


typedef struct t_e{
    int offset;
    int length;
}table_elem;

table_elem* make_table_elem(int os, int l){
    table_elem* new = malloc(sizeof (table_elem));
    if(new == NULL){
        perror("no memory for new table element");
        return NULL;
    }
    new->offset = os;
    new->length = l;
    return new;
}

typedef struct t{
    table_elem *array;
    int array_length;
    int current_length;
}table;

table* init_table(){
    table* T = malloc(sizeof(table));
    if(T == NULL){
        perror("no memory for new table");
        return NULL;
    }
    table_elem* tmp = malloc(sizeof (table_elem));
    if(tmp == NULL){
        perror("no memory for new table element");
        return NULL;
    }
    T->array = tmp;
    T->array_length = 1;
    T->current_length = 0;
    return T;
}

bool increase_table_capacity(table* T){
    T->array_length = T->array_length*2;
    table_elem* tmp = malloc(sizeof (table_elem)*T->array_length);
    if(tmp == NULL){
        perror("no memory for new table element");
        return false;
    }
    for(int i = 0; i < T->current_length; i++){
        tmp[i] = T->array[i];
    }
    free(T->array);
    T->array = tmp;
    return true;
}



bool add_elem_to_table(table* T,table_elem* T_elem){
    if(T_elem == NULL){
        return false;
    }
    bool no_mistakes_check = true;
    if(T->current_length+1 > T->array_length){
        no_mistakes_check = increase_table_capacity(T);
    }
    if(!no_mistakes_check){
        return false;
    }
    T->array[T->current_length] = *T_elem;
    T->current_length++;
    return true;
}



int push_symbol_to_table(char printing_symbol,table* T,int* offset, int* string_length){
    bool add_check = true;
    if(printing_symbol == EOF){
        add_check = add_elem_to_table(T, make_table_elem(*offset,*string_length));
    }
    if(add_check == false){
        return ADD_TO_TABLE_ERROR;
    }
    if(printing_symbol == EOF){
        return BREAK;
    }
    if(printing_symbol == NEW_LINE_SYMB){
        add_check =  add_elem_to_table(T, make_table_elem(*offset,*string_length));
        *offset += *string_length+1;

        *string_length = 0;
    }
    if(add_check == false){
        return ADD_TO_TABLE_ERROR;
    }
    if(printing_symbol != NEW_LINE_SYMB){
        (*string_length)++;
    }
    return NO_ERRORS;
}

int fill_table(table* T, char* fil_map, int fil_size){

    char printing_symbol;
    int offset = 0;
    int string_length = 0;

    for(int i = 0; i < fil_size; i++) {
        printing_symbol = fil_map[i];
        int push_result = push_symbol_to_table(printing_symbol, T, &offset, &string_length);
        if (push_result == BREAK) {
            break;
        }
        if (push_result != NO_ERRORS) {
            return push_result;
        }
    }
    return NO_ERRORS;
}


bool check_input(char* str){
    for(int i = 0; i < strlen(str); i++){
        if(!(((int)str[i] >= '0' && (int)str[i] <= '9')|| (int)str[i] == '\n')){
            return false;
        }
    }
    return true;
}

int get_scanned_number_of_line(table* T){
    int number_of_line;
    char input[10];
    char* fgets_check;

    do{
        fgets_check = fgets(input, 10, stdin);
        if(fgets_check == NULL){
            perror("cannot open file");
            return FGETS_ERR;
        }
        if((int)input[0] == '\n'){
            return CONTINUE;
        }
        if(!check_input(input)){
            perror("wrong arguments, please type 1 not negative number");
            number_of_line = -1;
            return CONTINUE;
        }
        number_of_line = atoi(input);
        if(number_of_line > T->current_length || number_of_line < 0){
            printf("unavailable line number, please enter another number\n");
            return CONTINUE;
        }
    }while (number_of_line > T->current_length || number_of_line < 0);

    return number_of_line;
}

int print_numbered_line(table* T, char* fil_map) {
    long lseek_checker;
    int read_check;
    struct pollfd pfd = {0, POLLIN, 0};
    int number_of_line = 1;
    while (number_of_line != 0) {
        int poll_check = poll(&pfd, 1, MAX_WAITING_TIME);
        if (poll_check == POLL_ERROR) {
            perror("lab6.out: poll error");
            return POLL_ERROR;
        }
        if (poll_check == TIMES_OUT) {
            printf("no input\n%s\n", fil_map);
            return NO_ERRORS;
        }
        number_of_line = get_scanned_number_of_line(T);
        if (number_of_line == FGETS_ERR) {
            return FGETS_ERR;
        }
        if(number_of_line == 0 || number_of_line == CONTINUE){
            continue;
        }
        number_of_line--;

        char string_buffer[T->array[number_of_line].length];
        memcpy(string_buffer, fil_map+T->array[number_of_line].offset, T->array[number_of_line].length);
        for (int j = 0; j < T->array[number_of_line].length; j++) {
            printf("%c", string_buffer[j]);
        }
        printf("\n");
        number_of_line++;
    }
    return NO_ERRORS;

}

int main() {
    int fd;
    int fd_checker = (fd = open("lab7.c",O_RDONLY));
    if(fd_checker == FILE_OPEN_READ_CLOSE_ERROR){
        perror("can't open file");
        exit(OPEN_FILE_FAIL);
    }
    struct stat fil_stat;
    int fil_size;
    int line_number;
    int fstat_res = fstat(fd, &fil_stat);
    if(fstat_res == FSTAT_ERROR){
        perror("can't get file statistic");
        close(fd);
        exit(FSTAT_ERROR);
    }
    fil_size = fil_stat.st_size;
    char* fil_map = (char*)mmap(0, fil_size, PROT_READ, MAP_SHARED, fd, 0);
    if(fil_map == MAP_FAILED){
        perror("failed mapping file");
        close(fd);
        exit(MAP_ERROR);
    }

    table* T = init_table();
    if(T == NULL){
        close(fd);
        exit(NO_MEMORY);
    }

    int table_error = fill_table(T,fil_map, fil_size);
    if(table_error != NO_ERRORS){
        free(T);
        close(fd);
        exit(table_error);
    }

    int lines_print_error = print_numbered_line(T,fil_map);
    if(lines_print_error != NO_ERRORS){
        free(T);
        close(fd);
        exit(lines_print_error);
    }
    free(T);
    int close_f_check = close(fd);
    if(close_f_check == FILE_OPEN_READ_CLOSE_ERROR){
        exit(CLOSE_FILE_FAIL);
    }
    return 0;
}