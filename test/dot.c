#include <stdlib.h>
#include <stdio.h>

#include "../papi_wrap.h"

int main(void){
    int handle1=-1;
    int handle2=-1;
    int i;
    int N=10000000;
    double *a = (double*)malloc(N*sizeof(double));
    double *b = (double*)malloc(N*sizeof(double));
    double sum=0.;

    handle1 =  pw_new_collector("counter 1");
    handle2 =  pw_new_collector("counter 2");

    printf( "two collectors with handles %d and %d\n", handle1, handle2);

    // the first counter counts the initialization loop
    pw_start_collector(handle1);
    for(i=0; i<N; i++) {
        a[i] = (double)i;

        b[i] = 0.1*(double)i;
    }
    pw_stop_collector(handle1);

    // the second counter counts the dot product loop
    pw_start_collector(handle2);
    sum = 0.;
    for(i=0; i<N; i++) {
        sum += a[i]*b[i];
    }
    pw_stop_collector(handle2);

    pw_print();
}
