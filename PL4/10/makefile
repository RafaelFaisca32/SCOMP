EXTRACFLAGS = -I$(COMMON_FOLDER) -g -ansi -pedantic -Wall -Wextra -Werror -Wno-unused-parameter -lrt -lpthread
COMMON_FOLDER = ../../common
COMMON_BASE = $(COMMON_FOLDER)/common

all: insert consult
	$(info "")

insert: $(COMMON_BASE).o
	$(CC) $(CFLAGS) $(EXTRACFLAGS) $(COMMON_BASE).o Insert.c -o insert

consult: $(COMMON_BASE).o
	$(CC) $(CFLAGS) $(EXTRACFLAGS) $(COMMON_BASE).o Consult.c -o consult

$(COMMON_BASE).o:
	$(CC) $(CFLAGS) -c $(COMMON_BASE).c -o $(COMMON_BASE).o

clean:
	rm insert consult