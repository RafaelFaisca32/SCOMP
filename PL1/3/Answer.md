# Answers

## 3

### a)

The main process creates 7 additional processes.

### b)

```txt
                mian
                  |
          /----Fork();-----\
         |                 |
      fork();         fortk();
     |       |       |       |
  fork(); fork(); fork(); fork();
  |    |  |    |  |    |  |    |
  1    2  3    4  5    6  7    8
```

### c)

8