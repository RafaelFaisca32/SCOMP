EXTRACFLAGS = -g -ansi -pedantic -Wall -Wextra -Werror -Wno-unused-parameter

run: answer answer2 answer3
	$(info Running answer 1)
	./answer
	$(info Running answer 2)
	./answer2
	$(info Running answer 3)
	./answer3
	rm answer answer2 answer3
answer:
	$(CC) $(CFLAGS) $(EXTRACFLAGS) Answer.c -o answer

answer2:
	$(CC) $(CFLAGS) $(EXTRACFLAGS) Answer2.c -o answer2

answer3:
	$(CC) $(CFLAGS) $(EXTRACFLAGS) Answer3.c -o answer3