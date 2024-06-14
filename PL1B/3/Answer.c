#define _XOPEN_SOURCE 700
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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

void handle_USR1(int signo, siginfo_t* sinfo, void* context) {
        char str[] = "SIGUSR1 has been captured and was sent by the process with PID ";
        write(STDOUT_FILENO, str, sizeof(str));
        writeNumber(sinfo->si_pid);
        write(STDOUT_FILENO, "\n", 1);
}

int main(int argc, char* argv[]) {
        struct sigaction act;
        memset(&act, 0, sizeof(struct sigaction));
        sigemptyset(&act.sa_mask); /* No signalsblocked*/
        act.sa_sigaction = handle_USR1;
        act.sa_flags     = SA_SIGINFO;
        sigaction(SIGUSR1, &act, NULL);
}
