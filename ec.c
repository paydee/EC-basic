//
// Created by pd on 25/10/2021.
//
#include <stdlib.h>
#include <stdio.h>
#include "gmp.h"

struct point {
    mpz_t x;
    mpz_t y;
};

int on_curve(struct point P, mpz_t p, mpz_t a, mpz_t b){
    mpz_t y, x;
    mpz_init(y);
    mpz_init(x);

    // y = P.y^2
    mpz_pow_ui(y, P.y, 2);

    //x = P.x^3 + a.P.x + b
    mpz_pow_ui(x, P.x, 3);
    mpz_add(x, x, b);
    mpz_addmul(x, P.x, a);

    return (mpz_congruent_p(y, x, p) != 0 ||( mpz_cmp_ui(P.x, 0) == 0 && mpz_cmp_ui(P.y, 0) == 0)) ? 1 : 0;

}

int is_infinity(struct point P, mpz_t p){
    if(mpz_divisible_p(P.x, p) && mpz_divisible_p(P.y, p)){
        return 1;
    }
    return 0;
}

int is_neg(struct point P, struct point Q, mpz_t p){
    mpz_t y_neg;
    mpz_init(y_neg);
    mpz_neg(y_neg, P.y);
    return (mpz_congruent_p(y_neg, Q.y, p) != 0) ? 1 : 0;
}

int is_equal(struct point P, struct point Q, mpz_t p){
    return (mpz_congruent_p(P.x, Q.x, p) == 0 && mpz_congruent_p(P.y, Q.y, p) == 0) ? 1 : 0;
}

int addition(struct point *R, struct point P, struct point Q, mpz_t p, mpz_t a, mpz_t b){
    if(on_curve(P, p, a, b) == 0 || on_curve(Q, p, a, b) == 0){
        printf("One of the point is not on curve!\n");
        return -1;
    }
//    mpz_init(R->x);
//    mpz_init(R->y);
    if(is_infinity(P, p)){
        mpz_set(R->x, Q.x);
        mpz_set(R->y, Q.y);
        return 0;
    }
    if(is_infinity(Q, p)){
        mpz_set(R->x, P.x);
        mpz_set(R->y, P.y);
        return 0;

    }
    if(is_neg(P, Q, p)){
        mpz_set_ui(R->x, 0);
        mpz_set_ui(R->y, 0);
        return 0;
    }
    mpz_t lambda, temp;
    mpz_init(lambda);
    mpz_init(temp);
    if(mpz_cmp(P.x, Q.x) != 0){
        mpz_sub(lambda, Q.x, P.x);
        mpz_invert(lambda, lambda, p);
        mpz_sub(temp, Q.y, P.y);
        mpz_mul(lambda, lambda, temp);
        mpz_pow_ui(R->x, lambda, 2);
        mpz_sub(R->x, R->x, P.x);
        mpz_sub(R->x, R->x, Q.x);
        mpz_mod(R->x, R->x, p);

        mpz_sub(temp, P.x, R->x);

        mpz_neg(R->y, P.y);
        mpz_addmul(R->y, temp, lambda);
        mpz_mod(R->y, R->y, p);
    } else{
        mpz_add(temp, Q.y, P.y);
        mpz_invert(temp, temp, p);
        mpz_pow_ui(lambda, P.x, 2);
        mpz_mul_ui(lambda, lambda, 3);
        mpz_add(lambda,lambda, a);
        mpz_mul(lambda, lambda, temp);

        mpz_mul(R->x, lambda, lambda);
        mpz_neg(temp, P.x);
        mpz_addmul_ui(R->x, temp, 2);
        mpz_mod(R->x, R->x, p);

        mpz_neg(R->y, P.y);
        mpz_neg(lambda, lambda);
        mpz_add(temp, R->x, temp);
        mpz_addmul(R->y, lambda, temp);
        mpz_mod(R->y, R->y, p);
    }
//    gmp_printf("add x= %Zd, y = %Zd\n", R->x, R->y);

    return 0;
}
int double_point(struct point *R, struct point P, mpz_t p, mpz_t a, mpz_t b){

    if(!on_curve(P, p, a, b)){
        return -1;
    }


    if(is_infinity(P, p)){
        mpz_set(R->x, P.x);
        mpz_set(R->y, P.y);
        return 0;
    }

    mpz_t lambda, temp;
    mpz_init(lambda);
    mpz_init(temp);
    mpz_add(temp, P.y, P.y);
    mpz_invert(temp, temp, p);
    mpz_pow_ui(lambda, P.x, 2);
    mpz_mul_ui(lambda, lambda, 3);
    mpz_add(lambda,lambda, a);
    mpz_mul(lambda, lambda, temp);

    mpz_mul(R->x, lambda, lambda);
    mpz_neg(temp, P.x);
    mpz_addmul_ui(R->x, temp, 2);
    mpz_mod(R->x, R->x, p);

    mpz_neg(R->y, P.y);
    mpz_neg(lambda, lambda);
    mpz_add(temp, R->x, temp);
    mpz_addmul(R->y, lambda, temp);
    mpz_mod(R->y, R->y, p);
    return 0;
}

