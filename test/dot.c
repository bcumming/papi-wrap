#include <stdlib.h>
#include <stdio.h>

#ifdef PW_MPI
#include <mpi.h>
#endif

#include "../papi_wrap.h"

int main(int argc, char** argv){
    int handle1=-1;
    int handle2=-1;
    int i;
    int N=1000000;
    double *a = (double*)malloc(N*sizeof(double));
    double *b = (double*)malloc(N*sizeof(double));
    double sum=0.;

    #ifdef PW_MPI
    int size, rank;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if(rank==0)
        printf("MPI version: %d processes\n", size);
    MPI_Barrier(MPI_COMM_WORLD);
    #endif


    handle1 =  pw_new_collector("initialize");
    handle2 =  pw_new_collector("dot product");

    printf( "two collectors with handles %d and %d\n", handle1, handle2);

    // the first counter counts the initialization loop
    pw_start_collector(handle1);
    #pragma omp parallel for
    for(i=0; i<N; i++) {
        a[i] = 0.001*(double)i;
        b[i] = 0.001*(double)i;
    }
    pw_stop_collector(handle1);

    // the second counter counts the dot product loop
    pw_start_collector(handle2);
    sum = 0.;
    #pragma omp parallel for reduction(+:sum)
    for(i=0; i<N; i++) {
        sum += a[i]*b[i];
    }
    pw_stop_collector(handle2);

    // print the result so that the compiler doesn't remove it when optimizing
    printf("the sum is %g\n", sum);

    #ifdef PW_MPI
    MPI_Finalize();
    #endif

    pw_print();
}
