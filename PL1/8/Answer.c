#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

int main() {
        pid_t p;

        if (fork() == 0) {
                printf("PID = %d\n", getpid());
                exit(0);
        }

        if ((p = fork()) == 0) {
                printf("PID = %d\n", getpid());
                exit(0);
        }

        printf("Parent PID = %d\n", getpid());

        printf("Waiting... (for PID=%d)\n", p);
        waitpid(p, NULL, 0);

        printf("Enter Loop...\n");
        /* Infinite loop */
        while (1)
                ;
}
