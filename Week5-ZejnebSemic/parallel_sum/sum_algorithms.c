#include <math.h>
#include <stdlib.h>
#include "sum_algorithms.h"


double do_sum(double* restrict var, long ncells)
{
    double sum = 0.0;
    for (long i = 0; i < ncells; i++){
        sum += var[i];
    }
    return sum;
}


long double do_long_double_sum(double* restrict var, long ncells)
{
    long double sum = 0.0;
    for (long i = 0; i < ncells; i++){
        sum += (long double)var[i];
    }
    return sum;
}


double do_pair_sum(double* restrict var, long ncells)
{
    if (ncells == 1) return var[0];
    long nmax = ncells / 2;
    double *pwsum = (double *)malloc(nmax * sizeof(double));

    for (long i = 0; i < nmax; i++){
        pwsum[i] = var[i*2] + var[i*2 + 1];
    }

    double result = do_pair_sum(pwsum, nmax);
    free(pwsum);
    return result;
}


double do_kahan_sum(double* restrict var, long ncells)
{
    double sum = 0.0;
    double c = 0.0;
    for (long i = 0; i < ncells; i++){
        double y = var[i] - c;
        double t = sum + y;
        c = (t - sum) - y;
        sum = t;
    }
    return sum;
}


double do_knuth_sum(double* restrict var, long ncells)
{
    double sum = 0.0;
    double err = 0.0;
    for (long i = 0; i < ncells; i++) {
        double temp = sum + var[i];
        if (fabs(sum) >= fabs(var[i]))
            err += (sum - temp) + var[i];
        else
            err += (var[i] - temp) + sum;
        sum = temp;
    }
    return sum + err;
}
