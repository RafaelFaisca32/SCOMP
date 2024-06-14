#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
        int   fd[2];
        pid_t p;
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
                pid_t pp;
                close(fd[0]); /* close à leitura do pipe */
                pp = getpid();
                printf("Pid do pai: %d\n", pp);
                write(fd[1], &pp, sizeof(pp)); /* escreve a pid do pai no pipe */
                close(fd[1]);                  /* close à escrita do pipe */
        }

        if (p == 0) { /* filho */
                pid_t pp;
                close(fd[1]);                 /* close à escrita do pipe */
                pp = 0;                       /* pai */
                read(fd[0], &pp, sizeof(pp)); /* lê a pid do pai no pipe */
                close(fd[0]);                 /*close à leitura do pipe */
                printf("Pid do pai vinda do filho: %d\n", pp);
        }

        return 0;
}
