#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>


void read_from_pipe (int file)
{
    FILE *stream;
    int c;
    stream = fdopen (file, "r");
    while ((c = fgetc (stream)) != EOF)
        putchar (c);
    fclose (stream);
}
void write_to_pipe (int file)
{
    FILE *stream;
    stream = fdopen (file, "w");
    fprintf (stream, "hello, world!\n");
    fprintf (stream, "goodbye, world!\n");
    fclose (stream);
}

static void si_hand_1 (int signo)
{
    if (signo == SIGINT) {
        printf("\a");
    }else if (signo == SIGQUIT) {

        printf("\a");
        exit(1);
    }
    exit(1);
}


int main(int argc, char* argv[])
{
    pid_t pid_1, pid_2, pid_3, pid_4;
    struct sigaction act;
    int ret;
    int mypipe[2];

    if (pipe(mypipe))
    {
        fprintf (stderr, "Pipe failed.\n");
        return EXIT_FAILURE;
    }

    int iterator = 0;
    while (iterator < 5) {
        pid_1 = fork();
        if (pid_1 == -1) {
            printf("fork 1 failure\n");
            exit(1);
        }
        if (!pid_1)
        {
            memset(&act, 0, sizeof(act));
            act.sa_handler = si_hand_1;
            sigemptyset(&act.sa_mask);
            ret = sigaction(SIGINT, &act, NULL);
            if(ret == 0){
                printf("IAMALONE\n");
            }
            if (ret == -1) {
                printf("sigaction failure\n");
                exit(1);
            }
            ret = sigaction(SIGQUIT, &act, NULL);
            if (ret == -1) {
                printf("sigaction failure\n");
                exit(1);
            }

        }
        iterator++;
    }

    for(;;)
        pause();
    return 0;
}