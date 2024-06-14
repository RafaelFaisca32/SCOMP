# 7

In my desktop, when I run the program I get the following output:

``` TXT
SIGNAL DETECTED [ 1 ]
SIGNAL DETECTED [ 2 ]
SIGNAL DETECTED [ 3 ]
SIGNAL DETECTED [ 4 ]
SIGNAL DETECTED [ 5 ]
SIGNAL DETECTED [ 6 ]
SIGNAL DETECTED [ 7 ]
SIGNAL DETECTED [ 8 ]
SIGNAL IS NOT IN MASK [ 9 ]
SIGNAL DETECTED [ 10 ]
SIGNAL DETECTED [ 11 ]
SIGNAL DETECTED [ 12 ]
SIGNAL DETECTED [ 13 ]
SIGNAL DETECTED [ 14 ]
SIGNAL DETECTED [ 15 ]
SIGNAL DETECTED [ 16 ]
SIGNAL DETECTED [ 17 ]
SIGNAL DETECTED [ 18 ]
SIGNAL IS NOT IN MASK [ 19 ]
SIGNAL DETECTED [ 20 ]
SIGNAL DETECTED [ 21 ]
SIGNAL DETECTED [ 22 ]
SIGNAL DETECTED [ 23 ]
SIGNAL DETECTED [ 24 ]
SIGNAL DETECTED [ 25 ]
SIGNAL DETECTED [ 26 ]
SIGNAL DETECTED [ 27 ]
SIGNAL DETECTED [ 28 ]
SIGNAL DETECTED [ 29 ]
SIGNAL DETECTED [ 30 ]
SIGNAL DETECTED [ 31 ]
SIGNAL IS NOT IN MASK [ 32 ]
SIGNAL IS NOT IN MASK [ 33 ]
SIGNAL DETECTED [ 34 ]
SIGNAL DETECTED [ 35 ]
SIGNAL DETECTED [ 36 ]
SIGNAL DETECTED [ 37 ]
SIGNAL DETECTED [ 38 ]
SIGNAL DETECTED [ 39 ]
SIGNAL DETECTED [ 40 ]
SIGNAL DETECTED [ 41 ]
SIGNAL DETECTED [ 42 ]
SIGNAL DETECTED [ 43 ]
SIGNAL DETECTED [ 44 ]
SIGNAL DETECTED [ 45 ]
SIGNAL DETECTED [ 46 ]
SIGNAL DETECTED [ 47 ]
SIGNAL DETECTED [ 48 ]
SIGNAL DETECTED [ 49 ]
SIGNAL DETECTED [ 50 ]
SIGNAL DETECTED [ 51 ]
SIGNAL DETECTED [ 52 ]
SIGNAL DETECTED [ 53 ]
SIGNAL DETECTED [ 54 ]
SIGNAL DETECTED [ 55 ]
SIGNAL DETECTED [ 56 ]
SIGNAL DETECTED [ 57 ]
SIGNAL DETECTED [ 58 ]
SIGNAL DETECTED [ 59 ]
SIGNAL DETECTED [ 60 ]
SIGNAL DETECTED [ 61 ]
SIGNAL DETECTED [ 62 ]
SIGNAL DETECTED [ 63 ]
SIGNAL DETECTED [ 64 ]
```

So it appears that all signals are blocked except for 9, 19, 32, and 33.

On my system signals 9 and 19 are `KILL` and `STOP` wich cannot be masked as they are handled by the kernel.

The commands `kill -l 32` and `kill -l 33` only output `32` and `33` but trough some online searching I found that these signal codes may be reserved for use by the kernel.

> The Linux kernel supports a range of 33 different real-time
> signals, numbered 32 to 64. However, the glibc POSIX threads
> implementation internally uses two (for NPTL) or three (for
> LinuxThreads) real-time signals (see pthreads(7)), and adjusts
> the value of SIGRTMIN suitably (to 34 or 35).

Source: [`man signal`](https://man7.org/linux/man-pages/man7/signal.7.html)