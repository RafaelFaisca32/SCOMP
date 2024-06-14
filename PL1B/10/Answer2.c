#define _XOPEN_SOURCE 700
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* Max size of input string */
#define MAX_STR_SIZE 300

int main() {
        char  str[MAX_STR_SIZE];
        pid_t p = fork();
        if (p == -1) {
                perror("Could not fork.");
                exit(1);
        } else if (p != 0) {
                /* Parent asks for string */
                printf("YOU HAVE 10 SECONDS TO INPUT A STRING\n> ");
                fgets(str, sizeof(str), stdin);
        } else {
                /* Child waits 10 secs before killing parent */
                sleep(10);
                printf("To slow, that is why the program will end!\n");
                kill(getppid(), SIGKILL);
        }
        /* Parent kills child */
        kill(p, SIGKILL);
        printf("THINKING...\n");
        sleep(20);
        printf("Successful execution!\n");
        return 0;
}
