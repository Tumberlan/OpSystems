#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

#define SUCCESS_STATUS 0
#define WRONG_ARGUMENTS_NUMBER_ERROR 1
#define FORK_ERROR 2
#define WAIT_ERROR 3
#define EXECVP_ERROR 4

#define FORK_ERROR_VALUE -1
#define WAIT_ERROR_VALUE -1
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

int wait_child_proc() {
    int child_return_code = 0;
    pid_t wait_res = wait(&child_return_code);

    if (wait_res == WAIT_ERROR_VALUE){
        perror("Error on wait for child result code");
        return WAIT_ERROR;
    }

    if (WIFSIGNALED(child_return_code)) {
        int signal_info = WTERMSIG(child_return_code);
        printf("Child process terminated with a signal: %d\n", signal_info);
        return SUCCESS_STATUS;
    }
    if (WIFEXITED(child_return_code)) {
        int exit_code = WEXITSTATUS(child_return_code);
        printf("Child process exited with code: %d\n", exit_code);
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

    int wait_res = wait_child_proc();
    if (wait_res != SUCCESS_STATUS) {
        return wait_res;
    }

    return SUCCESS_STATUS;
}