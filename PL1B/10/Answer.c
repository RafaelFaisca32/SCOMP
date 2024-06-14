#define _XOPEN_SOURCE 700
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* Max size of input string */
#define MAX_STR_SIZE 300

void signalHandler(int, siginfo_t*, void*);

int main() {
        char str[MAX_STR_SIZE];

        /* Set signal handler */
        struct sigaction act;
        memset(&act, 0, sizeof(struct sigaction));
        /* Mask all signals */
        sigfillset(&act.sa_mask);
        /* Set handler function */
        act.sa_sigaction = signalHandler;
        act.sa_flags     = SA_SIGINFO;
        /* Handle on SIGALRM */
        sigaction(SIGALRM, &act, NULL);

        /* Run */
        printf("YOU HAVE 10 SECONDS TO INPUT A STRING\n> ");
        alarm(10);
        fgets(str, sizeof(str), stdin);
        /* Cancel alarm */
        alarm(0);
        printf("THINKING...\n");
        sleep(20);
        printf("Successful execution!\n");
        return 0;
}

void signalHandler(int signum, siginfo_t* info, void* s) {
        char str[] = "\nTo slow, that is why the program will end!\n";
        write(STDOUT_FILENO, str, sizeof(str));
        exit(0);
}
