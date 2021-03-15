#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#define MAX_SIZE 150
// создаем структуру list, которая содержит строку и указатель на следующий элемент нашего списка

struct list{
    char* string;
    struct list* next;
};

// функция инициализации списка, выделяем память под строку, которую хотим вставить, вставляем её, т.к. у нас список строится
// от 1 элемента на данный момент, указатель на последующий элемент = NULL, т.е. наш вставленный элемент одновременно
// и голова списка и его конец
struct list* init_list(char* str){
    struct list* lst = (struct list*)malloc(sizeof (struct list));
    if(lst == NULL){
        perror("no memory for new list");
        exit(1);
    }
    lst->string = str;
    lst->next = NULL;
    return lst;
}

// функция добавления элемента в список, элемент добавляется в конец списка(создаем объект типа struct list, на основе аргумента str,
// у него поле next = NULL, к последнему элементу списка вместо указателя NULL присваеваем наш, созданный объект, теперь он - конец списка)
void append(struct list* L, char* str){
    struct list* lst = L;
    while(lst->next != NULL){
        lst = lst->next;
    }
    struct list* new = (struct list*)malloc(sizeof (struct list));
    if(new == NULL){
        perror("no memory for new list");
        exit(1);
    }
    new->string = str;
    new->next = NULL;
    lst->next = new;
}

// функция взятия строки, создаем новую строку с помощью malloc, с помощью fgets берем строку, проверяем её на то
// является ли она последней, если да, то меняем значение переменной is_end на true, чтобы в другой функции
// сообщить о том, что нужно закончить процесс ввода строк, возвращаем полученную строку
char* take_string(bool* is_end){
    char* str = (char*)malloc(sizeof (char) * MAX_SIZE);
    if(str == NULL){
        perror("no memory for new list");
        exit(1);
    }

    if(fgets(str, MAX_SIZE, stdin) == NULL){
        int file_checker = ferror(stdin);// ferror проверяет, имеются ли файловые ошибки в заданном потоке(stdin), возврат 0 означает отсутствие ошибок, а ненулевая величина указывает на наличие ошибки
        if(file_checker != 0){
            perror("errors in stdin");
            exit(3);
        }
    }
    //fgets считывает до num-1 символов из файла, указанного как stdin(стандартный
    // ввод - чтение с устройства текстового интерфейса пользователя(клавиатуры)), и помещает их в массив указанный первым
    // аргументом(str), символы считываются пока не встретится символ новой строки(\0), EOF или до достижения указанного в num предела,
    // по окончании считывания в массив str сразу после последнего считанного символа помещается нулевой символ, символ новой строки при считывании
    // сохраняется и становится частью массива str. если функция отработала верно, то она возвращает str, иначе NULL, т.к. как в случае ошибки, так
    // и при достижении конца файла возвращается null, для определения того, что именно произошло мы использовали ferror
    char end_input_symbol = '.';
    if(str[0] == end_input_symbol){
        *is_end = true;
    }
    return str;
}

//функция заполнения списка, вставляем новые строки, пока маркер is_end не станет false во время функции take_string
void fill_list(struct list* lst){
    bool is_end = false;
    while(!is_end){
        append(lst, take_string(&is_end));
    }

}

// функция вывода списка, идем по списку и выводим каждую строчку элемента списка struct_list, пока не дойдем до такого
// элемента, в котором next будет равен NULL
void print_list(struct list* lst){
    int print_check = 0;
    if(lst == NULL){
        perror("nothing to print");
        exit(4);
    }
    struct list* new = lst;

    print_check = printf("Here is your list: \n");
    if(print_check < 0){
        perror("can't print");
        exit(5);
    }
    while(new->next != NULL){
        for(int i = 0; i < (int)strlen(new->string) ; i++){ //strlen возвращает длину строки, оканчивающейся нулевым символом, на которую указывает str, при определении длины строки нулевой символ не учитывается
            print_check = printf("%c", new->string[i]);
            if(print_check < 0){
                perror("can't print");
                exit(5);
            }
        }
        new = new->next;
    }
    for (int i = 0; i < (int)strlen(new->string) ; i++) {
        print_check = printf("%c", new->string[i]);
        if(print_check < 0){
            perror("can't print");
            exit(5);
        }
    }
}

void list_free(struct list* lst){
    struct list* next = lst->next;
    while(lst->next != NULL){
        free(lst->string);
        free(lst);
        lst = next;
        next = lst->next;
    }
    free(lst->string);
    free(lst);
}

int main() {
    bool checker = false;
    char* str = take_string(&checker);

    struct list* lst = init_list(str);

    if(!checker){
        fill_list(lst);
    }

    print_list(lst);
    list_free(lst);
    return 0;
}