#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#define FIRST_OPEN_ERROR 0
#define SECOND_OPEN_ERROR 1
#define FIRST_CLOSE_F_ERROR 2
#define SECOND_CLOSE_F_ERROR 3
#define SETUID_ERROR (-1)
#define CLOSE_SUCCESS 0
#define NO_EXCEPTIONS 4


int open_close_file_f(FILE* file, int open_number){
    file = fopen("test_file.txt", "r");
    int error_open, error_close;
    if(open_number == 1){
        error_open = FIRST_OPEN_ERROR;
        error_close = FIRST_CLOSE_F_ERROR;
    }
    if(open_number == 2){
        error_open = SECOND_OPEN_ERROR;
        error_close = SECOND_CLOSE_F_ERROR;
    }
    if(file == NULL){
        perror("File wasn't opened"); // помещает значение глобальной переменной errno в строку и записывает эту строку в файл stderr, если ничего в функцию не передаем
        //то сначала выводится строка, а за ней следует двоеточие и сообщение об ошибке, соответствующее значению errno, иначе, выводится наша строка
        return (error_open);
    }
    int close_check = fclose(file);
    if(close_check != CLOSE_SUCCESS){
        perror("Error in closing file");
        return (error_close);
    }
    return NO_EXCEPTIONS;
}

int main() {
    FILE *file = NULL;// инициализируем объект типа файл
    long int uid = getuid();
    long int euid = geteuid();

    printf("Effective user id: %ld\n",euid);//вывод реального идентефикатора пользователя
    printf("Real user id: %ld\n",uid);//вывод эффективного идентификатора пользователя
    //фактический ID соответствует ID пользователя, который вызвал процесс, эффективный ID соответствует установленному setuid биту на исполняемом файле
    int open_close_file_check = open_close_file_f(file,1);
    if(open_close_file_check != NO_EXCEPTIONS){
        exit(open_close_file_check);
    }
    //открываем файл, если получилось открыть, file будет присвоен указатель файла test_file.txt, если нет доступа, то ничего не будет присвоено, т.е. NULL
    //поэтому при проверке сравниваем именно с NULL
    int setuid_check = setuid(getuid());
    if(setuid_check == SETUID_ERROR){
        perror("user isn't superuser");
        exit(SETUID_ERROR);
    }//устанавливает фактический идентификатор владельца текущего процесса, если фактический пользователь, вызвавший эту функцию является суперпользователем,
    // то также устанавливаются действительный и сохраненный идентификаторы, при успешной работе возвращает 0, при неуспешной -1
    printf("Effective user id: %d\n",geteuid());
    printf("Real user id: %d\n",getuid());
    open_close_file_check = open_close_file_f(file, 2);
    if(open_close_file_check != NO_EXCEPTIONS){
        exit(open_close_file_check);
    }
    return 0;
}