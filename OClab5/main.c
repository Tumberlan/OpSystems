#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define BUF_SIZE 15
#define END_OF_FILE 0
#define TO_THE_START 0
#define TO_THE_CURR 1
#define TO_THE_END 2
#define OPEN_FILE_FAIL 3
#define FILL_TABLE_FAIL 4
#define PRINT_LINE_FAIL 5
#define READ_ERROR 6
#define CLOSE_FILE_FAIL 7
#define ERROR_IN_INPUT_FILE 8
#define NO_ERRORS 9
#define LSEEK_ERROR -1L
#define FILE_OPEN_READ_CLOSE_ERROR -1
#define ADD_TO_TABLE_ERROR 10
#define NO_MEMORY 11
#define NEW_LINE_SYMB '\n'
#define BREAK 12


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

void free_table(table* T){
    for(int i = 0; i < T->current_length; i++){
        free(&T->array[i]);
    }
    free(T);
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



int push_symbol_to_table(char printing_symbol,table* T,int* offset,int* string_length){
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

int refill_read_buffer(int offset, char* buffer, long *lseek_check, int fd, int* read_check){
    *lseek_check = lseek(fd,offset,TO_THE_START);
    if(*lseek_check == LSEEK_ERROR){
        perror("Seek error");
        return ERROR_IN_INPUT_FILE;
    }
    *read_check = read(fd, buffer, BUF_SIZE);
    if(*read_check == END_OF_FILE){
        return BREAK;
    }
    if(*read_check == FILE_OPEN_READ_CLOSE_ERROR){
        perror("read_error");
        return READ_ERROR;
    }
    return NO_ERRORS;
}

int fill_table(table* T, int fd){
    char read_buffer[BUF_SIZE+1];
    int read_buffer_offset = 0;
    long lseek_check;
    int read_check;
    int refill_result = refill_read_buffer(read_buffer_offset, read_buffer, &lseek_check, fd, &read_check);
    if(refill_result == BREAK){
        printf("empty file\n");
        return ERROR_IN_INPUT_FILE;
    }
    if(refill_result != NO_ERRORS){
        return refill_result;
    }
    char printing_symbol;
    int offset = 0;
    int string_length = 0;
    int i = 0;
    while(i < strlen(read_buffer)){
        printing_symbol = read_buffer[i];
        int push_result = push_symbol_to_table(printing_symbol, T, &offset, &string_length);
        if (push_result == BREAK){
            break;
        }
        if(push_result != NO_ERRORS){
            return push_result;
        }
        i++;
        int end_of_read_buffer_check = (i == strlen(read_buffer));
        if(end_of_read_buffer_check){
            read_buffer_offset+= strlen(read_buffer);
            refill_result = refill_read_buffer(read_buffer_offset, read_buffer, &lseek_check, fd, &read_check);
            i = 0;
        }
        if(end_of_read_buffer_check && refill_result == BREAK){
            break;
        }
        if(end_of_read_buffer_check && refill_result != NO_ERRORS){
            return refill_result;
        }
    }
    return NO_ERRORS;
}


void print_table(table* T){
    for(int i = 0; i < T->current_length; i++){
        printf("%d offset: %d strlen: %d\n",i+1, T->array[i].offset, T->array[i].length);
    }
}

int get_scanned_number_of_line(table* T){
    int number_of_line;
    int scanf_checker;
    do{
        scanf_checker = scanf("%d", &number_of_line);
        fflush(stdin);
        if(scanf_checker != 1){
            perror("wrong argument type");
            number_of_line = -1;
            continue;
        }
        if(number_of_line > T->current_length || number_of_line < 0){
            printf("unavailable line number, please enter another number\n");
        }
    } while (number_of_line > T->current_length || number_of_line < 0);
    return number_of_line;
}


int print_numbered_line(table* T, int fd){
    long lseek_checker;
    int read_check;
    int number_of_line = get_scanned_number_of_line(T);
    while (number_of_line != 0) {
        number_of_line--;
        lseek_checker = lseek(fd, T->array[number_of_line].offset, TO_THE_START);
        if (lseek_checker == LSEEK_ERROR) {
            perror("Seek error");
            return LSEEK_ERROR;
        }
        char string_buffer[T->array[number_of_line].length];
        read_check = read(fd, string_buffer, T->array[number_of_line].length);
        if(read_check == FILE_OPEN_READ_CLOSE_ERROR){
            perror("read_error");
            return READ_ERROR;
        }
        for (int j = 0; j < T->array[number_of_line].length; j++) {
            printf("%c", string_buffer[j]);
        }
        printf("\n");
        number_of_line = get_scanned_number_of_line(T);
    }
    return NO_ERRORS;
}

int print_file(table* T, int fd){
    print_table(T);
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
        exit(NO_MEMORY);
    }
    int table_error = fill_table(T, fd);
    if(table_error != NO_ERRORS){
        exit(table_error);
    }
    int lines_print_error = print_numbered_line(T,fd);
    if(lines_print_error != NO_ERRORS){
        exit(lines_print_error);
    }
    free_table(T);
    int close_f_check = close(fd);
    if(close_f_check == FILE_OPEN_READ_CLOSE_ERROR){
        exit(CLOSE_FILE_FAIL);
    }
    return 0;
}