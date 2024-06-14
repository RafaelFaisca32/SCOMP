/* To compile with -ansi and -pedantic */
#define _XOPEN_SOURCE 700
#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/* Number of signals to send from child process */
#define SIGNALS_N 12

void handle_signal(int, siginfo_t*, void*);

int main() {
        pid_t            p;
        struct sigaction act;
        /* Set signal handler */
        memset(&act, 0, sizeof(struct sigaction));
        /* Block no signals */
        sigemptyset(&act.sa_mask);
        /* Set handler */
        act.sa_sigaction = handle_signal;
        act.sa_flags     = SA_SIGINFO;
        /* Handle on SIGUSR1 */
        sigaction(SIGUSR1, &act, NULL);

        /* Child processes */
        p = fork();
        if (p == -1) {
                perror("Could not fork.");
                exit(1);
        } else if (p == 0) {
                /* This is the child process */
                struct timespec time;
                int             i;
                /* Get parent's PID*/
                pid_t ppid = getppid();
                /* Define the signals we're going to send*/
                int sigs[SIGNALS_N] = {SIGUSR1, SIGUSR2, SIGINT, SIGUSR1, SIGUSR2, SIGINT,
                                       SIGUSR1, SIGUSR2, SIGINT, SIGUSR1, SIGUSR2, SIGINT};
                /* Time between signals*/
                time.tv_sec  = 0;
                time.tv_nsec = 10000000;
                /* Send signals */
                for (i = 0; i < SIGNALS_N; i++) {
                        kill(ppid, sigs[i]);
                        nanosleep(&time, NULL);
                }
                /* End child */
                exit(0);
        }

        /* Infinite loop */
        for (;;) {
                printf("I’m working!”\n");
                sleep(1);
        }
        return 0;
}

void writeNumber(unsigned int number) {
        /* itoa not signal-safe? */
        int        j;
        char       buffer[30];
        const char digits[] = "0123456789";
        if (number == 0) {
                write(STDOUT_FILENO, digits, 1);
        } else {
                for (j = 0; number > 0; number /= 10) buffer[j++] = number % 10;
                for (j = j - 1; j > -1; j--) write(STDOUT_FILENO, digits + buffer[j], 1);
        }
}

volatile sig_atomic_t USR1_COUNTER = 0;
void                  handle_signal(int signum, siginfo_t* info, void* s) {
        const char signalCaptured[]   = "SIGUSR1 signal captured, USR1_COUNTER = ";
        const char handlerCompleted[] = "SIGUSR1 handler completed.";
        USR1_COUNTER += 1;

        write(STDOUT_FILENO, signalCaptured, sizeof(signalCaptured));
        writeNumber(USR1_COUNTER);
        write(STDOUT_FILENO, "\n", 1);
        /* sleep is signal safe */
        sleep(2);
        write(STDOUT_FILENO, handlerCompleted, sizeof(handlerCompleted));
}
