#include <vector>
#include <string>
#include <iostream>

#ifdef PW_MPI
#include <mpi.h>
#endif

#include "PapiCollectors.h"
#include "papi_wrap.h"

const int N=2*1024;
const int numOuter=10;

int main(int argc, char **argv)
{
    // set up MPI
    #ifdef PW_MPI
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(rank==0)
        std::cout << "hello world from rank " << rank << " of " << size << std::endl;
    MPI_Barrier(MPI_COMM_WORLD);
    #endif

    #ifdef _OPENMP
    std::cout << "Using OpenMP :: " << omp_get_max_threads() << " threads" << std::endl;
    #endif

    // initialise data
    std::vector<double> A(N*N);
    std::vector<double> x(N);
    std::vector<double> y(N);
    #pragma omp parallel for
    for(int i=0; i<N*N; i++)
        A[i] = 1.;
    #pragma omp parallel for
    for(int i=0; i<N; i++)
        x[i] = 1.;

    // create Papi counters
    //int handle_ij = PapiCollectors::instance()->addCollector("IJLoop");
    //int handle_ji = PapiCollectors::instance()->addCollector("JILoop");
    int handle_ij = pw_new_collector("IJLoop");
    int handle_ji = pw_new_collector("JILoop");

    const long long numFlops = N*N*2;
    std::cout << "FLOPs per iteration :: " << numFlops << std::endl;
    std::cout << "FLOPs total         :: " << numOuter*numFlops << std::endl;

    /*************************************
    *************************************/
    double *yp = &y[0];
    double *xp = &x[0];
    double *Ap = &A[0];
    for(int outer=0; outer<numOuter; outer++){
        // matrix-vector multiply IJ order
        // matix traversal is stride 1
        //PapiCollectors::instance()->start(handle_ij);
        pw_start_collector(handle_ij);
        #pragma omp parallel for
        for(int i=0; i<N; i++){
            //#pragma ivdep
            for(int j=0; j<N; j++)
                yp[i] += Ap[i*N + j]*xp[j];
        }
        //PapiCollectors::instance()->stop(handle_ij);
        pw_stop_collector(handle_ij);

        // matrix-vector multiply JI order
        // matix traversal is stride N
        //PapiCollectors::instance()->start(handle_ji);
        pw_start_collector(handle_ji);
        #pragma omp parallel for
        for(int j=0; j<N; j++){
            for(int i=0; i<N; i++)
                yp[i] += Ap[i*N + j]*xp[j];
        }
        //PapiCollectors::instance()->stop(handle_ji);
        pw_stop_collector(handle_ji);

    }
    // print the counters to std::out
    //PapiCollectors::instance()->print();

    // write the counters to file, in plain text format
    //papiCounters.writeToFile(std::string("counters.txt"), fileFormatPlain);

    pw_print();
#ifdef PW_MPI
    MPI_Finalize();
#endif
}
