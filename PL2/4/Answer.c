#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* Number of bytes read at a time */
#define PACKET_SIZE 500

int main() {
        pid_t p;
        int   fd[2];
        char* filename = "ler.txt";

        /* Create pipe */
        if (pipe(fd) == -1) {
                perror("Could not create pipe.");
                exit(1);
        }

        /* Fork */
        p = fork();
        if (p == -1) {
                perror("Could not fork.");
                exit(1);
        } else if (p > 0) {
                /* Parent */
                FILE*  file;
                char   packet[PACKET_SIZE];
                size_t read;
                /* Close reading end of pipe */
                close(fd[0]);
                /* Open file for reading */
                file = fopen(filename, "r");
                if (file == NULL) {
                        perror("File could not be opened for reading");
                        exit(1);
                }
                /* While the EOF is not reached */
                while (!feof(file)) {
                        /* Atempt to read PACKET_SIZE bytes of file */
                        read = fread(packet, 1, PACKET_SIZE, file);
                        /* Write the data that was read to the pipe */
                        write(fd[1], packet, read);
                }
                /* Close pipe */
                close(fd[1]);
                /* Close file */
                fclose(file);
                /* Wait for child to terminate */
                waitpid(p, NULL, 0);
        } else if (p == 0) {
                /* Child */
                ssize_t length;
                char    packet[PACKET_SIZE];
                /* Close writing-end of file */
                close(fd[1]);
                /* While we're able to read from the pipe... */
                while ((length = read(fd[0], packet, PACKET_SIZE)) > 0) {
                        /* Output the data that was read */
                        write(STDOUT_FILENO, packet, length);
                }
                /* Close pipe */
                close(fd[0]);
                /* Terminate child */
                exit(0);
        }

        return 0;
}
