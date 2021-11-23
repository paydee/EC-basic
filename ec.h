//
// Created by pd on 15/11/2021.
//
#include <stdlib.h>
#include <stdio.h>
#include "gmp.h"
struct point {
    mpz_t x;
    mpz_t y;
};
int on_curve(struct point P, mpz_t p, mpz_t a, mpz_t b);
int is_infinity(struct point P, mpz_t p);
int is_neg(struct point P, struct point Q, mpz_t p);
int is_equal(struct point P, struct point Q, mpz_t p);
int addition(struct point *R, struct point P, struct point Q, mpz_t p, mpz_t a, mpz_t b);
int double_point(struct point *R, struct point P, mpz_t p, mpz_t a, mpz_t b);
int multiple(struct point *R, struct point P, mpz_t p, mpz_t k, mpz_t a, mpz_t b);

#ifndef TP_EC_H
#define TP_EC_H

#endif //TP_EC_H
