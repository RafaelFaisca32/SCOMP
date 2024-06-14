#define _XOPEN_SOURCE 700
#include <errno.h>
#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* Chance from 0 to 100 (int) that simula1 will succeed */
#define SUCCESS_CHANCE 100
/* Number of child processes to create */
#define CHLD_N 50
/* Number of child processes that need to have completed simula1 before the algorith is evaluated in
 * it's efficency */
#define CHLD_CUTOFF 25
/* Number of children that need to have successfuly completed simula1 for the algorithm to be deemed
 * efficient*/
#define USEFUL_CUTTOFF 1
/* Maximum simulated delay times for simula1 and simula2 range from 1 to MAX_WAIT_TIME+1 */
#define MAX_WAIT_TIME 20

/* Number of children who have successfuly completed simula1 */
volatile sig_atomic_t successN;
/* Number of children who have failed at completing simula1 */
volatile sig_atomic_t failedN;

/* Simulated functions */
int  simula1();
void simula2();
/* Signal handlers to increment successN and failedN */
void signalHandlerSuccess(int, siginfo_t*, void*);
void signalHandlerFail(int, siginfo_t*, void*);
/* Handler to tell child process to run simula2 */
void signalHandlerSimula2(int, siginfo_t*, void*);

int main() {
        /* Iterator and temporary variable */
        int i, signal;
        /* The pids of all child processes created*/
        pid_t cPids[CHLD_N];
        /* To define the signal handlers */
        struct sigaction act1, act2, act3;

        /* Initialize successN and failedN */
        successN = 0;
        failedN  = 0;

        /* Set signal handler to increment successN */
        memset(&act1, 0, sizeof(struct sigaction));
        /* Mask all signals */
        sigfillset(&act1.sa_mask);
        sigdelset(&act1.sa_mask, SIGUSR1);
        sigdelset(&act1.sa_mask, SIGUSR2);
        /* Set handler function */
        act1.sa_sigaction = signalHandlerSuccess;
        act1.sa_flags     = SA_SIGINFO;
        /* Handle on SIGUSR1 */
        sigaction(SIGUSR1, &act1, NULL);

        /* Set signal handler to increment failedN */
        memset(&act2, 0, sizeof(struct sigaction));
        /* Mask all signals */
        sigfillset(&act2.sa_mask);
        sigdelset(&act2.sa_mask, SIGUSR1);
        sigdelset(&act2.sa_mask, SIGUSR2);
        /* Set handler function */
        act2.sa_sigaction = signalHandlerFail;
        act2.sa_flags     = SA_SIGINFO;
        /* Handle on SIGUSR2 */
        sigaction(SIGUSR2, &act2, NULL);

        /* Set signal handler to start simula2 (for child processes)*/
        memset(&act3, 0, sizeof(struct sigaction));
        /* Mask all signals */
        sigfillset(&act3.sa_mask);
        /* Set handler function */
        act3.sa_sigaction = signalHandlerSimula2;
        act3.sa_flags     = SA_SIGINFO;
        /* Handle on SIGCONT */
        sigaction(SIGCONT, &act3, NULL);

        /* Create child processes */
        for (i = 0; i < CHLD_N; i++) {
                cPids[i] = fork();
                if (cPids[i] == -1) {
                        /* Forking error */
                        perror("Could not fork.");
                        exit(1);
                } else if (cPids[i] == 0) {
                        /* Needed to seed RNG */
                        time_t t;
                        /* Seed rng */
                        srand((unsigned)time(&t) + getpid());
                        /* Send signal to parrent depending on the result of simula 1 */
                        kill(getppid(), (simula1()) ? SIGUSR1 : SIGUSR2);
                        /* Wait for parent to send a response back */
                        pause();
                        /* Should never get here because should either end in the SIGCONT handler or
                         * the SIGKILL signal */
                        exit(1);
                }
        }

        /* Wait for enough processes to end simula 1 */
        while (failedN + successN < CHLD_CUTOFF) pause();

        /* Determine wether algorithm is efficient */
        if (successN < USEFUL_CUTTOFF) {
                /* Allgorithm is inefficient */
                printf("Inefficient algorithm!\n");
                /* Send a KILL signal to all child processes, ending them */
                signal = SIGKILL;
        } else {
                /* Send a CONT signal to all child processes, telling them to execute simula2 and to
                 * end */
                signal = SIGCONT;
        }
        /* Send signal to processes */
        for (i = 0; i < CHLD_N; i++) kill(cPids[i], signal);
        /* Wait for all processes to end */
        for (i = 0; i < CHLD_N; i++) waitpid(cPids[i], NULL, 0);

        return 0;
}

int simula1() {
        /* Determine wether function was successful */
        int success = rand() % 99 < SUCCESS_CHANCE;
        /* Simulate that the function is running */
        printf("%d is running simula 1\n", getpid());
        sleep(rand() % MAX_WAIT_TIME + 1);
        printf("%d is done running simula 1 [%s]\n", getpid(), (success) ? "SUCCESS" : "FAIL");
        return success;
}

void simula2() {
        /* Simulate that the function is running */
        printf("%d is running simula 2\n", getpid());
        sleep(rand() % MAX_WAIT_TIME + 1);
        printf("%d is done running simula 2\n", getpid());
}

void signalHandlerSuccess(int signum, siginfo_t* info, void* s) {
        /* Incremend successN */
        successN++;
}

void signalHandlerFail(int signum, siginfo_t* info, void* s) {
        /* Decrement successN */
        failedN++;
}

void signalHandlerSimula2(int signum, siginfo_t* info, void* s) {
        /* Run simula2 and exit */
        /* The function simula2 is not handler safe but that is ok because the
        handler is blocking all other signals and will terminate the process */
        simula2();
        exit(0);
}
