
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void fill_array(float *arr, size_t size)
{

    for (int i = 0; i < size; i++)
    {
        arr[i] = 1.5;
    }
}

int main(int argc, char **argv)
{

    int n = atoi(argv[1]);
    clock_t start, end;
    float array1[n];
    float array2[n];
    fill_array(array1, n);
    fill_array(array2, n);

    int result = 0;
    start = clock();
    for (int i = 0; i < n; i++)
    {
        result += array1[i] * array2[i];
    }
    end = clock();
    printf("Result: %d, time elapsed: %3.15f\n", result, (float)(end - start) / CLOCKS_PER_SEC);
}