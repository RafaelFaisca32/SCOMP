#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
        int   fd[2], inteiro;
        pid_t p;
        char  string[300];

        if (pipe(fd) == -1) {
                perror("Pipe failed");
                return 1;
        }

        p = fork();
        if (p == -1) {
                perror("Fork failed");
                return 1;
        }

        if (p > 0) {          /* pai */
                close(fd[0]); /* close à leitura do pipe */
                printf("Insere o número, pai:\n");
                scanf("%d", &inteiro);
                printf("Insere a string, pai:\n");
                scanf("%s", string);
                write(fd[1], &inteiro, sizeof(inteiro));   /* escreve o inteiro do pai no pipe */
                write(fd[1], &string, sizeof(string) + 1); /* escreve a string do pai no pipe */
                close(fd[1]);                              /*close à escrita do pipe */
        }

        if (p == 0) {                                   /* filho */
                close(fd[1]);                           /* close à escrita do pipe */
                read(fd[0], &inteiro, sizeof(inteiro)); /* lê o inteiro do pai no pipe */
                read(fd[0], &string, sizeof(300));      /* lê a string do pai no pipe */
                close(fd[0]);                           /* close à leitura do pipe */
                printf("Inteiro do pai vinda do filho: %d\n", inteiro);
                printf("String do pai vinda do filho: %s\n", string);
        }

        return 0;
}
