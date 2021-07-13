#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


#define PUTENV_ERROR -1

int a;


int* func(){
    int mas[4];
    for(int i = 0; i < 4; i++){
        mas[i] = i+1;
    }
    return &mas;
}
int main()
{

  /*  int putenv_checker = putenv("TZ=America/Los_Angeles");
    if(putenv_checker == PUTENV_ERROR){
        perror("putenv failed");
        exit(1);
    } //модифицируем значение переменной окружения TZ под то, которое нам нужно, чтобы
    //узнать время в калифорнии, но т.к. идентификатора для калифорнии нет, мы используем Los_Angeles, т.к. он в том же часовом поясе


    time_t now; //инициализация переменной now арифмитического типа, для представления времени
    int putenv_checker = putenv("TZ=America/Los_Angeles");
    if(putenv_checker == PUTENV_ERROR){
        perror("putenv failed");
        exit(1);
    }
    struct tm sp1; //структура struct tm содержит компоненты календарного времени, чтобы их посмотреть достаточно написать sp-> и выбрать нужное значение
    struct tm *sp; //структура struct tm содержит компоненты календарного времени, чтобы их посмотреть достаточно написать sp-> и выбрать нужное значение
    (void) time( &now ); //time записывает в now значение времени  в секундах  от 1 января 1970, 00:00:00 UTC, при неуспешном завершении записывает -1

     printf("%s", ctime( &now ) ); // ctime возвращает  адрес переменной now, это  адрес  начала  символьной строки
     // строка ctime выглядит так: "День недели Месяц Число Час:Минута:Секунда Год\n\0"

     sp = localtime(&now);//localtime преобразует календарное время переменной now и сохраняет в структуру struct tm
     //возвращает указатель на эту структуру, указатель мы присваеваем переменной sp
     //подробнее, localtime  сначала   вызывает  tzset,   чтобы   получить   значение переменной  shell   TZ.   На   основании   этого   значения   tzset
     //инициализирует  массив   указателей  на  строки  tzname  и  внешние переменные timezone  и  altzone.  timezone  устанавливается  равной
     //разнице времени  между UTC(время взятое за единое, от него отсчитываются все остальные часовые пояса)  и данной временной зоной (в секундах).


     printf("%d/%d/%02d %d:%02d %s\n",
         sp->tm_mon + 1, sp->tm_mday,
         1900 + sp->tm_year, sp->tm_hour,
         sp->tm_min, tzname[sp->tm_isdst]
         ); // выводим нужные нам поля структуры struct tm, записанные в sp, в tzname хранится 2 значения "PST"(стандартное время) и "PDT"(летнее время),
         // если параметрические значения не заданы, то вместо "PST" - пусто, и если часовой пояс с переходом на летнее время не указан, вместо "PDT" - пусто
     return 0;*/

  time_t n1;
  time_t n2;
  struct tm *sp1;
  struct tm *sp2;
  time(&n1);
    _sleep(1);
    time(&n2);
    sp1 = localtime(&n1);
    sp2 = localtime(&n2);
    printf("%d/%d/%02d %d:%02d %s\n",
           sp1->tm_mon + 1, sp1->tm_mday,
           1900 + sp1->tm_year, sp1->tm_hour,
           sp1->tm_min, tzname[sp1->tm_isdst]
    );
    printf("%d/%d/%02d %d:%02d %s\n",
           sp2->tm_mon + 1, sp2->tm_mday,
           1900 + sp2->tm_year, sp2->tm_hour,
           sp2->tm_min, tzname[sp2->tm_isdst]
    );


    return 0;
 }
