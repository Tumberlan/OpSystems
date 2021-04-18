#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#define MAX_SIZE 150
#define INIT_MEMORY_FAULT 1
#define ADD_MEMORY_FAULT 2
#define PRINT_FAULT 3
#define EVERYTHING_OK 4
// создаем структуру list, которая содержит строку и указатель на следующий элемент нашего списка

struct list{
    char* string;
    int len;
    struct list* next;
};

// функция инициализации списка, выделяем память под строку, которую хотим вставить, вставляем её, т.к. у нас список строится
// от 1 элемента на данный момент, указатель на последующий элемент = NULL, т.е. наш вставленный элемент одновременно
// и голова списка и его конец
struct list* init_list(char* str, int l){
    struct list* lst = (struct list*)malloc(sizeof (struct list));
    if(lst == NULL){
        perror("no memory for new list");
        free(str);
        return NULL;
    }
    lst->string = str;
    lst->len = l;
    lst->next = NULL;
    return lst;
}


void list_free(struct list* lst){
    if(lst == NULL){
        return;
    }
    struct list *next = lst->next;
    while (lst->next != NULL) {
        free(lst->string);
        free(lst);
        lst = next;
        next = lst->next;
    }
    free(lst->string);
    free(lst);

}


// функция добавления элемента в список, элемент добавляется в конец списка(создаем объект типа struct list, на основе аргумента str,
// у него поле next = NULL, к последнему элементу списка вместо указателя NULL присваеваем наш, созданный объект, теперь он - конец списка)
bool append(struct list* L, char* str, int l){
    struct list* lst = L;
    while(lst->next != NULL){
        lst = lst->next;
    }
    struct list* new = (struct list*)malloc(sizeof (struct list));
    if(new == NULL){
        perror("no memory for adding list element");
        return false;
    }
    if(str == NULL){
        perror("mistakes in input");
        return false;
    }
    new->string = str;
    new->len = l;
    new->next = NULL;
    lst->next = new;
    return true;
}

// функция взятия строки, создаем новую строку с помощью malloc, с помощью fgets берем строку, проверяем её на то
// является ли она последней, если да, то меняем значение переменной is_end на true, чтобы в другой функции
// сообщить о том, что нужно закончить процесс ввода строк, возвращаем полученную строку
char* take_string(bool* is_end, int* l){


    char* str = (char*)malloc(sizeof (char) * MAX_SIZE);
    if(str == NULL){
        perror("no memory for new string");
        return NULL;
    }
    char end_symbol = '\n';

    char* fgets_checker = fgets(str, MAX_SIZE, stdin);
    if(fgets_checker == NULL){
        int file_checker = ferror(stdin);// ferror проверяет, имеются ли файловые ошибки в заданном потоке(stdin), возврат 0 означает отсутствие ошибок, а ненулевая величина указывает на наличие ошибки
        if(file_checker != 0){
            perror("errors in stdin");
            free(str);
            return NULL;
        }
    }

    int local_size = strlen(str);
    *l = local_size;
    char end_input_symbol = '.';
    if(str[0] == end_input_symbol){
        *is_end = true;
    }


    if(str[local_size-1] == end_symbol){
        return str;
    }
    int size = MAX_SIZE;
    while(true) {
        char new_str[MAX_SIZE];
        fgets_checker =  fgets(new_str, MAX_SIZE, stdin);
        if(fgets_checker == NULL){
            int file_checker = ferror(stdin);
            if(file_checker != 0){
                perror("errors in stdin");
                free(str);
                return NULL;
            }
        }

        int new_len = strlen(new_str);

        str = realloc(str, (size+new_len)*sizeof(char));
        if(str == NULL){
            perror("no memory for new string");
            return NULL;
        }
        for(int i = 0; i < new_len;i ++){
            str[size+i] = new_str[i];
        }

        size += new_len;
        *l = size;
        if(new_len < MAX_SIZE-1 || new_str[new_len-1] == end_symbol){
            break;
        }
    }

    return str;


    //fgets считывает до num-1 символов из файла, указанного как stdin(стандартный
    // ввод - чтение с устройства текстового интерфейса пользователя(клавиатуры)), и помещает их в массив указанный первым
    // аргументом(str), символы считываются пока не встретится символ новой строки(\0), EOF или до достижения указанного в num предела,
    // по окончании считывания в массив str сразу после последнего считанного символа помещается нулевой символ, символ новой строки при считывании
    // сохраняется и становится частью массива str. если функция отработала верно, то она возвращает str, иначе NULL, т.к. как в случае ошибки, так
    // и при достижении конца файла возвращается null, для определения того, что именно произошло мы использовали ferror

}

//функция заполнения списка, вставляем новые строки, пока маркер is_end не станет false во время функции take_string
void fill_list(struct list* lst){
    bool is_end = false;
    while(!is_end){
        int l = 0;
        char* s = take_string(&is_end, &l);
        bool append_checker = append(lst, s,l);
        if(append_checker == false){
            break;
        }
    }

}


// функция вывода списка, идем по списку и выводим каждую строчку элемента списка struct_list, пока не дойдем до такого
// элемента, в котором next будет равен NULL
int print_list(struct list* lst){
    int print_check = 0;
    if(lst == NULL) {
        printf("empty list");
        return EVERYTHING_OK;
    }
    struct list *new = lst;

    print_check = printf("Here is your list: \n");
    if (print_check < 0) {
        perror("can't print");
        list_free(lst);
        return (PRINT_FAULT);
    }
    while (new->next != NULL) {
        //strlen возвращает длину строки, оканчивающейся нулевым символом, на которую указывает str, при определении длины строки нулевой символ не учитывается
        int i = 0;
        while(i < new->len) {
            print_check = printf("%c", new->string[i]);
            if (print_check < 0) {
                list_free(lst);
                perror("can't print");
                return (PRINT_FAULT);
            }
            i++;
        }
        new = new->next;
    }
    int i = 0;
    while (i < new->len) {
        print_check = printf("%c", new->string[i]);
        if (print_check < 0) {
            perror("can't print");
            list_free(lst);
            return (PRINT_FAULT);
        }
        i++;
    }

    return EVERYTHING_OK;
}


int main() {
    bool checker = false;
    int l = 0;
    char* str = take_string(&checker, &l);
    while(str == NULL){
        printf("try again");
        perror("mistakes in input");
        str = take_string(&checker, &l);
    }
    struct list* lst = init_list(str,l);
    if(!checker){
        fill_list(lst);
    }

    int print_lst_check = print_list(lst);
    if(print_lst_check != EVERYTHING_OK){
        list_free(lst);
        exit(print_lst_check);
    }
    list_free(lst);
    return 0;
}
