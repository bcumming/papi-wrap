/*
  STREAM benchmark implementation in CUDA.

    COPY:       a(i) = b(i)                 
    SCALE:      a(i) = q*b(i)               
    SUM:        a(i) = b(i) + c(i)          
    TRIAD:      a(i) = b(i) + q*c(i)        

  It measures the memory system on the device.
  The implementation is in single precision.

  Code based on the code developed by John D. McCalpin
  http://www.cs.virginia.edu/stream/FTP/Code/stream.c

  Written by: Massimiliano Fatica, NVIDIA Corporation

  Further modifications by: Ben Cumming, CSCS
*/

#define N   (2<<26)
#define NTIMES  2

#include <stdio.h>
#include <float.h>
#include <limits.h>
#include <sys/time.h>

#include "../papi_wrap.h"

# ifndef MIN
# define MIN(x,y) ((x)<(y)?(x):(y))
# endif
# ifndef MAX
# define MAX(x,y) ((x)>(y)?(x):(y))
# endif

typedef double real;

static double   avgtime[4] = {0}, maxtime[4] = {0},
        mintime[4] = {FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX};


static char *label[4] = {"Copy:      ", "Scale:     ", "Add:       ", "Triad:     "};

static double   bytes[4] = {
    2 * sizeof(real) * N,
    2 * sizeof(real) * N,
    3 * sizeof(real) * N,
    3 * sizeof(real) * N
};

/* A gettimeofday routine to give access to the wall
   clock timer on most UNIX-like systems.  */


double mysecond()
{
    struct timeval tp;
    struct timezone tzp;
    int i = gettimeofday(&tp,&tzp);
    return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}


template <typename T>
__global__ void set_array(T *a,  T value, int len)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx < len)
        a[idx] = value;
}

template <typename T>
__global__ void STREAM_Copy(T *a, T *b, int len)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx < len)
        b[idx] = a[idx];
}

template <typename T>
__global__ void STREAM_Scale(T *a, T *b, T scale,  int len)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx < len)
        b[idx] = scale* a[idx];
}

template <typename T>
__global__ void STREAM_Add( T *a, T *b, T *c,  int len)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx < len)
        c[idx] = a[idx]+b[idx];
}

template <typename T>
__global__ void STREAM_Triad( T *a, T *b, T *c, T scalar, int len)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx < len)
        c[idx] = a[idx]+scalar*b[idx];
}

int main()
{
    real *d_a, *d_b, *d_c;
    int j,k;
    double times[4][NTIMES];
    real scalar;

    int handle_add;

    printf(" STREAM Benchmark implementation in CUDA\n");
    printf(" Array size (%s precision) =%7.2f MB\n", sizeof(double)==sizeof(real)?"double":"single", double(N)*double(sizeof(real))/1.e6);

    /* Allocate memory on device */
    cudaMalloc((void**)&d_a, sizeof(real)*N);
    cudaMalloc((void**)&d_b, sizeof(real)*N);
    cudaMalloc((void**)&d_c, sizeof(real)*N);

    /* Compute execution configuration */
    dim3 dimBlock(192);
    dim3 dimGrid(N/dimBlock.x );
    if( N % dimBlock.x != 0 ) dimGrid.x+=1;

    printf(" using %d threads per block, %d blocks\n",dimBlock.x,dimGrid.x);

    /* Initialize memory on the device */
    set_array<real><<<dimGrid,dimBlock>>>(d_a, 2.f, N);
    set_array<real><<<dimGrid,dimBlock>>>(d_b, .5f, N);
    set_array<real><<<dimGrid,dimBlock>>>(d_c, .5f, N);


    handle_add =  pw_new_collector("dummy");
    pw_start_collector(handle_add);
    pw_stop_collector(handle_add);

    handle_add =  pw_new_collector("add");

    /*  --- MAIN LOOP --- repeat test cases NTIMES times --- */

    scalar=3.0f;
    for (k=0; k<NTIMES; k++)
    {
        times[0][k]= mysecond();
        STREAM_Copy<real><<<dimGrid,dimBlock>>>(d_a, d_c, N);
        cudaThreadSynchronize();
        times[0][k]= mysecond() -  times[0][k];

        times[1][k]= mysecond();
        STREAM_Scale<real><<<dimGrid,dimBlock>>>(d_b, d_c, scalar,  N);
        cudaThreadSynchronize();
        times[1][k]= mysecond() -  times[1][k];

        if(k==1) pw_start_collector(handle_add);

        times[2][k]= mysecond();
        STREAM_Add<real><<<dimGrid,dimBlock>>>(d_a, d_b, d_c,  N);
        cudaThreadSynchronize();
        times[2][k]= mysecond() -  times[2][k];

        if(k==1) pw_stop_collector(handle_add);

        times[3][k]= mysecond();
        STREAM_Triad<real><<<dimGrid,dimBlock>>>(d_b, d_c, d_a, scalar,  N);
        cudaThreadSynchronize();
        times[3][k]= mysecond() -  times[3][k];
    }

    pw_print();
    /*  --- SUMMARY --- */

    for (k=1; k<NTIMES; k++) /* note -- skip first iteration */
    {
        for (j=0; j<4; j++)
        {
            avgtime[j] = avgtime[j] + times[j][k];
            mintime[j] = MIN(mintime[j], times[j][k]);
            maxtime[j] = MAX(maxtime[j], times[j][k]);
        }
    }

    printf("Function      Rate (GB/s)   Avg time(s)  Min time(s)  Max time(s)\n");
    for (j=0; j<4; j++) {
        avgtime[j] = avgtime[j]/(double)(NTIMES-1);

        printf("%s%11.4f     %11.8f  %11.8f  %11.8f\n", label[j],
                1.0E-09 * bytes[j]/mintime[j],
                avgtime[j],
                mintime[j],
                maxtime[j]);
    }


    /* Free memory on device */
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);

}
