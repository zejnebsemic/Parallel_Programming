//
// Created by Adnan Hajro on 29. 10. 2025..
//

#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "sum_algorithms.h"

#define ORDERS_OF_MAGNITUDE 1.0e9

void cpu_timer_start(struct timespec *tstart_cpu) {
    clock_gettime(CLOCK_MONOTONIC, tstart_cpu);
}

double cpu_timer_stop(struct timespec tstart_cpu) {
    struct timespec tstop_cpu, tresult;
    clock_gettime(CLOCK_MONOTONIC, &tstop_cpu);

    tresult.tv_sec = tstop_cpu.tv_sec - tstart_cpu.tv_sec;
    tresult.tv_nsec = tstop_cpu.tv_nsec - tstart_cpu.tv_nsec;

    return (double)tresult.tv_sec + (double)tresult.tv_nsec * 1.0e-9;
}

int main(int argc, char *argv[])
{
    printf("========================================\n");
    printf(" GLOBAL SUM PRECISION TEST (Adnan Hajro assignment)\n");
    printf("========================================\n\n");

    
    for (int pow_of_two = 10; pow_of_two <= 27; pow_of_two++) {
        long ncells = (long)pow(2.0, (double)pow_of_two);
        long ncellsdiv2 = ncells / 2;

        printf("========================================\n");
        printf("TESTING WITH 2^%d = %ld elements\n", pow_of_two, ncells);
        printf("========================================\n");

        double high_value = 1.0e-1;
        double low_value  = 1.0e-1 / ORDERS_OF_MAGNITUDE;

        double accurate_sum = (double)ncellsdiv2 * high_value +
                              (double)ncellsdiv2 * low_value;

        double *energy = (double *)malloc(ncells * sizeof(double));
        if (energy == NULL) {
            fprintf(stderr, "Error: Failed to allocate memory for %ld elements\n", ncells);
            return 1;
        }

        
        printf("Initializing array (Leblanc problem: high values first, then low values)\n");
        for (long i = 0; i < ncells; i++) {
            energy[i] = (i < ncellsdiv2) ? high_value : low_value;
        }

        printf("Expected accurate sum: %-17.16lg\n\n", accurate_sum);

        struct timespec cpu_timer;
        double test_sum, cpu_time;

        
        cpu_timer_start(&cpu_timer);
        test_sum = do_sum(energy, ncells);
        cpu_time = cpu_timer_stop(cpu_timer);
        printf("  accurate sum %-17.16lg sum %-17.16lg diff %10.4lg rel diff %10.4lg time %lf",
               accurate_sum, test_sum, test_sum - accurate_sum,
               (test_sum - accurate_sum) / accurate_sum, cpu_time);
        printf("   Standard double sum\n");

        
        cpu_timer_start(&cpu_timer);
        long double test_sum_ld = do_long_double_sum(energy, ncells);
        cpu_time = cpu_timer_stop(cpu_timer);
        printf("  accurate sum %-17.16lg sum %-17.16Lg diff %10.4Lg rel diff %10.4Lg time %lf",
               accurate_sum, test_sum_ld, test_sum_ld - accurate_sum,
               (test_sum_ld - accurate_sum) / accurate_sum, cpu_time);
        printf("   Long double sum\n");

      
        cpu_timer_start(&cpu_timer);
        test_sum = do_pair_sum(energy, ncells);
        cpu_time = cpu_timer_stop(cpu_timer);
        printf("  accurate sum %-17.16lg sum %-17.16lg diff %10.4lg rel diff %10.4lg time %lf",
               accurate_sum, test_sum, test_sum - accurate_sum,
               (test_sum - accurate_sum) / accurate_sum, cpu_time);
        printf("   Pairwise sum\n");

      
        cpu_timer_start(&cpu_timer);
        test_sum = do_kahan_sum(energy, ncells);
        cpu_time = cpu_timer_stop(cpu_timer);
        printf("  accurate sum %-17.16lg sum %-17.16lg diff %10.4lg rel diff %10.4lg time %lf",
               accurate_sum, test_sum, test_sum - accurate_sum,
               (test_sum - accurate_sum) / accurate_sum, cpu_time);
        printf("   Kahan sum\n");

        
        cpu_timer_start(&cpu_timer);
        test_sum = do_knuth_sum(energy, ncells);
        cpu_time = cpu_timer_stop(cpu_timer);
        printf("  accurate sum %-17.16lg sum %-17.16lg diff %10.4lg rel diff %10.4lg time %lf",
               accurate_sum, test_sum, test_sum - accurate_sum,
               (test_sum - accurate_sum) / accurate_sum, cpu_time);
        printf("   Knuth sum\n");

        free(energy);
        printf("\n");
    }

    printf("========================================\n");
    printf("All tests complete!\n");
    printf("========================================\n");

    return 0;
}
