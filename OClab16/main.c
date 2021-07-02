#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#define ERROR -1
#define ERR_ISATTY 0
#define STDIN_DESC 0
#define AMOUNT_OF_PRESSED_KEYS 1

char reply(char *question) {
    char answer;
    struct termios term;
    int fd = STDIN_DESC;
    printf("%s if yes, type 'y', if not 'n'\n", question);
    if(fflush(stdout) == EOF) {
        perror("fflush error");
        return ERROR;
    }
    if (tcgetattr(fd, &term) == ERROR) {
        perror("tcgetattr error");
        return ERROR;
    }
    int count = AMOUNT_OF_PRESSED_KEYS;
    term.c_lflag &= ~(ISIG | ICANON);
    term.c_cc[VMIN] = count;
    if (tcsetattr(fd, TCSAFLUSH, &term) == ERROR) {
        perror("tcsetattr error");
        return ERROR;
    }
    if (read(fd, &answer, count) == ERROR) {
        perror("read error");
        return ERROR;
    }
    if (tcsetattr(fd, TCSANOW, &term) == ERROR) {
        perror("tcsetattr error");
        return ERROR;
    }
    return answer;
}

int main(){
    char answer;
    int fd = STDIN_DESC;
    if(isatty(fd) == ERR_ISATTY){
        perror("Issatty error");
        exit(EXIT_FAILURE);
    }
    answer = reply("Everything OK?");
    printf("\n");
    if (answer == ERROR) {
        exit(EXIT_FAILURE);
    }
    switch (answer) {
        case 'y':
            printf("Your answer: Yes\n");
            break;
        case 'n':
            printf("Your answer: No\n");
            break;
        default:
            printf("Invalid reply\n");
            break;
    }
    exit(EXIT_SUCCESS);
}