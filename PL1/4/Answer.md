# Answers

## 4

### a)

a

### b)

```
                                                                    |
                                                                    |
                                                                    | int a=0, b, c, d;
                                                                    | b = (int) fork();
                                                                   / \
                                                                  /   \
                                                                 /     \
                                                                /       \
c = (int) getpid(); /*getpid(), getppid(): unistd.h*/           |       | c = (int) getpid(); /*getpid(), getppid(): unistd.h*/
d = (int) getppid();                                            |       | d = (int) getppid();
a = a + 5;                                                      |       | a = a + 5;
printf("\na=%d, b=%d, c=%d, d=%d\n",a,b,c,d);                   |       | printf("\na=%d, b=%d, c=%d, d=%d\n",a,b,c,d);
```