EXTRACFLAGS = -I$(COMMON_FOLDER) -g -ansi -pedantic -Wall -Wextra -Werror -Wno-unused-parameter -lrt -lpthread -flto
COMMON_FOLDER = ../../common
COMMON_BASE = $(COMMON_FOLDER)/common

run: answer
	./answer
	rm answer

answer: $(COMMON_BASE).o
	$(CC) $(CFLAGS) $(EXTRACFLAGS) $(COMMON_BASE).o Answer.c -o answer

$(COMMON_BASE).o:
	$(CC) $(CFLAGS) -flto -c $(COMMON_BASE).c -o $(COMMON_BASE).o