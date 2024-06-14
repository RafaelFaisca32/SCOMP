#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define PRODUCT_N 10
#define CHLD_N    5
/* Minimum number there can be in a bet */
#define MIN_RDM 3
/* Maximum number there can be in a bet */
#define MAX_RDM 25

typedef unsigned long barcode_t;
typedef unsigned int  eur_t;
typedef unsigned char cnt_t;

typedef struct {
        pid_t pid;
        int   fd[2];
} pidPipe;

typedef struct {
        barcode_t barcode;
        char*     name;
        eur_t     price_eur;
        cnt_t     price_cnt;
} productInfo;

typedef struct {
        barcode_t barcode;
        int       child;
} barcodeRequest;

/* Helper function to generate random numbers */
int getRandomN() { return rand() % (MAX_RDM - MIN_RDM) + MIN_RDM; }

void initializeProductDatabase(barcode_t (*)[PRODUCT_N], productInfo (*)[PRODUCT_N]);

int main() {
        int         sharedFd[2], i;
        barcode_t   barcodes[PRODUCT_N];
        productInfo productDatabase[PRODUCT_N];
        pidPipe     ppis[CHLD_N];
        char*       barcodesp;
        time_t      t;

        /* Seed RNG */
        srand(time(&t));

        /* Generate random barcodes */
        for (barcodesp = (char*)&barcodes[0]; barcodesp < (char*)&barcodes[PRODUCT_N];
             barcodesp++) {
                *barcodesp = (char)rand();
        }

        /* Attempt to open pipe */
        if (pipe(sharedFd) == -1) {
                /* Error */
                perror("Could not open pipe.\n");
                exit(1);
        }

        for (i = 0; i < CHLD_N; i++) {
                /* Attempt to open pipe */
                if (pipe(ppis[i].fd) == -1) {
                        /* Error */
                        perror("Could not open pipe.\n");
                        exit(1);
                }

                ppis[i].pid = fork();

                if (ppis[i].pid == -1) {
                        /* Fork error */
                        perror("Could not fork.\n");
                        exit(1);
                } else if (ppis[i].pid == 0) {
                        /* Child */
                        barcodeRequest req;
                        productInfo    prod;
                        size_t         nameSize;
                        /* Close writing end of pipe */
                        close(ppis[i].fd[1]);
                        /* Set correct child num */
                        req.child = i;
                        /* Seed rng */
                        srand(time(&t) + getpid());
                        /* Main loop */
                        for (;;) {
                                /* Simulate delay between barcode scans */
                                printf("CLIENT:%ld: SLEEPING...\n", (long)getpid());
                                sleep(getRandomN());
                                /* Place a request for a product */
                                /* Choose a random barcode */
                                req.barcode = barcodes[rand() % PRODUCT_N];
                                printf("CLIENT:%ld: REQUESTING PRODUCT WITH BARCODE %lu\n",
                                       (long)getpid(), req.barcode);
                                /* Write request to global pipe */
                                write(sharedFd[1], &req, sizeof(barcodeRequest));
                                /* Read data from singular pipe */
                                /* Read barcode (can be omited if desired) */
                                read(ppis[i].fd[0], &prod.barcode, sizeof(prod.barcode));
                                /* Read name length */
                                read(ppis[i].fd[0], &nameSize, sizeof(size_t));
                                /* Alocate space for the name */
                                prod.name = malloc(nameSize);
                                if (prod.name == NULL) {
                                        /* Malloc error */
                                        perror("Could not malloc.\n");
                                        exit(1);
                                }
                                /* Read name */
                                read(ppis[i].fd[0], prod.name, nameSize);
                                /* Read price (euros) */
                                read(ppis[i].fd[0], &prod.price_eur, sizeof(prod.price_eur));
                                /* Read price (cents) */
                                read(ppis[i].fd[0], &prod.price_cnt, sizeof(prod.barcode));
                                /* Display product*/
                                printf("CLIENT:%ld: GOT PRODUCT\n\tBARCODE: %lu\n\tNAME: "
                                       "%s\n\tPRICE: %u.%u\n",
                                       (long)getpid(), prod.barcode, prod.name, prod.price_eur,
                                       (unsigned)prod.price_cnt);
                                /* Free name */
                                free(prod.name);
                        }
                } else {
                        /* Parent */
                        /* Close reading end of pipe */
                        close(ppis[i].fd[0]);
                }
        }

        /* Initialize the product database */
        initializeProductDatabase(&barcodes, &productDatabase);

        /* Close writing end of pipe */
        close(sharedFd[1]);
        /* Main loop */
        for (;;) {
                /* Read request */
                barcodeRequest br;
                printf("SERVER: WAITING FOR REQUESTS...\n");
                if (read(sharedFd[0], &br, sizeof(barcodeRequest)) != sizeof(barcodeRequest)) {
                        /* Error */
                        perror("Could not read from pipe.\n");
                        exit(1);
                }
                /* Display message */
                printf("SERVER: GOT REQUEST FOR BARCODE [%lu]\n", br.barcode);
                /* Look for product in product database */
                for (i = 0; i < PRODUCT_N; i++) {
                        /* Product found */
                        if (productDatabase[i].barcode == br.barcode) {
                                int    fd       = ppis[br.child].fd[1];
                                size_t nameSize = strlen(productDatabase[i].name) + 1;
                                printf("SERVER: FOUND PRODUCT, NOW SENDING\n");
                                /* Send barcode (can be omited if desired) */
                                write(fd, &productDatabase[i].barcode,
                                      sizeof(productDatabase[i].barcode));
                                /* Send name length */
                                write(fd, &nameSize, sizeof(size_t));
                                /* Send name */
                                write(fd, productDatabase[i].name, nameSize);
                                /* Send price (euros) */
                                write(fd, &productDatabase[i].price_eur,
                                      sizeof(productDatabase[i].price_eur));
                                /* Send price (cents) */
                                write(fd, &productDatabase[i].price_cnt,
                                      sizeof(productDatabase[i].price_cnt));
                        }
                }
        }
        return 0;
}

