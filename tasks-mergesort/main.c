#include <stdio.h>
#include <time.h>
#include <stdlib.h>

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

int main(int argc, char* argv[]) {
    int size = atoi(argv[1]);
    int *a = malloc (sizeof (int) * size);
    int *temp = malloc (sizeof (int) * size);

    srand (314159);

    for (int i = 0; i < size; i++) {
        a[i] = rand () % size;
    }
    double start = get_time();
    mergesort_serial(a, size, temp);
    double finish = get_time();

    printf("Ready, time: %.6f \n", (float)(finish - start));

    return 0;
}