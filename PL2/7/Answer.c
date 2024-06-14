#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int spawn_childs(int n) {
        pid_t p;
        int   i;
        for (i = 0; i < n; i++) {
                p = fork();
                if (p == -1) {
                        perror("Fork failed");
                        return -1;
                }
                if (p == 0) { /*caso seja filho*/
                        return i + 1;
                }
        }
        return 0; /*caso seja pai*/
}

int main() {
        int vec1[1000], vec2[1000], fd[5][2], i, j, id_crianca;

        for (i = 0; i < 1000; i++) { /* FOR para dar valores ao VEC1 */
                vec1[i] = i;
        }

        for (j = 0; j < 1000; j++) { /* FOR para dar valores ao VEC2 */
                vec2[j] = j;
        }


        for (i = 0; i < 5; i++) {
                if (pipe(fd[i]) == -1) {
                        perror("Pipe failed");
                        return 1;
                }
        }

        id_crianca = spawn_childs(
                5); /* chama a funçaõ para criar n processos filho, neste caso, 5 processos filho.*/

        if (id_crianca == 0) { /* pai */
                int i, tmp, resultado = 0, status;
                for (i = 0; i < 5; i++) { /* for a passar pelos 5 filhos*/
                        close(fd[i][1]);  /* close a parte de escrita do pipe(i) */
                        wait(&status);
                        if (WIFEXITED(
                                    status)) { /* verifica se os 5 processos filho já terminaram */
                                read(fd[i][0], &tmp, 4); /* read do tmp passado pelas crianças */
                                resultado += tmp;
                        }
                        close(fd[i][0]); /* close a parte de leitura do pipe(i) */
                }
                printf("Soma do resultado final é = %d\n", resultado);
        }

        if (id_crianca > 0) { /*child*/
                int i, tmp = 0;
                for (i = (id_crianca - 1) * 200; i < id_crianca * 200; i++) {
                        tmp = tmp + vec1[i] + vec2[i];
                }
                write(fd[id_crianca - 1][1], &tmp, 4); /*escrita do tmp no pipe */
                close(fd[id_crianca - 1][1]);          /* close da escrita do pipe */
        }
        return 0;
}