void initializeProductDatabase(barcode_t (*barcodes)[PRODUCT_N],
                               productInfo (*productDatabase)[PRODUCT_N]) {
        (*productDatabase)[0].barcode   = (*barcodes)[0];
        (*productDatabase)[0].name      = "Conta-km Bicicleta GPS BRYTON Rider 15 BRYTON";
        (*productDatabase)[0].price_eur = 59;
        (*productDatabase)[0].price_cnt = 0;

        (*productDatabase)[1].barcode = (*barcodes)[1];
        (*productDatabase)[1].name =
                "RELÓGIO GPS DE CORRIDA COM SENSOR CARDIO NO PULSO M430 Preto POLAR";
        (*productDatabase)[1].price_eur = 140;
        (*productDatabase)[1].price_cnt = 40;

        (*productDatabase)[2].barcode = (*barcodes)[2];
        (*productDatabase)[2].name =
                "Lanterna frontal de acampamento a pilhas - ONNIGHT 50 - 30 lúmenes FORCLAZ";
        (*productDatabase)[2].price_eur = 4;
        (*productDatabase)[2].price_cnt = 40;

        (*productDatabase)[3].barcode = (*barcodes)[3];
        (*productDatabase)[3].name =
                "Walkie-talkies recarregável por USB - WT TREK 500 - 10km - 2 Unidades FORCLAZ";
        (*productDatabase)[3].price_eur = 50;
        (*productDatabase)[3].price_cnt = 10;

        (*productDatabase)[4].barcode   = (*barcodes)[4];
        (*productDatabase)[4].name      = "Balança Impedancímetro SCALE 500 em Vidro NEWFEEL";
        (*productDatabase)[4].price_eur = 20;
        (*productDatabase)[4].price_cnt = 0;

        (*productDatabase)[5].barcode   = (*barcodes)[5];
        (*productDatabase)[5].name      = "MINI BIKE DOMYOS";
        (*productDatabase)[5].price_eur = 45;
        (*productDatabase)[5].price_cnt = 20;

        (*productDatabase)[6].barcode   = (*barcodes)[6];
        (*productDatabase)[6].name      = "Corda de saltar JR100 DOMYOS";
        (*productDatabase)[6].price_eur = 1;
        (*productDatabase)[6].price_cnt = 95;

        (*productDatabase)[7].barcode   = (*barcodes)[7];
        (*productDatabase)[7].name      = "Trampolim de cardio training 100 DOMYOS";
        (*productDatabase)[7].price_eur = 35;
        (*productDatabase)[7].price_cnt = 0;

        (*productDatabase)[8].barcode   = (*barcodes)[8];
        (*productDatabase)[8].name      = "Passadeira de Treino Sem Motor W100 DOMYOS";
        (*productDatabase)[8].price_eur = 299;
        (*productDatabase)[8].price_cnt = 99;

        (*productDatabase)[9].barcode = (*barcodes)[9];
        (*productDatabase)[9].name = "Reativador anti-embaciamento para óculos de natação NABAIJI";
        (*productDatabase)[9].price_eur = 5;
        (*productDatabase)[9].price_cnt = 50;
}
