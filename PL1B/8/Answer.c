#define _XOPEN_SOURCE 700
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* Number of child processes to create */
#define N_CHLD 5

volatile sig_atomic_t counter = 0;

/* Since signals may be set to pending but not qued there is a chance this function will not go
 * trough */
void signalHandler(int signo, siginfo_t* sinfo, void* context) {
        ++counter;
        /*const char str[] = "CHILD STOPED.\n";
        write(STDOUT_FILENO, str, sizeof(str));*/
}

int main() {
        int   i, j;
        pid_t p[N_CHLD];
        /* Set signal handler */
        struct sigaction act;
        memset(&act, 0, sizeof(struct sigaction));
        /* Block all signals */
        sigfillset(&act.sa_mask);
        sigdelset(&act.sa_mask, SIGCHLD);
        /* Set signal handler */
        act.sa_sigaction = signalHandler;
        act.sa_flags     = SA_SIGINFO;
        /* Handle on SIGCHLD */
        sigaction(SIGCHLD, &act, NULL);


        for (i = 0; i < N_CHLD; i++) {
                /* Increment the number of child processes */
                /* Create the child process */
                p[i] = fork();
                if (p[i] == -1) {
                        perror("Could not fork.");
                        exit(1);
                } else if (p[i] == 0) {
                        /* Child prints numbers */
                        for (j = i * 200; j <= (i + 5) * 200; j++) { printf("%d\n", j); }
                        /* This will automaticaly send a CHLD signal to the main process */
                        printf("DONE\n");
                        exit(0);
                }
        }

        /* Pause untill the counter is updated */
        while (counter < N_CHLD) { pause(); }

        /* Seems unecessary... */
        for (i = 0; i < N_CHLD; i++) { waitpid(p[i], NULL, 0); }

        return 0;
}
