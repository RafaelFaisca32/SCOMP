EXTRACFLAGS = -g -ansi -pedantic -Wall -Wextra -Werror -Wno-unused-parameter

run: answer wait10
	./answer
	rm answer wait10
answer:
	$(CC) $(CFLAGS) $(EXTRACFLAGS) Answer.c -o answer

wait10:
	$(CC) $(CFLAGS) $(EXTRACFLAGS) Answer2.c -o wait10