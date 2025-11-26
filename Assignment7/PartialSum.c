#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include "timer.h"

int main(int argc, char *argv[])
{
    int rank, nprocs;
    int ncells = 10000;
    struct timespec tstart_time;

    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &nprocs);

    int base = ncells / nprocs;
    int extra = ncells % nprocs;

    int nsize = (rank < extra) ? base + 1 : base;

    double *a_global = NULL;
    if (rank == 0) {
        a_global = (double *)malloc(ncells * sizeof(double));
        printf("Main process initializing array of %d elements\n", ncells);
        for (int i = 0; i < ncells; i++) {
            a_global[i] = (double)(i + 1);
        }
        printf("Array initialization complete\n\n");
    }

    int nsizes[nprocs];
    int offsets[nprocs];

    
    MPI_Allgather(&nsize, 1, MPI_INT, nsizes, 1, MPI_INT, comm);

    offsets[0] = 0;
    for (int i = 1; i < nprocs; i++) {
        offsets[i] = offsets[i - 1] + nsizes[i - 1];
    }

    
    double *a_local = (double *)malloc(nsize * sizeof(double));

    cpu_timer_start(&tstart_time);


    MPI_Scatterv(
        a_global, nsizes, offsets, MPI_DOUBLE,
        a_local, nsize, MPI_DOUBLE,
        0, comm
    );

    double scatter_time = cpu_timer_stop(tstart_time);

    cpu_timer_start(&tstart_time);


    double local_sum = 0.0;
    for (int i = 0; i < nsize; i++) {
        local_sum += a_local[i];
    }

    double compute_time = cpu_timer_stop(tstart_time);

    printf("Rank %d: processed %d elements, partial sum = %.2f, compute time = %lf sec\n",
           rank, nsize, local_sum, compute_time);

    double total_sum = 0.0;

    cpu_timer_start(&tstart_time);

    MPI_Reduce(&local_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, comm);

    double reduce_time = cpu_timer_stop(tstart_time);

    if (rank == 0) {
        printf("\n========================================\n");
        printf("Timing Results:\n");
        printf("  Scatter operation: %lf seconds\n", scatter_time);
        printf("  Reduce operation:  %lf seconds\n", reduce_time);
        printf("\nFinal Results:\n");
        printf("  Total sum across all processes: %.2f\n", total_sum);
        printf("========================================\n");

        free(a_global);
    }

    free(a_local);

    MPI_Finalize();
    return 0;
}
