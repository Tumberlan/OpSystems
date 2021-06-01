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
#define MUNMAP_ERROR -1
#define MAP_ERROR 13
#define BUF_SIZE 15
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

bool increase_table_capacity(table* my_table){
    my_table->array_length = my_table->array_length*2;
    table_elem* tmp = malloc(sizeof (table_elem)*my_table->array_length);
    if(tmp == NULL){
        perror("no memory for new table element");
        return false;
    }
    for(int i = 0; i < my_table->current_length; i++){
        tmp[i] = my_table->array[i];
    }
    free(my_table->array);
    my_table->array = tmp;
    return true;
}



bool add_elem_to_table(table* my_table,table_elem* table_elem){
    if(table_elem == NULL){
        return false;
    }
    bool no_mistakes_check = true;
    if(my_table->current_length+1 > my_table->array_length){
        no_mistakes_check = increase_table_capacity(my_table);
    }
    if(!no_mistakes_check){
        return false;
    }
    my_table->array[my_table->current_length] = *table_elem;
    my_table->current_length++;
    return true;
}



int push_symbol_to_table(char printing_symbol,table* my_table,int* offset, int* string_length){
    bool add_check = true;
    if(printing_symbol == EOF){
        add_check = add_elem_to_table(my_table, make_table_elem(*offset,*string_length));
    }
    if(add_check == false){
        return ADD_TO_TABLE_ERROR;
    }
    if(printing_symbol == EOF){
        return BREAK;
    }
    if(printing_symbol == NEW_LINE_SYMB){
        add_check =  add_elem_to_table(my_table, make_table_elem(*offset,*string_length));
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

int fill_table(table* T, char* file_map, int file_size){

    char printing_symbol;
    int offset = 0;
    int string_length = 0;

    for(int i = 0; i < file_size; i++) {
        printing_symbol = file_map[i];
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



int print_numbered_line(table* my_table, char* file_map) {
    int read_check;
    struct pollfd pfd = {0, POLLIN, 0};
    int number_of_line = 1;
    while (number_of_line != 0) {
        int poll_check = poll(&pfd, 1, MAX_WAITING_TIME);
        if (poll_check == POLL_ERROR) {
            perror("poll error");
            return POLL_ERROR;
        }
        if (poll_check == TIMES_OUT) {
            printf(" no input\n%s\n", file_map);
            return NO_ERRORS;
        }
        bool is_continue;
        if(pfd.revents == POLLERR){
            perror("mistake on your device");
            return POLL_ERROR;
        }
        if(pfd.revents == POLLHUP){
            perror("device connection problems");
            return POLL_ERROR;
        }
        if(pfd.revents == POLLNVAL){
            perror("no file descriptor match");
            return POLL_ERROR;
        }
        if(pfd.revents) {
            number_of_line = get_scanned_number_of_line(my_table);
            is_continue = true;
        }
        if(!is_continue){
            perror("no answer");
            return POLL_ERROR;
        }
        if (number_of_line == FGETS_ERR) {
            return FGETS_ERR;
        }
        if(number_of_line == 0 || number_of_line == CONTINUE){
            continue;
        }
        number_of_line--;

        char string_buffer[my_table->array[number_of_line].length];
        memcpy(string_buffer, file_map+my_table->array[number_of_line].offset, my_table->array[number_of_line].length);
        for (int j = 0; j < my_table->array[number_of_line].length; j++) {
            printf("%c", string_buffer[j]);
        }
        printf("\n");
        number_of_line++;
        pfd.revents = 0;
    }
    return NO_ERRORS;
}


int close_file_function(int file_des){
    int close_f_check = close(file_des);
    if(close_f_check == FILE_OPEN_READ_CLOSE_ERROR) {
        perror("error in closing file");
        return CLOSE_FILE_FAIL;
    }
    return NO_ERRORS;
}

int unmapping_function(char* file_map, int file_size, int file_des){
    int munmap_result = munmap(file_map, file_size);
    if (munmap_result == MUNMAP_ERROR){
        perror("munmap error");
        int close_file_res = close_file_function(file_des);
        if(close_file_res != NO_ERRORS){
            return close_file_res;
        }
        return (MUNMAP_ERROR);
    }
    return NO_ERRORS;
}
void take_file_name(char* filename){
    int scanf_checker;
    do{
        scanf_checker = scanf("%s", filename);
        if(scanf_checker != 1){
            printf("wrong argument\n");
        }
    }while(scanf_checker != 1);

}

int get_file_stat(int* file_des, struct stat* file_stat){
    char filename[BUF_SIZE];
    take_file_name(filename);
    int fd_checker = (*file_des = open(filename,O_RDONLY));
    if(fd_checker == FILE_OPEN_READ_CLOSE_ERROR){
        perror("can't open file");
        return (OPEN_FILE_FAIL);
    }
    int fstat_res = fstat(*file_des, file_stat);
    if(fstat_res == FSTAT_ERROR){
        perror("can't get file statistic");
        int close_f_check = close_file_function(*file_des);
        if(close_f_check != NO_ERRORS){
            return (close_f_check);
        }
        return (FSTAT_ERROR);
    }
    return NO_ERRORS;
}

int make_table(char* file_map, table* my_table, int file_size, int file_des){
    int close_f_check;

    int table_error = fill_table(my_table,file_map, file_size);
    if(table_error != NO_ERRORS){
        free(my_table);
        int unmapping_result = unmapping_function(file_map,file_size,file_des);
        if(unmapping_result != NO_ERRORS){
            return (unmapping_result);
        }
        close_f_check = close_file_function(file_des);
        if(close_f_check != NO_ERRORS){
            return (close_f_check);
        }
        return (table_error);
    }
    return NO_ERRORS;
}

int printing_lines(table* my_table, char* file_map, int file_des, int file_size){
    int lines_print_error = print_numbered_line(my_table,file_map);
    if(lines_print_error != NO_ERRORS){
        free(my_table);
        int unmapping_result = unmapping_function(file_map,file_size,file_des);
        if(unmapping_result != NO_ERRORS){
            return (unmapping_result);
        }
        int close_f_check = close_file_function(file_des);
        if(close_f_check != NO_ERRORS){
            return (close_f_check);
        }
        return (lines_print_error);
    }
}

int main() {
    int close_f_check;
    int scanf_checker;
    int unmapping_result;
    int file_des;
    struct stat file_stat;
    int file_size;
    int get_file_stat_res = get_file_stat(&file_des, &file_stat);
    if(get_file_stat_res != NO_ERRORS){
        exit(NO_ERRORS);
    }
    file_size = file_stat.st_size;
    char* file_map = (char*)mmap(0, file_size, PROT_READ, MAP_SHARED, file_des, 0);
    if(file_map == MAP_FAILED){
        perror("failed mapping file");
        close_f_check = close_file_function(file_des);
        if(close_f_check != NO_ERRORS){
            exit(close_f_check);
        }
        exit(MAP_ERROR);
    }
    table* my_table = init_table();
    if(my_table == NULL){
        unmapping_result = unmapping_function(file_map,file_size,file_des);
        if(unmapping_result != NO_ERRORS){
            exit(unmapping_result);
        }
        close_f_check = close_file_function(file_des);
        if(close_f_check != NO_ERRORS){
            exit(close_f_check);
        }
        exit(NO_MEMORY);
    }
    int make_table_result = make_table(file_map, my_table, file_size, file_des);
    if(make_table_result != NO_ERRORS){
        exit(make_table_result);
    }
    int printing_lines_result = printing_lines(my_table, file_map,file_des,file_size);
    if(printing_lines_result != NO_ERRORS){
        exit(printing_lines_result);
    }
    free(my_table);
    unmapping_result = unmapping_function(file_map,file_size,file_des);
    if(unmapping_result != NO_ERRORS){
        exit(unmapping_result);
    }
    close_f_check = close_file_function(file_des);
    if(close_f_check != NO_ERRORS){
        exit(close_f_check);
    }
    return 0;
}