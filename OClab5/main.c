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


typedef struct t_e{
    int offset;
    int len;
}table_elem;

table_elem* make_table_elem(int os, int l){
    table_elem* new = malloc(sizeof (table_elem));
    if(new == NULL){
        perror("no memory for new table element");
        return NULL;
    }
    new->offset = os;
    new->len = l;
    return new;
}

typedef struct t{
    table_elem *array;
    int arr_len;
    int curr_len;
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
    T->arr_len = 1;
    T->curr_len = 0;
    return T;
}

void free_table(table* T){
    for(int i = 0; i < T->curr_len; i++){
        free(&T->array[i]);
    }
    free(T);
}
bool add_elem_to_table(table* T,table_elem* T_elem){
    if(T_elem == NULL){
        return false;
    }
    if(T->curr_len+1 > T->arr_len){
        T->arr_len = T->arr_len*2;
        table_elem* tmp = malloc(sizeof (table_elem)*T->arr_len);
        if(tmp == NULL){
            perror("no memory for new table element");
            return false;
        }
        for(int i = 0; i < T->curr_len; i++){
            tmp[i] = T->array[i];
        }
        free(T->array);
        T->array = tmp;
    }
    T->array[T->curr_len] = *T_elem;
    T->curr_len++;
    return true;
}

int fill_table(table* T, int fd){
    char buffer[BUF_SIZE+1];
    int offset = 0;
    long lseek_check = lseek(fd,offset,TO_THE_START);
    if(lseek_check == LSEEK_ERROR){
        perror("Seek error");
        return ERROR_IN_INPUT_FILE;
    }
    int read_check = read(fd, buffer, BUF_SIZE);
    if(read_check == END_OF_FILE){
        printf("empty file\n");
        return ERROR_IN_INPUT_FILE;
    }else if(read_check == FILE_OPEN_READ_CLOSE_ERROR){
        perror("read_error");
        return READ_ERROR;
    }
    char printing_symbol;
    int tmp_offset = 0;
    int tmp_len = 0;
    int i = 0;
    while(i < strlen(buffer)){
        printing_symbol = buffer[i];
        if(printing_symbol == EOF){
            bool add_check = add_elem_to_table(T, make_table_elem(tmp_offset,tmp_len));
            if(add_check == false){
                return ADD_TO_TABLE_ERROR;
            }
            break;
        }
        if(printing_symbol == NEW_LINE_SYMB){
            bool add_check =  add_elem_to_table(T, make_table_elem(tmp_offset,tmp_len));
            if(add_check == false){
                return ADD_TO_TABLE_ERROR;
            }
            tmp_offset += tmp_len+1;
            tmp_len = 0;
        }else{
            tmp_len++;
        }
        i++;

        if(i == strlen(buffer)){
            offset += strlen(buffer);
            lseek_check = lseek(fd,offset,TO_THE_START);
            if(lseek_check == LSEEK_ERROR){
                perror("Seek error");
                return ERROR_IN_INPUT_FILE;
            }

            read_check = read(fd, buffer, BUF_SIZE);
            if(read_check == END_OF_FILE){
                break;
            }else if(read_check == FILE_OPEN_READ_CLOSE_ERROR){
                perror("read_error");
                return READ_ERROR;
            }
            i = 0;
        }
    }
    return NO_ERRORS;
}


void print_table(table* T){
    for(int i = 0; i < T->curr_len; i++){
        printf("%d offset: %d strlen: %d\n",i+1, T->array[i].offset, T->array[i].len);
    }
}

int print_numbered_line(table* T, int fd){
    int number;
    int scanf_checker;
    long lseek_checker;
    int read_check;
    do{
        scanf_checker = scanf("%d", &number);
        fflush(stdin);
        if(scanf_checker != 1){
            printf("wrong argument\n");
            number = -1;
        }else if(number > T->curr_len || number < 0){
            printf("unavailable line number, please enter another number\n");
        }
    } while (number > T->curr_len || number < 0);
    while (number != 0) {
        number--;
        lseek_checker = lseek(fd, T->array[number].offset, TO_THE_START);
        if (lseek_checker == LSEEK_ERROR) {
            perror("Seek error");
            return LSEEK_ERROR;
        }
        char buf[T->array[number].len];
        read_check = read(fd, buf, T->array[number].len);
        if(read_check == FILE_OPEN_READ_CLOSE_ERROR){
            perror("read_error");
            return READ_ERROR;
        }
        for (int j = 0; j < T->array[number].len; j++) {
            printf("%c", buf[j]);
        }
        printf("\n");
        do{
            scanf_checker = scanf("%d", &number);
            fflush(stdin);
            if(scanf_checker != 1){
                printf("wrong argument\n");
                number = -1;
            }else if(number > T->curr_len || number < 0){
                printf("unavailable line number, please enter another number\n");
            }
        } while (number > T->curr_len || number < 0);
    }
    return NO_ERRORS;
}

int print_file(table* T, int fd){
    print_table(T);
    int read_check;
    long lseek_check;
    for(int i = 0; i < T->curr_len; i++){
        lseek_check = lseek(fd, T->array[i].offset, TO_THE_START);
        if(lseek_check == LSEEK_ERROR){
            perror("Seek error");
            return LSEEK_ERROR;
        }
        char buf[T->array[i].len];
        read_check = read(fd, buf, T->array[i].len);
        if(read_check == FILE_OPEN_READ_CLOSE_ERROR){
            perror("read_error");
            return READ_ERROR;
        }
        for(int j = 0; j < T->array[i].len;j++){
            printf("%c",buf[j]);
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

    table* my_table = init_table();
    if(my_table == NULL){
        exit(NO_MEMORY);
    }
    int table_error = fill_table(my_table, fd);
    if(table_error != NO_ERRORS){
        exit(table_error);
    }

    int lines_print_error = print_numbered_line(my_table,fd);
    if(lines_print_error != NO_ERRORS){
        exit(lines_print_error);
    }

    free_table(my_table);
    int close_f_check = close(fd);
    if(close_f_check == FILE_OPEN_READ_CLOSE_ERROR){
        exit(CLOSE_FILE_FAIL);
    }
    return 0;
}