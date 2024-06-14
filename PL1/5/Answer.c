#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
        /* Create child 1 */
        pid_t p1 = fork();
        if (p1 == -1) {
                perror("Could not fork.");
                exit(1);
        } else if (p1 != 0) {
                /* Main */
                /* Create child 2 */
                pid_t p2 = fork();
                if (p2 == -1) {
                        perror("Could not fork.");
                        exit(1);
                } else if (p2 != 0) {
                        /* Main */
                        /* Wait for children */
                        int status1, status2;
                        waitpid(p1, &status1, 0);
                        waitpid(p2, &status2, 0);
                        /* Display values */
                        printf("1 = %d; 2 = %d\n", WEXITSTATUS(status1), WEXITSTATUS(status2));
                } else {
                        /* Child 2 */
                        sleep(2);
                        exit(2);
                }
        } else {
                /* Child 1 */
                sleep(1);
                exit(1);
        }
}
