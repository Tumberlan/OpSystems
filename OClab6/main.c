#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <poll.h>

#define TO_THE_START 0
#define TO_THE_CURR 1
#define TO_THE_END 2
#define OPEN_FILE_FAIL 3
#define FILL_TABLE_FAIL 4
#define PRINT_LINE_FAIL 5
#define READ_ERROR 6
#define CLOSE_FILE_FAIL 7
#define NO_ERRORS 9
#define LSEEK_ERROR -1L
#define FILE_OPEN_READ_CLOSE_ERROR -1
#define ADD_TO_TABLE_ERROR 10
#define NO_MEMORY 11
#define NEW_LINE_SYMB '\n'
#define BREAK 12
#define FGETS_ERR -3
#define BUF_SIZE 15
#define POLL_ERROR -1
#define CONTINUE -2
#define TIMES_OUT 0
#define NO_READING 0
#define MAX_WAITING_TIME 5000
#define INPUT_NUMBER_ARRAY_LENGTH 10

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

int fill_table(table* T, int fd){
    char read_buffer[BUF_SIZE];
    int read_check;
    char printing_symbol;
    int offset = 0;
    int string_length = 0;
    while((read_check = read(fd, read_buffer, BUF_SIZE)) > 0){
        for(int i = 0; i < read_check;i++){
            printing_symbol = read_buffer[i];
            int push_result = push_symbol_to_table(printing_symbol, T,&offset,&string_length);
            if (push_result == BREAK){
                break;
            }
            if(push_result != NO_ERRORS){
                return push_result;
            }
        }
    }
    if (read_check == FILE_OPEN_READ_CLOSE_ERROR) {
        perror("read_error");
        return READ_ERROR;
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

void check(char* input, bool* skip, bool* skip_continue, bool* next_iter){
    if(strlen(input) >= INPUT_NUMBER_ARRAY_LENGTH-1 ){
        *skip = true;
        *skip_continue = true;
        *next_iter = true;
    }
}

int get_scanned_number_of_line(table* T){
    int number_of_line;
    char input[INPUT_NUMBER_ARRAY_LENGTH];
    char* fgets_check;
    bool skip = false;
    bool skip_continue;
    bool next_iter = false;
    do{
        next_iter = false;
        fgets_check = fgets(input, INPUT_NUMBER_ARRAY_LENGTH+1, stdin);
        if(strlen(input) < INPUT_NUMBER_ARRAY_LENGTH){
            skip_continue = false;
        }
        if(skip){
            skip = skip_continue;
            next_iter = true;
            continue;
        }
        if(fgets_check == NULL){
            perror("cannot open file");
            return FGETS_ERR;
        }
        if((int)input[0] == '\n'){
            continue;
        }
        if(!check_input(input)){
            perror("wrong arguments, please type 1 not negative number");
            check(input, &skip, &skip_continue, &next_iter);
            next_iter = true;
            continue;
        }
        number_of_line = atoi(input);
        if(number_of_line > T->current_length || number_of_line < 0){
            perror("unavailable line number, please enter another number");
        }
        check(input, &skip, &skip_continue, &next_iter);
    }while (number_of_line > T->current_length || number_of_line < 0 || next_iter);
    return number_of_line;
}


int print_file(table* T, int fd){
    int read_check;
    long lseek_check;
    for(int i = 0; i < T->current_length; i++){
        lseek_check = lseek(fd, T->array[i].offset, TO_THE_START);
        if(lseek_check == LSEEK_ERROR){
            perror("Seek error");
            return LSEEK_ERROR;
        }
        char string_buffer[T->array[i].length];
        read_check = read(fd, string_buffer, T->array[i].length);
        if(read_check == FILE_OPEN_READ_CLOSE_ERROR){
            perror("read_error");
            return READ_ERROR;
        }
        for(int j = 0; j < T->array[i].length;j++){
            printf("%c",string_buffer[j]);
        }
        printf("\n");
    }
    return NO_ERRORS;
}

int print_numbered_line(table* T, int fd) {
    long lseek_checker;
    int read_check;
    struct pollfd pfd = {0, POLLIN, 0};
    int number_of_line = 1;
    while (number_of_line != 0) {
        if(pfd.events == NO_READING){
            perror("can't read");
            return POLL_ERROR;
        }
        int poll_check = poll(&pfd, 1, MAX_WAITING_TIME);
        if (poll_check == POLL_ERROR) {
            perror("poll error");
            return POLL_ERROR;
        }
        if (poll_check == TIMES_OUT) {
            printf(" no input\n");
            int print_file_res = print_file(T, fd);
            return print_file_res;
        }
        number_of_line = get_scanned_number_of_line(T);
        if (number_of_line == FGETS_ERR) {
            return FGETS_ERR;
        }
        if(number_of_line == 0 || number_of_line == CONTINUE){
            continue;
        }
        number_of_line--;
        lseek_checker = lseek(fd, T->array[number_of_line].offset, TO_THE_START);
        if (lseek_checker == LSEEK_ERROR) {
            perror("Seek error");
            return LSEEK_ERROR;
        }
        char string_buffer[T->array[number_of_line].length];
        read_check = read(fd, string_buffer, T->array[number_of_line].length);
        if (read_check == FILE_OPEN_READ_CLOSE_ERROR) {
            perror("read_error");
            return READ_ERROR;
        }
        for (int j = 0; j < T->array[number_of_line].length; j++) {
            printf("%c", string_buffer[j]);
        }
        printf("\n");
        number_of_line++;
        if(pfd.revents){
            continue;
        }
        pfd.events = NO_READING;
    }
    return NO_ERRORS;

}

int main() {
    char filename[BUF_SIZE];
    int scanf_checker;
    do{
        scanf_checker = scanf("%s", filename);
        if(scanf_checker != 1){
            printf("wrong argument\n");
        }
    }while(scanf_checker != 1);
    int fd;
    int fd_checker = (fd = open(filename,O_RDONLY));
    if(fd_checker == FILE_OPEN_READ_CLOSE_ERROR){
        perror("can't open file");
        exit(OPEN_FILE_FAIL);
    }
    table* T = init_table();
    if(T == NULL){
        close(fd);
        exit(NO_MEMORY);
    }
    int table_error = fill_table(T, fd);
    if(table_error != NO_ERRORS){
        free(T);
        close(fd);
        exit(table_error);
    }
    int lines_print_error = print_numbered_line(T,fd);
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