#define _XOPEN_SOURCE 700
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

volatile sig_atomic_t isTaskADone = 0;

void taskA(pid_t);
void taskB();
void taskC();
void signalHandler(int, siginfo_t*, void*);

int main() {
        pid_t p = fork();
        if (p == -1) {
                perror("Could not fork.");
                exit(1);
        } else if (p > 0) {
                /* Parent process */
                /* Execute task A */
                taskA(p);
        } else {
                /* Child process */
                /* Set signal handler */
                struct sigaction act;
                memset(&act, 0, sizeof(struct sigaction));
                /* Mask all signals */
                sigfillset(&act.sa_mask);
                /* Set handler function */
                act.sa_sigaction = signalHandler;
                act.sa_flags     = SA_SIGINFO;
                /* Handle on SIGUSR1 */
                sigaction(SIGUSR1, &act, NULL);
                /* Execute task B */
                taskB();
                exit(0);
        }

        wait(NULL);
        return 0;
}

void taskA(pid_t childPid) {
        /* Execute tsk for 3 secconds */
        int i = 0;
        do {
                sleep(1);
                printf("A - %d seccond(s) elapsed...\n", ++i);
        } while (i != 3);
        /* Send signal to child */
        printf("A - Ended\n");
        kill(childPid, SIGUSR1);
}

void taskB() {
        /* To get random seed */
        time_t t;
        /* Time to wait in secconds */
        int waitTime;
        /* Iterator */
        int i;
        /* Get random time to wait */
        srand((unsigned)time(&t));
        waitTime = rand() % 4 + 1;

        /* Execute tsk for 3 secconds */
        i = 0;
        do {
                sleep(1);
                printf("B - %d seccond(s) elapsed...\n", ++i);
        } while (i != waitTime);
        printf("B - Ended\n");

        /* Wait for task A */
        /* Cannot use pause because the following
        operations may occur in the following order:
                1 ) (isTaskADone == 0) is evaluated to false
                2 ) taskA send signal to child
                3 ) signal handler sets isTaskADone to 1
                4 ) pause() starts listening for signals (SIGUSR1 had already been sent)
        in this order of operations pause() never unblocks the process.
        */
        while (isTaskADone == 0)
                ;

        /* Execute task C */
        taskC();
}

void taskC() {
        /* Execute tsk for 3 secconds */
        int i = 0;
        do {
                sleep(1);
                printf("C - %d seccond(s) elapsed...\n", ++i);
        } while (i != 3);
}

void signalHandler(int signum, siginfo_t* info, void* s) { isTaskADone = 1; }
