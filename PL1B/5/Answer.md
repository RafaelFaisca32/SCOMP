# 5

## a)

When I press `CTRL-C` the terminal sends a `SIGINT` signal to the currently running process, terminating it.

## b)

File `Answer2.c`.

## c)

When I press `CTRL-\` the terminal sends a `SIGQUIT` signal to the currently running process, terminating it.

## d)

File `Answer3.c`.

## e)

When I send a `SIGINT` or a `SIGQUIT` signal the process, the program handles it displaying the appropriate message.

## f)

The `kill %1` command terminates the first job, a suspended/background process.

## g)

Because `write` is [signal-safe](https://man7.org/linux/man-pages//man7/signal-safety.7.html) but `printf` is not.