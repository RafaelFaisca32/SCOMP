#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <unistd.h>

int main() {
        printf("\tWAIT PROGRAM, WAITING 10 SECONDS\n");
        sleep(10);
        printf("\tExecution ended!\n");
        return 0;
}
