#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
        struct leitura {
                int  lei;
                char vec[280];
        };

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

        if (p > 0) { /* pai */
                struct leitura pai;
                close(fd[0]); /* close à leitura do pipe */
                printf("Insere o número, pai:\n");
                scanf("%d", &inteiro);
                printf("Insere a string, pai:\n");
                scanf("%s", string);
                pai.lei = inteiro;
                strcpy(pai.vec, string);
                write(fd[1], &pai, sizeof(pai)); /* escreve a struct do pai no pipe */
                close(fd[1]);                    /* close à escrita do pipe */
        }

        if (p == 0) { /* filho */
                struct leitura filho;
                close(fd[1]);                       /* close à escrita do pipe */
                read(fd[0], &filho, sizeof(filho)); /* lê a struct do pai no pipe */
                close(fd[0]);                       /* close à leitura do pipe */
                printf("Inteiro do pai vinda do filho: %d\n", filho.lei);
                printf("String do pai vinda do filho: %s\n", filho.vec);
        }

        return 0;
}
