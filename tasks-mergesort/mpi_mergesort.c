// mpicc mpi_mergesort.c -lm -o mpi_mergesort

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
#include "get_time.c"


extern double get_time (void);

void merge(int a[], int size, int size1, int temp[]) {
    int i1 = 0;
    int i2 = size1;
    int tempi = 0;
    while (i1 < size1 && i2 < size) {
        if (a[i1] < a[i2]) {
            temp[tempi] = a[i1];
            i1++;
        } else {
            temp[tempi] = a[i2];
            i2++;
        }
        tempi++;
    }
    while (i1 < size1) {
        temp[tempi] = a[i1];
        i1++;
        tempi++;
    }
    while (i2 < size) {
        temp[tempi] = a[i2];
        i2++;
        tempi++;
    }
    memcpy(a, temp, size * sizeof(int));
}


void mergesort_serial(int a[], int size, int temp[]) {
    if (size > 1) {
        mergesort_serial(a, size / 2, temp);
        mergesort_serial(a + size / 2, size - size / 2, temp);
        merge(a, size, size / 2, temp);
    }
}

int main (int argc, char *argv[]) {
    MPI_Init (&argc, &argv);
    int comm_size, my_rank;
    MPI_Comm_size (MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    int tag = 123;
    // Set test data

    int size = atoi (argv[1]);
    int part_size = size / comm_size;
    int first_part = size % part_size + part_size;
    if (my_rank == 0) {
        int *a = malloc (sizeof (int) * size);
        int *temp = malloc (sizeof (int) * size);

        srand (314159);

        for (int i = 0; i < size; i++) {
            a[i] = rand () % size;
        }
        // Sort with root process
        double start = get_time ();

        {
            for (int i = 0; i < comm_size; i++) {
                if (i == 0) {
                    mergesort_serial(a, first_part, temp);
                } else {
                    MPI_Send(a + first_part + (i - 1) * part_size, part_size, MPI_INT, i, tag, MPI_COMM_WORLD);
                }
            }

            MPI_Status status;
            for (int i = 1; i < comm_size; i++) {
                MPI_Recv(a + first_part + (i - 1) * part_size, part_size, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
            }

            for (int i = 1; i < comm_size; i++) {
                merge(a, first_part + i * part_size, first_part + (i - 1) * part_size, temp);
            }
        }

        double end = get_time ();
        printf ("%.2f\n", end - start);

        // Result check
        for (int i = 1; i < size; i++) {
            if (a[i - 1] > a[i]) {
                printf ("Implementation error: a[%d]=%d > a[%d]=%d\n", i - 1,
                        a[i - 1], i, a[i]);
                MPI_Abort (MPI_COMM_WORLD, 1);
            }
        }
    }
    else {
        int *a = malloc (sizeof (int) * part_size);
        int *temp = malloc (sizeof (int) * part_size);
        MPI_Status status;
        MPI_Recv (a, part_size, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
        mergesort_serial (a, part_size, temp);
        MPI_Send (a, part_size, MPI_INT, 0, tag, MPI_COMM_WORLD);
    }
    MPI_Finalize ();
    return 0;
}


