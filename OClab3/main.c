#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main() {
    FILE *file;// инициализируем объект типа файл
    int print_check = 0;
    print_check = printf("Effective user id: %d\n",geteuid());//вывод эффективного идентификатора пользователя
    if(print_check < 0){
        perror("can't print");
        exit(4);
    }
    print_check = printf("Real user id: %d\n",getuid());//вывод реального идентефикатора пользователя
    //фактический ID соответствует ID пользователя, который вызвал процесс, эффективный ID соответствует установленному setuid биту на исполняемом файле
    if(print_check < 0){
        perror("can't print");
        exit(4);
    }

    if((file = fopen("test_file.txt", "r"))==NULL){
        perror("File wasn't opened 1"); // помещает значение глобальной переменной errno в строку и записывает эту строку в файл stderr, если ничего в функцию не передаем
        //то сначала выводится строка, а за ней следует двоеточие и сообщение об ошибке, соответствующее значению errno, иначе, выводится наша строка
        exit(1);
    }else{
        if(fclose(file) != 0){
            perror("can't close file");
            exit(5);
        }
    }
    //открываем файл, если получилось открыть, file будет присвоен указатель файла test_file.txt, если нет доступа, то ничего не будет присвоено, т.е. NULL
    //поэтому при проверке сравниваем именно с NULL

    if(setuid(getuid()) == EPERM){
        perror("user isn't superuser");
        exit(2);
    }//устанавливает фактический идентификатор владельца текущего процесса, если фактический пользователь, вызвавший эту функцию является суперпользователем,
    // то также устанавливаются действительный и сохраненный идентификаторы, при успешной работе возвращает 0, при неуспешной -1

    print_check = printf("Effective user id: %d\n",geteuid());
    if(print_check < 0){
        perror("can't print");
        exit(4);
    }
    print_check = printf("Real user id: %d\n",getuid());
    if(print_check < 0){
        perror("can't print");
        exit(4);
    }

    if((file = fopen("test_file.txt", "r"))==NULL){
        perror("File wasn't opened 2");
        exit(3);
    }else{
        if(fclose(file) != 0){
            perror("can't close file");
            exit(5);
        }
    }


    return 0;
}
