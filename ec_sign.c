//
// Created by pd on 15/11/2021.
//
#include <stdlib.h>
#include <stdio.h>
#include "gmp.h"
#include "ec.h"
#include <time.h>
#include <openssl/md5.h>
#include <string.h>


int main(int argc, char *argv[]){

    if(argc < 3){
        return 0;
    }
    char *mess_file = argv[1];
    char *user_key = argv[2];
    char para_file[8] = "ec_";
    strcat(para_file, user_key);
    FILE *fm;
    //read curve's param
    fm = fopen(para_file, "r");
    mpz_t p, a, b, n;
    mpz_init(p);
    mpz_init(a);
    mpz_init(b);
    mpz_init(n);
    struct point G;
    mpz_init(G.x);
    mpz_init(G.y);
    gmp_fscanf(fm, "p = %Zd\nn = %Zd\na = %Zd\nb = %Zd\nG_x = %Zd\nG_y = %Zd\n", p, n, a, b, &G.x, &G.y);
    gmp_printf("p = %Zd\nn = %Zd\na = %Zd\nb = %Zd\nG_x = %Zd\nG_y = %Zd\n", p, n, a, b, &G.x, &G.y);
    fclose(fm);

    //read keys
    mpz_t d;
    mpz_init(d);
    strcat(para_file, "_pri");
    fm = fopen(para_file, "r");
    gmp_fscanf(fm, "d = %Zx\n", d);
    gmp_printf("d = %Zd\n", d);

    //hash mess
    fm = fopen(mess_file, "rb");
    MD5_CTX mdContext;
    MD5_Init(&mdContext);
    int bytes;
    unsigned char data[1024];
    unsigned char *mess_hash_str;
    size_t digest_len = mpz_sizeinbase(n, 16);
    printf("key size: %ld\n", digest_len);
    mess_hash_str = (unsigned char*)malloc(MD5_DIGEST_LENGTH*sizeof(char));
    while ((bytes = fread(data, 1, 1024, fm)) != 0){
        MD5_Update(&mdContext, data, bytes);
    }
    MD5_Final(mess_hash_str, &mdContext);


    //conversion
    char m_h[MD5_DIGEST_LENGTH*2 +1];
    char *m_hp = &m_h[0];
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        m_hp += sprintf(m_hp, "%x",(int) mess_hash_str[i]);
    }

    mpz_t mess_hash;
    mpz_init(mess_hash);
    mpz_set_str(mess_hash, m_h, 16);
    gmp_printf("\nhash value: %Zd\n", mess_hash);


    //find random k
    gmp_randstate_t r_gen;
    gmp_randinit_default(r_gen);
    gmp_randseed_ui(r_gen, time(NULL));
    mpz_t k;
    mpz_init(k);
    while (mpz_cmp_ui(k, 0) == 0){
        mpz_urandomm(k, r_gen, n);
    }
    //Compute k-1
    mpz_invert(k, k, n);

    //Compute P = kG
    struct point P;
    mpz_init(P.x);
    mpz_init(P.y);
    mpz_t r;
    mpz_init(r);

    multiple(&P, G, p, k, a, b);

    //Compute r = P.x (modn)
    mpz_mod(r, P.x, n);
    mpz_t sign;
    mpz_init_set(sign, mess_hash);
    mpz_addmul(sign, r, d);
    mpz_mul(sign, sign, k);
    mpz_mod(sign, sign, n);

    char sign_file[50] = "sign_";
    strcat(sign_file, "_");
    strcat(sign_file, user_key);
    strcat(sign_file, mess_file);

    freopen(sign_file, "w", stdout);
    gmp_printf("message hash = %s\nr = %Zd\ns = %Zd", m_h, r, sign);
    fclose(stdout);
    return 0;

}
