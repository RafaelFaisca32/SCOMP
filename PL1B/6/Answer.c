/* To compile with -ansi and -pedantic */
#define _XOPEN_SOURCE 700
#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

/* Number of signals to send from child process */
#define SIGNALS_N 12

void handle_signal(int, siginfo_t*, void*);

int main() {
        struct sigaction act;
        /* Set signal handler */
        memset(&act, 0, sizeof(struct sigaction));
        /* Block all signals */
        sigfillset(&act.sa_mask);
        /* Set handler */
        act.sa_sigaction = handle_signal;
        act.sa_flags     = SA_SIGINFO;
        /* Handle on SIGUSR1 */
        sigaction(SIGUSR1, &act, NULL);

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
        const char str[] = "SIGUSR1 signal captured, USR1_COUNTER = ";
        USR1_COUNTER += 1;

        write(STDOUT_FILENO, str, sizeof(str));
        writeNumber(USR1_COUNTER);
        write(STDOUT_FILENO, "\n", 1);
}
