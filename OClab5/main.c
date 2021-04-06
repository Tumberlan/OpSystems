
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#define BUF_SIZE 15


typedef struct t_e{
    int offset;
    int len;
}table_elem;

table_elem* make_table_elem(int os, int l){
    table_elem* new = malloc(sizeof (table_elem));
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

    table_elem* tmp = malloc(sizeof (table_elem));
    T->array = tmp;
    T->arr_len = 1;
    T->curr_len = 0;
    return T;
}

void add_elem_to_table(table* T,table_elem* T_elem){
    if(T->curr_len+1 > T->arr_len){
        T->arr_len = T->arr_len*2;
        table_elem* tmp = malloc(sizeof (table_elem)*T->arr_len);
        for(int i = 0; i < T->curr_len; i++){
            tmp[i] = T->array[i];
        }
        free(T->array);
        T->array = tmp;
    }
    T->array[T->curr_len] = *T_elem;
    T->curr_len++;
}

void print_table(table* T){
    for(int i = 0; i < T->curr_len; i++){
        printf("%d offset: %d strlen: %d\n",i+1, T->array[i].offset, T->array[i].len);
    }
}

void print_file(table* T, int fd){
    print_table(T);
    for(int i = 0; i < T->curr_len; i++){
        if(lseek(fd, T->array[i].offset, 0) == -1L){
            perror("Seek error");
            exit(2);
        }
        char *buf = malloc(sizeof (char)*T->array[i].len);
        read(fd, buf, T->array[i].len);
        for(int j = 0; j < T->array[i].len;j++){
            printf("%c",buf[j]);
        }
        printf("\n");
        free(buf);
    }
}


int main() {

    char filename[BUF_SIZE];
    scanf("%s", filename);
    //printf("%s", filename);
    //printf("%d", (int)strlen(filename));

    int fd;
    if((fd = open(filename,O_RDONLY)) == -1){
        perror("can't open file");
        exit(1);
    }

    char *buffer = malloc(sizeof (char)*BUF_SIZE);

    int offset = 0;
    if(lseek(fd,offset,0) == -1L){
        perror("Seek error");
        exit(2);
    }

    if(read(fd, buffer, BUF_SIZE) == 0){
        printf("empty file\n");
        return 0;
    }


    table* my_table = init_table();
    char printing_symbol;
    int tmp_offset = 0;
    int tmp_len = 0;
    int i = 0;
    while(i < strlen(buffer)){
        printing_symbol = buffer[i];
        if(printing_symbol == EOF){
            add_elem_to_table(my_table, make_table_elem(tmp_offset,tmp_len));
            break;
        }
        if(printing_symbol == '\n'){
            add_elem_to_table(my_table, make_table_elem(tmp_offset,tmp_len));
            tmp_offset += tmp_len+1;
            tmp_len = 0;
        }else{
            tmp_len++;
        }
        i++;

        if(i == strlen(buffer)){
            offset += strlen(buffer);
            if(lseek(fd,offset,0) == -1L){
                perror("Seek error");
                exit(2);
            }

            if(read(fd, buffer, BUF_SIZE) == 0){
                break;
            }
            i = 0;
        }
    }
    //print_table(my_table);

    print_file(my_table,fd);




    return 0;
}
