#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define STATUS_RAW    0
#define STATUS_CUT    1
#define STATUS_FOLDED 2
#define STATUS_WELDED 3
#define STATUS_PACKED 4

#define M1_AT_A_TIME  5
#define M2_AT_A_TIME  5
#define M3_AT_A_TIME  10
#define M4_AT_A_TIME  100
#define TOT_PIECECES  1000
#define PIECE_RATE_MS 150
#define MAX_STR_SIZE  1000

typedef struct {
        int status;
        int id;
} piece;

void WorkA0(int p01[2], pid_t M1);
void WorkM1(int p01[2], int p12[2], pid_t M2);
void WorkM2(int p12[2], int p23[2], pid_t M3);
void WorkM3(int p23[2], int p34[2], pid_t M4);
void WorkM4(int p34[2]);

int main() {
        pid_t M1, M2, M3, M4;
        int   p01[2];

        /* Open pipe for comunication between A0 and M1 */
        if (pipe(p01) == -1) {
                /* Error */
                perror("Could not open pipe.\n");
                exit(1);
        }
        M1 = fork();
        if (M1 == -1) {
                /* Fork error */
                perror("Could not fork.\n");
                exit(1);
        } else if (M1 == 0) {
                /* Machine 1 */
                int p12[2];
                /* Close writing end of pipe */
                close(p01[1]);
                /* Open pipe for comunication between M1 and M2 */
                if (pipe(p12) == -1) {
                        /* Error */
                        perror("Could not open pipe.\n");
                }
                M2 = fork();
                if (M2 == -1) {
                        /* Fork error */
                        perror("Could not fork.\n");
                        exit(1);
                } else if (M2 == 0) {
                        /* Machine 2 */
                        int p23[2];
                        /* Close writing end of pipe */
                        close(p12[1]);
                        /* Open pipe for comunication between M2 and M3 */
                        if (pipe(p23) == -1) {
                                /* Error */
                                perror("Could not open pipe.\n");
                        }
                        M3 = fork();
                        if (M3 == -1) {
                                /* Fork error */
                                perror("Could not fork.\n");
                                exit(1);
                        } else if (M3 == 0) {
                                /* Machine 3 */
                                int p34[2];
                                /* Close writing end of pipe */
                                close(p23[1]);
                                /* Open pipe for comunication between M3 and M4 */
                                if (pipe(p34) == -1) {
                                        /* Error */
                                        perror("Could not open pipe.\n");
                                }
                                M4 = fork();
                                if (M4 == -1) {
                                        /* Fork error */
                                        perror("Could not fork.\n");
                                        exit(1);
                                } else if (M4 == 0) {
                                        /* Machine 4 */
                                        /* Close writing end of pipe */
                                        close(p34[1]);
                                        /* Start machine */
                                        WorkM4(p34);
                                        exit(0);
                                }
                                /* Close reading end of pipe */
                                close(p34[0]);
                                /* Start machine */
                                WorkM3(p23, p34, M4);
                                exit(0);
                        }
                        /* Close reading end of pipe */
                        close(p23[0]);
                        /* Start machine */
                        WorkM2(p12, p23, M3);
                        exit(0);
                }
                /* Close reading end of pipe */
                close(p12[0]);
                /* Start machine */
                WorkM1(p01, p12, M2);
                exit(0);
        }


        /* Storage 0 */
        /* Close reading end of pipe */
        close(p01[0]);
        /* Start machine */
        WorkA0(p01, M1);
        return 0;
}

/* This storage sends raw materials into machine 1 with a certain delay */
void WorkA0(int p01[2], pid_t M1) {
        int i;
        /* Set up sleep interval */
        struct timespec rt;
        rt.tv_sec  = 0;
        rt.tv_nsec = PIECE_RATE_MS * 1000000;
        /* Send the pices to M1 */
        for (i = 0; i < TOT_PIECECES; i++) {
                piece pi;
                /* Simulate delay between pieces coming in */
                nanosleep(&rt, NULL);
                /* Make piece */
                pi.status = STATUS_RAW;
                pi.id     = i;
                /* Send pice */
                write(p01[1], &pi, sizeof(piece));
        }
        /* Close pipe */
        close(p01[1]);
        /* Wait for M1 to finish */
        printf("M1: WAITING FOR SHUTDOWN\n");
        waitpid(M1, NULL, 0);
        printf("M1: SHUTING DOWN\n");
}

