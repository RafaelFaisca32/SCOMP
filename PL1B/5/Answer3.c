/* To compile with -ansi and -pedantic */
#define _XOPEN_SOURCE 700
#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void signalHandlerINT(int, siginfo_t*, void*);
void signalHandlerQUIT(int, siginfo_t*, void*);

int main() {
        struct sigaction act;
        /* Set signal handler (for INT) */
        memset(&act, 0, sizeof(struct sigaction));
        /* Block only SIGINT */
        sigemptyset(&act.sa_mask);
        sigaddset(&act.sa_mask, SIGINT);
        /* Set handler */
        act.sa_sigaction = signalHandlerINT;
        act.sa_flags     = SA_SIGINFO;
        /* Handle on SIGINT */
        sigaction(SIGINT, &act, NULL);

        /* Set signal handler (for QUIT) */
        sigemptyset(&act.sa_mask);
        /* Block only SIGQUIT */
        sigaddset(&act.sa_mask, SIGQUIT);
        /* Set handler */
        act.sa_sigaction = signalHandlerQUIT;
        act.sa_flags     = SA_SIGINFO;
        /* Handle on SIGQUIT */
        sigaction(SIGQUIT, &act, NULL);

        /* Infinite loop */
        for (;;) {
                printf("I <3 SCOMP!\n");
                sleep(1);
        }
        return 0;
}

void signalHandlerINT(int signum, siginfo_t* info, void* s) {
        /* Display message */
        char str[] = "I won’t let the process end with CTRL-C!\n";
        write(STDOUT_FILENO, str, sizeof(str));
}

void signalHandlerQUIT(int signum, siginfo_t* info, void* s) {
        /* Display message */
        char str[] = "I won’t let the process end with CTRL-\\!\n";
        write(STDOUT_FILENO, str, sizeof(str));
}
