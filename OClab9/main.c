#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

#define SUCCESS_STATUS 0
#define WRONG_ARGUMENTS_NUMBER_ERROR 1
#define FORK_ERROR 2
#define EXECVP_ERROR 4

#define FORK_ERROR_VALUE -1
#define CHILD_FORK_RESULT_VALUE 0

#define REQUIRED_ARGUMENTS_NUMBER 2


int execute_command(char *command_name, char *argv[]) {
    pid_t fork_res = fork();
    if (fork_res == FORK_ERROR_VALUE) {
        perror("Error on fork");
        return FORK_ERROR;
    }
    if (fork_res == CHILD_FORK_RESULT_VALUE) {
        execvp(command_name, argv);
        perror("Error on execvp");
        return EXECVP_ERROR;
    }
    return SUCCESS_STATUS;
}


int main(int argc, char *argv[]) {

    if (argc != REQUIRED_ARGUMENTS_NUMBER) {
        fprintf(stderr, "Wrong arguments number\n");
        return WRONG_ARGUMENTS_NUMBER_ERROR;
    }

    char *command_name = "cat";
    char *file_name = argv[1];
    char *command_argv[] = {command_name, file_name, NULL};

    printf("Output of specified file:\n");

    int execute_command_res = execute_command(command_name, command_argv);
    if (execute_command_res != SUCCESS_STATUS) {
        return execute_command_res;
    }

    return SUCCESS_STATUS;
}