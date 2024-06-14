EXTRACFLAGS = -g -ansi -pedantic -Wall -Wextra -Werror -Wno-unused-parameter

run: answer answer2
	$(info Running answer 1)
	./answer
	$(info Running answer 2)
	./answer2
	rm answer answer2
answer:
	$(CC) $(CFLAGS) $(EXTRACFLAGS) Answer.c -o answer

answer2:
	$(CC) $(CFLAGS) $(EXTRACFLAGS) Answer2.c -o answer2