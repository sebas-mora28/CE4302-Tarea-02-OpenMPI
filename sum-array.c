

#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


void fill_array(int* arr, size_t size){

    for(int i=0; i<size; i++){
        arr[i] = 1;
    }

}


int main(int argc, char** argv){

    int n = atoi(argv[1]);
    
    MPI_Init(&argc, &argv);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);


    
    // Central s
    if (world_rank == 0) 
    {
        int array[n];
        fill_array(array, n);

        int index;
        int chunk_size = n / world_size;
        int i;
        for(i=1; i< world_size - 1; i++){
            index = i * chunk_size;
            MPI_Send(&chunk_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&array[index], chunk_size, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        index = i * chunk_size;
        int element_left = n - index;
        MPI_Send(&element_left, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(&array[index], element_left, MPI_INT, i, 0, MPI_COMM_WORLD);


        int sum = 0; 
        for(int i=0; i<chunk_size; i++){
            sum+= array[i];
        }

        
        int partial_res;
        for(int i=1; i < world_size; i++){
            MPI_Recv(&partial_res, 1, MPI_INT, MPI_ANY_SOURCE , 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
            printf("Receiving partial result from node %d: %d\n", i, partial_res);
            sum += partial_res;
        }
        MPI_Barrier(MPI_COMM_WORLD);
        printf("sum of array is : %d\n", sum);
    }   
    else
    {



        int elements_to_process;
        MPI_Recv(&elements_to_process, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
        int chunk_array[elements_to_process];
        MPI_Recv(&chunk_array, elements_to_process, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
        printf("node %d processing %d elements\n", world_rank, elements_to_process);

        int partial_sum = 0;
        for(int i=0; i<elements_to_process; i++){
            partial_sum += chunk_array[i];
        }
        MPI_Send(&partial_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
    
    }

    MPI_Finalize();
    return 0;
}