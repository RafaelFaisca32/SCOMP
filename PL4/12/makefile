EXTRACFLAGS = -I$(COMMON_FOLDER) -g -ansi -pedantic -Wall -Wextra -Werror -Wno-unused-parameter -lrt -lpthread
COMMON_FOLDER = ../../common
COMMON_BASE = $(COMMON_FOLDER)/common

run: seller client
	./seller & ./client & ./client & ./client & ./client & ./client
	rm seller client

seller: $(COMMON_BASE).o
	$(CC) $(CFLAGS) $(EXTRACFLAGS) $(COMMON_BASE).o Seller.c -o seller

client: $(COMMON_BASE).o
	$(CC) $(CFLAGS) $(EXTRACFLAGS) $(COMMON_BASE).o Client.c -o client

$(COMMON_BASE).o:
	$(CC) $(CFLAGS) -c $(COMMON_BASE).c -o $(COMMON_BASE).o