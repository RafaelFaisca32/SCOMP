EXTRACFLAGS = -I$(COMMON_FOLDER) -g -ansi -pedantic -Wall -Wextra -Werror -Wno-unused-parameter -Wc++-compat -lrt -lpthread -flto
COMMON_FOLDER = ../../common
COMMON_BASE = $(COMMON_FOLDER)/common

run: writer reader
	./writer
	./reader
	rm writer reader

writer: $(COMMON_BASE).o
	$(CC) $(CFLAGS) $(EXTRACFLAGS) $(COMMON_BASE).o Writer.c -o writer

reader: $(COMMON_BASE).o
	$(CC) $(CFLAGS) $(EXTRACFLAGS) $(COMMON_BASE).o Reader.c -o reader

$(COMMON_BASE).o:
	$(CC) $(CFLAGS) -flto -c $(COMMON_BASE).c -o $(COMMON_BASE).o