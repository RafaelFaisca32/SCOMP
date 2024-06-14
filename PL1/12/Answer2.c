#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

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
                        return (i + 1) * 2;
                }
        }
        return 0; /*caso seja pai*/
}

int main() {
        spawn_childs(6);
        return 0;
}
