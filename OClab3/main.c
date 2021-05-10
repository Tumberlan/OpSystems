#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#define SUCCESS_IN_FILE_CLOSING 0
#define FIRST_OPEN_ERROR 0
#define SECOND_OPEN_ERROR 1
#define FIRST_CLOSE_F_ERROR 2
#define SECOND_CLOSE_F_ERROR 3
#define SETUID_ERROR (-1)

int main() {
    FILE *file;// инициализируем объект типа файл

    printf("Effective user id: %d\n",geteuid());//вывод эффективного идентификатора пользователя
    printf("Real user id: %d\n",getuid());//вывод реального идентефикатора пользователя
    //фактический ID соответствует ID пользователя, который вызвал процесс, эффективный ID соответствует установленному setuid биту на исполняемом файле

    file = fopen("test_file.txt", "r");
    if(file == NULL){
        perror("File wasn't opened 1"); // помещает значение глобальной переменной errno в строку и записывает эту строку в файл stderr, если ничего в функцию не передаем
        //то сначала выводится строка, а за ней следует двоеточие и сообщение об ошибке, соответствующее значению errno, иначе, выводится наша строка
        exit(FIRST_OPEN_ERROR);
    }else{
        int close_check = fclose(file);
        if(close_check != 0){
            perror("Error in closing file");
            exit(FIRST_CLOSE_F_ERROR);
        }
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

    file = fopen("test_file.txt", "r");
    if(file == NULL){
        perror("File wasn't opened 2");
        exit(SECOND_OPEN_ERROR);
    }
    int close_check = fclose(file);
    if(close_check != SUCCESS_IN_FILE_CLOSING){
        perror("Error in closing file");
        exit(SECOND_CLOSE_F_ERROR);
    }



    return 0;
}
