#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int main() {
        pid_t p, espera;
        int   status, saida, fd[2];

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
                char string1[100] = "Hello world";
                char string2[100] = "Goodbye!";
                close(fd[0]);                                /* close à leitura do pipe */
                write(fd[1], &string1, sizeof(string1) + 1); /* escreve a string1 do pai no pipe */
                write(fd[1], &string2, sizeof(string2) + 1); /* escreve a string2 do pai no pipe */
                close(fd[1]);                                /* close à escrita do pipe */
        }

        if (p == 0) { /* filho */
                char string1[100];
                char string2[100];
                close(fd[1]);               /* close à escrita do pipe */
                read(fd[0], &string1, 101); /* lê a string1 do pai no pipe */
                read(fd[0], &string2, 101); /* lê a string2 do pai no pipe */
                close(fd[0]);               /* close à leitura do pipe */
                printf("String1 do pai vinda do filho: %s\n", string1);
                printf("String2 do pai vinda do filho: %s\n", string2);
        }

        espera = wait(&status);
        saida  = WEXITSTATUS(status);
        printf("PID: %d e terminou com: %d\n", espera, saida);
        return 0;
}
