# 6

## a)
## b)
File `Answer.c`.
When running this program the process is interupted by the signal handler when `SIGUSR1` is sent.

## c)
File `Answer2.c`.
When running this program the main process is interupted by the child process when it sends the signal `SIGUSR1` to it's parent, the parent process is also kiled when other signals are sent.

## d)
File `Answer3.c`.
This program differs from the one in `c)` because the signal handler takes a long time to run, and when it is running all the other signals sent from the child process are blocked, making it so that the main process is not terminated.

## e)
File `Answer4.c`
This program differs from the one in `d)` because the signal handler takes a long time to run, however other signals sent from the child process are not blocked, making it so that the main process is terminated even when the signal handler is running.
