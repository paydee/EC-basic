//
// Created by pd on 22/11/2021.
//
#include <stdio.h>
#include "gmp.h"
#include "ec.h"
#include <string.h>


int main(int argc, char *argv[]){

    if(argc < 3){
        return 0;
    }
    char *sign_file = argv[1];
    char *user_key = argv[2];
    FILE *fm;


    //read curve's param
    char para_file[16] = "ec_";
    strcat(para_file, user_key);
    fm = fopen(para_file, "r");
    if (fm == NULL) {
        perror("Failed: ");
        return 1;
    }

    mpz_t p, a, b, n;
    mpz_init(p);
    mpz_init(a);
    mpz_init(b);
    mpz_init(n);
    struct point G;
    mpz_init(G.x);
    mpz_init(G.y);
    gmp_fscanf(fm, "p = %Zd\nn = %Zd\na = %Zd\nb = %Zd\nG_x = %Zd\nG_y = %Zd\n", p, n, a, b, &G.x, &G.y);
    gmp_printf("p = %Zx\nn = %Zx\na = %Zx\nb = %Zx\nG_x = %Zx\nG_y = %Zx\n", p, n, a, b, &G.x, &G.y);
    fclose(fm);

    //read public key
    strcat(para_file, "_pub");
    printf("keyfile: %s\n", para_file);
    struct point Q;
    mpz_init(Q.x);
    mpz_init(Q.y);
    fm = fopen(para_file, "r");
    if (fm == NULL) {
        perror("Failed: ");
        return 1;
    }
    gmp_fscanf(fm, "Q_x = %Zd\nQ_y = %Zd", &Q.x, &Q.y);
    gmp_printf("Q_x = %Zd\nQ_y = %Zd\n", Q.x, Q.y);
    fclose(fm);

//    Read signature
    fm = fopen(sign_file, "r");
    if (fm == NULL) {
        perror("Failed: ");
        return 1;
    }
    mpz_t r, mess_hash, sign;
    mpz_init(r);
    mpz_init(mess_hash);
    mpz_init(sign);
    gmp_fscanf(fm, "message hash = %Zx\nr = %Zd\ns = %Zd", mess_hash, r, sign);
    gmp_printf("message hash = %Zd\nr = %Zd\ns = %Zd\n", mess_hash, r, sign);

    //Compute sign = s-1; u1 = sign*z mod n, u2 = sign*r mod n
    mpz_invert(sign, sign, n);
    mpz_t u1, u2;
    mpz_init(u1);
    mpz_init(u2);
    mpz_mul(u1, sign, mess_hash);
    mpz_mod(u1, u1, n);

    mpz_mul(u2, sign, r);
    mpz_mod(u2, u2, n);

    struct point P, P1, P2;
    mpz_init(P.x);
    mpz_init(P.y);
    mpz_init(P1.x);
    mpz_init(P1.y);
    mpz_init(P2.x);
    mpz_init(P2.y);
    multiple(&P1, G, p, u1, a, b);
    multiple(&P2, Q, p, u2, a, b);
    addition(&P, P1, P2, p, a, b);
    printf((mpz_congruent_p(r, P.x, n) == 0) ? "Signature valid!\n" : "Signature invalid!\n");

    return 0;

}

