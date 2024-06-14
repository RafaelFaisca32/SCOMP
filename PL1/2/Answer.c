#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
        pid_t p;

        printf("I'm..\n");
        p = fork();
        if (p == -1) {
                perror("Could not fork.");
                exit(1);
        } else if (p == 0) {
                printf("I'll never join you!\n");
        } else {
                printf("the..\n");
                p = fork();
                if (p == -1) {
                        perror("Could not fork.");
                        exit(1);
                } else if (p == 0) {
                        printf("I'll never join you!\n");
                } else {
                        printf("father!\n");
                        p = fork();
                        if (p == -1) {
                                perror("Could not fork.");
                                exit(1);
                        } else if (p == 0) {
                                printf("I'll never join you!\n");
                        }
                }
        }
        return 0;
}
