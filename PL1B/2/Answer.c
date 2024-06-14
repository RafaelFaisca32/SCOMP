#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <unistd.h>

int main() {
        for (;;) {
                printf("I <3 SCOMP!\n");
                sleep(1);
        }
        return 0;
}
