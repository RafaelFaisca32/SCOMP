EXTRACFLAGS = -g -ansi -pedantic -Wall -Wextra -Werror -Wno-unused-parameter

run: answer
	./answer
	rm answer
answer:
	$(CC) $(CFLAGS) $(EXTRACFLAGS) Answer.c -o answer