void WorkM1(int p01[2], int p12[2], pid_t M2) {
        piece pieces[M1_AT_A_TIME];
        int   pieceN = 0, i;

        for (;;) {
                if (read(p01[0], &pieces[pieceN], sizeof(piece)) < (long)sizeof(piece)) {
                        printf("M1: SHUTING DOWN\n");
                        close(p01[0]);
                        close(p12[1]);
                        waitpid(M2, NULL, 0);
                        exit(0);
                } else if (++pieceN == M1_AT_A_TIME) {
                        /* Used to buffer the output text */
                        char str[MAX_STR_SIZE] = "M1: CUTING PIECES [ ";
                        pieceN                 = 0;
                        /* Cut all pieces */
                        for (i = 0; i < M1_AT_A_TIME; i++) {
                                pieces[i].status = STATUS_CUT;
                                sprintf(str + strlen(str), "%d ", pieces[i].id);
                        }
                        strcat(str, "]\n");
                        write(STDOUT_FILENO, str, strlen(str));
                        /* Send pieces to M2 */
                        for (i = 0; i < M1_AT_A_TIME; i++) {
                                write(p12[1], &pieces[i], sizeof(piece));
                        }
                }
        }
}

void WorkM2(int p12[2], int p23[2], pid_t M3) {
        piece pieces[M2_AT_A_TIME];
        int   pieceN = 0, i;

        for (;;) {
                if (read(p12[0], &pieces[pieceN], sizeof(piece)) < (long)sizeof(piece)) {
                        printf("M2: SHUTING DOWN\n");
                        close(p12[0]);
                        close(p23[1]);
                        waitpid(M3, NULL, 0);
                        exit(0);
                } else if (++pieceN == M2_AT_A_TIME) {
                        /* Used to buffer the output text */
                        char str[MAX_STR_SIZE] = "M2: FOLDING PIECES [ ";
                        pieceN                 = 0;
                        /* Fold all pieces */
                        for (i = 0; i < M2_AT_A_TIME; i++) {
                                pieces[i].status = STATUS_FOLDED;
                                sprintf(str + strlen(str), "%d ", pieces[i].id);
                        }
                        strcat(str, "]\n");
                        write(STDOUT_FILENO, str, strlen(str));
                        /* Send pieces to M3 */
                        for (i = 0; i < M2_AT_A_TIME; i++) {
                                write(p23[1], &pieces[i], sizeof(piece));
                        }
                }
        }
}

void WorkM3(int p23[2], int p34[2], pid_t M4) {
        piece pieces[M3_AT_A_TIME];
        int   pieceN = 0, i;

        for (;;) {
                if (read(p23[0], &pieces[pieceN], sizeof(piece)) < (long)sizeof(piece)) {
                        printf("M3: SHUTING DOWN\n");
                        close(p23[0]);
                        close(p34[1]);
                        waitpid(M4, NULL, 0);
                        exit(0);
                } else if (++pieceN == M3_AT_A_TIME) {
                        /* Used to buffer the output text */
                        char str[MAX_STR_SIZE] = "M3: WELDING PIECES [ ";
                        pieceN                 = 0;
                        /* Weld all pieces */
                        for (i = 0; i < M3_AT_A_TIME; i++) {
                                pieces[i].status = STATUS_WELDED;
                                sprintf(str + strlen(str), "%d ", pieces[i].id);
                        }
                        strcat(str, "]\n");
                        write(STDOUT_FILENO, str, strlen(str));
                        /* Send pieces to M4 */
                        for (i = 0; i < M3_AT_A_TIME; i++) {
                                write(p34[1], &pieces[i], sizeof(piece));
                        }
                }
        }
}

void WorkM4(int p34[2]) {
        piece pieces[M4_AT_A_TIME];
        int   pieceN = 0, i;

        for (;;) {
                if (read(p34[0], &pieces[pieceN], sizeof(piece)) < (long)sizeof(piece)) {
                        printf("M4: SHUTING DOWN\n");
                        close(p34[0]);
                        exit(0);
                } else if (++pieceN == M4_AT_A_TIME) {
                        /* Used to buffer the output text */
                        char str[MAX_STR_SIZE] = "M4: PACKING PIECES [ ";
                        pieceN                 = 0;
                        /* Pack all pieces */
                        for (i = 0; i < M4_AT_A_TIME; i++) {
                                pieces[i].status = STATUS_PACKED;
                                sprintf(str + strlen(str), "%d ", pieces[i].id);
                        }
                        strcat(str, "]\n");
                        write(STDOUT_FILENO, str, strlen(str));
                        /* Simulate storing the pieces */
                        printf("M4: PIECES SENT TO STORAGE A1");
                }
        }
}
