#define _XOPEN_SOURCE 700
#include <errno.h>
#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Flag that signifies that a command is EXECuting */
#define CMD_EXEC 1
/* Flag that signifies that no command is executing, they're STOPed */
#define CMD_STOP 0

/* The command struct */
typedef struct {
        char cmd[32];
        int  tempo;
} comando;

/* The status of a running command */
volatile sig_atomic_t commandStatus;
/* The signal handler function */
void signalHandler(int, siginfo_t*, void*);

int main() {
        /* Iterator */
        unsigned long i;
        /* To define the signal handler */
        struct sigaction act;
        /* To block other signals */
        sigset_t set;
        /* The array of commands */
        comando cmds[] = {
                {"./wait10", 20},
                {"./wait10", 11},
                {"./wait10", 9},
                {"./wait10", 1},
        };

        /* Block all possible signals except for SIGCHLD */
        sigfillset(&set);
        sigdelset(&set, SIGCHLD);
        pthread_sigmask(SIG_SETMASK, &set, NULL);

        /* Set signal handler to activate when a child's process ends */
        memset(&act, 0, sizeof(struct sigaction));
        /* Mask all signals */
        sigfillset(&act.sa_mask);
        /* Set handler function */
        act.sa_sigaction = signalHandler;
        act.sa_flags     = SA_SIGINFO;
        /* Handle on SIGCHLD */
        sigaction(SIGCHLD, &act, NULL);

        /* Iterate over all the commands */
        for (i = 0; i < sizeof(cmds) / sizeof(comando); i++) {
                pid_t p;
                /* Set the status of the current command to CMD_EXEC*/
                commandStatus = CMD_EXEC;
                /* Try to create a child process to execute the command */
                p = fork();
                if (p == -1) {
                        /* Forking error */
                        perror("Could not fork.");
                        exit(1);
                } else if (p == 0) {
                        /* Child process executes program */
                        /* Initiating argument_list */
                        char* argument_list[2];
                        argument_list[0] = cmds[i].cmd;
                        argument_list[1] = NULL;
                        /* trying to execute the program */
                        if (execvp(cmds[i].cmd, argument_list) == -1) {
                                /* Could not execute */
                                /* Make sure parrent doesn't kill this process */
                                commandStatus = CMD_STOP;
                                /* Display error */
                                fprintf(stderr, "Could not execvp (%s).", strerror(errno));
                                /* Exit */
                                exit(1);
                        };
                } else {
                        /* Parrent waits a certain amount of time or until the CHLD signal is
                         * recieved */
                        sleep(cmds[i].tempo);
                        /* If the command is executing when the sllep ends */
                        if (commandStatus == CMD_EXEC) {
                                /* Kill child process */
                                kill(p, SIGKILL);
                                /* Wait for the kernel to kill the process */
                                waitpid(p, NULL, 0);
                                /* Display message */
                                printf("The command %s didn’t end in its allowed time!\n",
                                       cmds[i].cmd);
                        }
                }
        }
        return 0;
}

void signalHandler(int signum, siginfo_t* info, void* s) {
        /* When a CHLD signal is received, that means that either the command has finished executing
        or the main process killed it, either way, the command execution is stoped */
        commandStatus = CMD_STOP;
}
