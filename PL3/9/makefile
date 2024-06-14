EXTRACFLAGS = -I$(COMMON_FOLDER) -g -ansi -pedantic -Wall -Wextra -Werror -Wno-unused-parameter -lrt -lpthread
COMMON_FOLDER = ../../common
COMMON_BASE = $(COMMON_FOLDER)/common

run: Answer
	./Answer
	rm Answer

Answer: $(COMMON_BASE).o
	$(CC) $(CFLAGS) $(EXTRACFLAGS) $(COMMON_BASE).o Answer.c -o Answer

$(COMMON_BASE).o:
	$(CC) $(CFLAGS) -c $(COMMON_BASE).c -o $(COMMON_BASE).o