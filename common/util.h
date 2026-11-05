#ifndef UTIL_H
#define UTIL_H
#include "core.h"
const char *fvec(const double *v, int n);
void out_mat(const char *name, const double *m, int r, int c);
double peval_r(const double *a, int d, double x);
void pdiv(const double *a, int da, const double *b, int db, double *q, double *rem);
void pmul(const double *a, int da, const double *b, int db, double *o);
int pintscale(double *a, int d);
const char *pfactor(const double *a, int d, const char *var);
void in_line3(const char *name, double *a, double *d);
void in_plane3(const char *name, double *n, double *d);
const char *fplane(const double *n, double d);
void out_line3(const char *name, const double *a, const double *d);
#endif
