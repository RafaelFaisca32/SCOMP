/* To compile with -ansi and -pedantic */
#define _XOPEN_SOURCE 700
#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void signalHandler(int, siginfo_t*, void*);

int main() {
        struct sigaction act;
        /* Set signal handler */
        memset(&act, 0, sizeof(struct sigaction));
        /* Block only SIGINT */
        sigemptyset(&act.sa_mask);
        sigaddset(&act.sa_mask, SIGINT);
        /* Set handler */
        act.sa_sigaction = signalHandler;
        act.sa_flags     = SA_SIGINFO;
        /* Handle on SIGINT */
        sigaction(SIGINT, &act, NULL);

        /* Infinite loop */
        for (;;) {
                printf("I <3 SCOMP!\n");
                sleep(1);
        }
        return 0;
}

void signalHandler(int signum, siginfo_t* info, void* s) {
        /* Display message */
        char str[] = "I won’t let the process end with CTRL-C!\n";
        write(STDOUT_FILENO, str, sizeof(str));
}
