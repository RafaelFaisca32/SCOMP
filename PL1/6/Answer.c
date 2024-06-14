#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int main() {
        int i;
        int status;
        for (i = 0; i < 4; i++) {
                pid_t p = fork();
                if (p > 0 && p % 2 == 0) { waitpid(p, &status, 0); }
        }
        return 0;
}