int multiple(struct point *R, struct point P, mpz_t p, mpz_t k, mpz_t a, mpz_t b){
//    gmp_printf("p = %Zd\nb = %Zx\na = %Zd\nGx = %Zx\nGy = %Zx\n", p, b, a, P.x, P.y);
    if(!on_curve(P, p, a, b)){
        return -1;
    }

    char *bin;
    bin = (char*)malloc((mpz_sizeinbase(k,2)+2)*sizeof(char));
    mpz_get_str(bin, 2, k);
    int i = 0;
    struct point T;
    mpz_init_set(T.x, P.x);
    mpz_init_set(T.y, P.y);

    while (bin[i] != '\0'){
        double_point(R, T, p, a, b);
        mpz_set(T.x, R->x);
        mpz_set(T.y, R->y);
//        gmp_printf("Tx= %Zd, Ty = %Zd\n", T.x, T.y);

        if(bin[i] == '1'){
            addition(R, T, P, p, a, b);
        }
        i++;
    }
    return 0;

}

//int main(int argc, char *argv[]){
//    mpz_t p, b, n, a;
//    struct point G;
//    mpz_init_set_si(a, -3);
//    mpz_init_set_str(p, "6277101735386680763835789423207666416083908700390324961279", 10);
//    mpz_init_set_str(b, "64210519e59c80e70fa7e9ab72243049feb8deecc146b9b1", 16);
//    mpz_init_set_str(n, "6277101735386680763835789423176059013767194773182842284081", 10);
//    mpz_init_set_str(G.x, "188da80eb03090f67cbf20eb43a18800f4ff0afd82ff1012", 16);
//    mpz_init_set_str(G.y, "07192b95ffc8da78631011ed6b24cdd573f977a11e794811", 16);
//    gmp_printf("p = %Zd\nb = %Zd\na = %Zd\nn = %Zd\nGx = %Zd\nGy = %Zd\n", p, b, a, n, G.x, G.y);
//    if(on_curve(G, p, a, b) == 1){
//        printf("G is on curve!\n");
//    } else{
//        printf("G is not on curve!\n");
//    }
//    printf("Size in base of b: %ld size of b(mpz_t): %ld size of char: %ld\n", mpz_sizeinbase(b, 2), sizeof(b), sizeof(char));
//
////    test = (char*) malloc(1);
////    printf("size of string before cast: %ld\n", strlen(test));
////    mpz_get_str(test, 2, b);
////    printf("%s size: %ld\n", test, sizeof(test));
////    int i = 0;
////    while (test[i] != '\0'){
////        printf("%c", test[i]);
////        i ++;
////    }
////    printf("\nstring length = %ld bit length = %d size of one bit: %ld \n", strlen(test), i, sizeof(test[0]));
//
//    return 0;
//}
