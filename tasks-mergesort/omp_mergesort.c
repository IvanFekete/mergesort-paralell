//gcc-9 -fopenmp omp_mergesort.c -o omp_mergesort
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include "get_time.c"

extern double get_time(void);

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

int main(int argc, char *argv[]) {
    int size = atoi(argv[1]);
    int threads = atoi(argv[2]);

    omp_set_nested(1);
    omp_set_num_threads(threads);

    int *a = malloc(sizeof(int) * size);
    int *temp = malloc(sizeof(int) * size);

    int i;
    srand(314159);
    for (i = 0; i < size; i++) {
        a[i] = rand() % size;
    }


    int part_size = size / threads;
    int first_part = size % part_size + part_size;
    double start = get_time();
    {
#pragma omp parallel for shared(a, temp)
        for (int i = 0; i < threads; i++) {
            if (i == 0) {
                mergesort_serial(a, first_part, temp);
            } else {
                mergesort_serial(a + first_part + (i - 1) * part_size, part_size, temp + + first_part + (i - 1) * part_size);
            }
        }

        for (int i = 1; i < threads; i++) {
            merge(a, first_part + i * part_size, first_part + (i - 1) * part_size, temp);
        }
    }
    double end = get_time();
    printf("%.4f\n", end - start);


    for (i = 1; i < size; i++) {
        if (!(a[i - 1] <= a[i])) {
            printf("Error: final array not sorted => a[%d]=%d > a[%d]=%d\n", i - 1,
                   a[i - 1], i, a[i]);
            return 1;
        }
    }
    return 0;
}


