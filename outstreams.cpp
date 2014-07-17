#include "outstreams.h"

outstreams::outstreams() {
#ifdef PW_MPI
    std::stringstream debug_fname;
    debug_fname << "papi_debug";
    int mpi_rank = -1;
    int mpi_size = -1;

    // assume that MPI has already been initialized...
    MPI_Comm_size(MPI_COMM_WORLD,&mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD,&mpi_rank);

    // write to file
    debug_fname << "_" << mpi_rank << "_" << mpi_size;
    debug_fname << ".txt";
    fid_.open(debug_fname.str().c_str());
    #endif
}

