#define _XOPEN_SOURCE 700
#include <signal.h>
#include <stdio.h>

int main() {
        /* The pid of the program (never longer than long in any implementation) */
        long pid = -1;
        /* What to do */
        int action = -1;
        /* The sigal to send */
        int sig = -1;
        /* Helper to find if signal is valid */
        sigset_t helper;

        /* Read pid */
        printf("ENTER PID > ");
        scanf("%ld", &pid);

        /* Select action */
        do {
                printf("1) HALT    2) RESUME    3) QUIT    4) TERM    5) CUSTOM\nSELECT ACTION>");
                scanf("%d", &action);
        } while (action < 1 || action > 5);

        /* React accordingly to action */
        switch (action) {
                case 1: sig = SIGTSTP; break;
                case 2: sig = SIGCONT; break;
                case 3: sig = SIGQUIT; break;
                case 4: sig = SIGTERM; break;
                case 5:
                        /* Fill helper set */
                        sigfillset(&helper);
                        /* Continue to ask for signal while it is not in the set */
                        do {
                                printf("ENTER SIGNAL NUMBER > ");
                                scanf("%d", &sig);
                        } while (sigismember(&helper, sig) != 1);
                        break;
        }
        /* Send signal */
        kill(pid, sig);
        return 0;
}
