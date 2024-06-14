#define _XOPEN_SOURCE 700
#include <limits.h>
#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void handle_signal(int, siginfo_t*, void*);

int main() {
        /* Set signal handler */
        struct sigaction act;
        memset(&act, 0, sizeof(struct sigaction));
        /* Mask all signals */
        sigfillset(&act.sa_mask);
        /* Set handler function */
        act.sa_sigaction = handle_signal;
        act.sa_flags     = SA_SIGINFO;
        /* Handle on SIGUSR1 */
        sigaction(SIGUSR1, &act, NULL);
        /* Triger the signal handler */
        kill(getpid(), SIGUSR1);
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

void handle_signal(int signum, siginfo_t* info, void* s) {
        /* Signal handler */
        const char sigDetected[]   = "SIGNAL DETECTED [ ";
        const char sigUndetected[] = "SIGNAL IS NOT IN MASK [ ";
        const char endBraces[]     = " ]\n";
        /* Sigcode being iterated */
        int sigcode;
        /* The set of blocked signals */
        sigset_t sigs;
        /* Get the set of blocked signals */
        sigprocmask(SIG_BLOCK, NULL, &sigs);

        /* Iterate all signals until sigcode wraps around */
        /* There is no -standard- way of checking if sigset_t is empty */
        /* NSIG is not posix complient */
        /* The posix standard does not specify where signals begin/end */
        /* https://austingroupbugs.net/view.php?id=1138 */
        /* SIGRTMIN and SIGRTMAX only cover run time signals there may be
           others before or after them */
        for (sigcode = INT_MIN;; sigcode++) {
                /* Get info about sigcode */
                int sm = sigismember(&sigs, sigcode);
                /* Is the current sigcode is in the set? */
                if (sm == 1) {
                        /* Sigcode is in set */
                        write(STDOUT_FILENO, sigDetected, sizeof(sigDetected));
                } else if (sm == 0) {
                        /* Sigcode is valid but is not in set */
                        write(STDOUT_FILENO, sigUndetected, sizeof(sigUndetected));
                } else if (sm == -1) {
                        /* sigcode is not valid signal*/
                        if (sigcode == INT_MAX) break;
                        else
                                continue;
                }
                writeNumber(sigcode);
                write(STDOUT_FILENO, endBraces, sizeof(endBraces));
                /* Stop when sigcode reaches max value */
                if (sigcode == INT_MAX) break;
        }
}
