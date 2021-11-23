//
// Created by pd on 14/11/2021.
//
#include <stdlib.h>
#include <stdio.h>
#include "gmp.h"
#include "ec.h"
#include <time.h>
#include <string.h>
int main(int argc, char *argv[]){
    char user_key[4];
    char para_file[16] = "ec_";
    strcpy(user_key, (argc >= 2)? argv[1] : "224");

    FILE *fp;
    strcat(para_file, user_key);
    fp = fopen(para_file, "r");

    if(fp == NULL){
        perror("Failed: ");
        return 1;
    }

    //Read curve parameters
    mpz_t p, b, n, a;
    struct point G, Q;
    mpz_init(p);
    mpz_init(b);
    mpz_init(n);
    mpz_init(a);
    mpz_init(G.x);
    mpz_init(G.y);
    mpz_init(Q.x);
    mpz_init(Q.y);
    gmp_fscanf(fp, "p = %Zd\nn = %Zd\na = %Zd\nb = %Zd\nG_x = %Zd\nG_y = %Zd\n", p, n, a, b, &G.x, &G.y);
    gmp_printf("p = %Zx\nn = %Zx\na = %Zx\nb = %Zx\nG_x = %Zx\nG_y = %Zx\n", p, n, a, b, &G.x, &G.y);

    fclose(fp);

    //Gen d random
    gmp_randstate_t r_gen;
    gmp_randinit_default(r_gen);
    gmp_randseed_ui(r_gen, time(NULL));
    mpz_t d;
    mpz_init(d);
    while (mpz_cmp_ui(d, 0) == 0){
        mpz_urandomm(d, r_gen, n);
    }

    //Compute Q = dG
    multiple(&Q, G, p, d, a, b);
    gmp_printf("Q_x = %Zd\nQ_y = %Zd\n", &Q.x, &Q.y);


    char key_pri[16];
    memcpy(key_pri, para_file, strlen(para_file));
    strcat(para_file, "_pub");
    strcat(key_pri, "_pri");

    freopen(para_file, "w", stdout);
    gmp_printf("Q_x = %Zd\nQ_y = %Zd", Q.x, Q.y);

    freopen(key_pri, "w", stdout);
    gmp_printf("d = %Zd", d);
    fclose(stdout);

    return 0;




}
