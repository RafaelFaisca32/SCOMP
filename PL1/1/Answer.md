# Answers

## 1

### a)

One possible output:

```txt
1. x = 2
2. x = 0
3. 0; x = 2
3. 1234; x = 0
```

Because the processes don't share memory on the stack.


### b)

No because there is a race condition on the printf on line 6 wich may complete before the printf on line 9.