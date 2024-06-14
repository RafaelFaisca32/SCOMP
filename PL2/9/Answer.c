#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

typedef struct {
        int customer_code;
        int product_code;
        int quantity;
} Venda;

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
        int   fd[2], i, id_crianca;
        Venda sales[50000];

        for (i = 0; i < 50000; i++) { /* preencher o array com valores*/
                Venda vendas;
                vendas.customer_code = i;
                vendas.product_code  = i;
                vendas.quantity      = 1;
                sales[i]             = vendas;
        }

        sales[10].quantity   = 25; /*sales com mais de 20 unit vendidas*/
        sales[15].quantity   = 32; /*sales com mais de 20 unit vendidas*/
        sales[3000].quantity = 21; /*sales com mais de 20 unit vendidas*/

        if (pipe(fd) == -1) {
                perror("Pipe failed");
                return 1;
        }

        id_crianca = spawn_childs(10); /* cria 10 processos filho */

        if (id_crianca == 0) { /* pai */
                int products[50000];
                int i, status;
                close(fd[1]);              /* close à escrita do pipe */
                for (i = 0; i < 10; i++) { /* for a passar por todos os processos filho */
                        if (read(fd[0], &sales, sizeof(Venda)) >
                            0) { /* verifica a existencia de valores passados pelo pipe */
                                products[i] = sales->product_code; /* guardo no array products o
                                                                      product_code */
                                wait(&status);
                                if (WIFEXITED(status)) { /* verifica se os 10 processos filho já
                                                            terminaram */
                                        printf("Superior a 20: %d\n", products[i]);
                                }
                        }
                }
                close(fd[0]); /* close à leitura do pipe */
        }

        if (id_crianca > 0) { /* filho */
                int i;
                close(fd[0]); /* close à leitura do pipe */
                for (i = (id_crianca - 1) * 5000; i < id_crianca * 5000;
                     i++) { /* 5000 sales por criança */
                        if (sales[i].quantity > 20) {
                                write(fd[1], &sales[i],
                                      sizeof(Venda)); /* escrita do codigo de produto de todas as
                                                         sales com quantidade superior a 20 para
                                                         eventual leitura pelo processo pai */
                        }
                }
                close(fd[1]); /* close à escrita do pipe */
        }
}
