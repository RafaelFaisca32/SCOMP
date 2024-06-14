EXTRACFLAGS = -I$(COMMON_FOLDER) -g -ansi -pedantic -Wall -Wextra -Werror -Wno-unused-parameter -lrt -lpthread
COMMON_FOLDER = ../../common
COMMON_BASE = $(COMMON_FOLDER)/common

run: writer reader
	./writer & ./reader & ./writer & ./writer & ./reader & ./reader
	rm writer reader

writer: $(COMMON_BASE).o
	$(CC) $(CFLAGS) $(EXTRACFLAGS) $(COMMON_BASE).o Writer.c -o writer

reader: $(COMMON_BASE).o
	$(CC) $(CFLAGS) $(EXTRACFLAGS) $(COMMON_BASE).o Reader.c -o reader

$(COMMON_BASE).o:
	$(CC) $(CFLAGS) -c $(COMMON_BASE).c -o $(COMMON_BASE).o