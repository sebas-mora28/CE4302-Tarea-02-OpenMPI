#include <mpi.h>
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
    MPI_Init(&argc, &argv);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);


    clock_t start, end; 
    
    // Central node
    if (world_rank == 0)
    {
        float array1[n];
        float array2[n];
        fill_array(array1, n);
        fill_array(array2, n);

        int index;
        int chunk_size = n / world_size;
        int i;
        start = clock();
        /**
        * Comienza a distribuir las cargas entre los nodos
        */
        for (i = 1; i < world_size - 1; i++)
        {
            index = i * chunk_size;
            MPI_Send(&chunk_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&array1[index], chunk_size, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&array2[index], chunk_size, MPI_FLOAT, i, 0, MPI_COMM_WORLD);

        }


        /*
        * Se calcula los elementos restantes por distribuir y se enviar al 
        * ultimo nodo 
        */
        index = i * chunk_size;
        int element_left = n - index;
        MPI_Send(&element_left, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(&array1[index], element_left, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
        MPI_Send(&array2[index], element_left, MPI_FLOAT, i, 0, MPI_COMM_WORLD);


        printf("Central node is processing %d elements", chunk_size)
        // Nodo central comienza a realizar su parte del procesamiento
        float res = 0;
        for (int i = 0; i < chunk_size; i++)
        {
            res += array1[i] * array2[i];
        }


        // Se recibe los resultados parciales de los nodos y se suman al resultado final
        float partial_res;
        for (int i = 1; i < world_size; i++)
        {
            MPI_Recv(&partial_res, 1, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Receiving partial result from node %d: %3.15f\n", i, partial_res);
            res += partial_res;
        }
        MPI_Barrier(MPI_COMM_WORLD);
        
        end = clock();
        float time = (float)(end - start) / CLOCKS_PER_SEC;
        printf("result : %3.15f, time elapsed %3.15f\n", res, time);
    }
    else
    {

        /*
        * El nodo recibe los trozos de array que le corresponden procesar
        */
        int chunk_size;
        MPI_Recv(&chunk_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        float chunk_array1[chunk_size];
        float chunk_array2[chunk_size];
        MPI_Recv(&chunk_array1, chunk_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&chunk_array2, chunk_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("node %d is processing %d elements\n", world_rank, chunk_size);

        /*
        * Lleva a cabo la operacion producto punto al trozo del array  
        */
        float partial_res = 0;
        for (int i = 0; i < chunk_size; i++)
        {
            partial_res += chunk_array1[i] * chunk_array2[i];
        }

        // Envia el resultado parcial al nodo central
        MPI_Send(&partial_res, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}