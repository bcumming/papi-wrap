/***************************************************************************************
 *  test creation of a papi handle as a static variable in a function
 ***************************************************************************************/
#include <stdlib.h>
#include <stdio.h>

#include "../papi_wrap.h"

void foo() {
    static int papi_handle = -1;
    static double *a = NULL;
    static double *b = NULL;

    int i;
    int N=1000000;
    double sum=0.;

    if(papi_handle<0) {
        printf("calling foo() for the first time :: ");

        // initialise data fields
        a = (double*)malloc(N*sizeof(double));
        b = (double*)malloc(N*sizeof(double));
        for(i=0; i<N; i++) {
            a[i] = 0.001*(double)(i+1);
            b[i] = 10.*a[i];
        }

        // set the papi handle
        papi_handle =  pw_new_collector("foo");

        // check for errors
        if(papi_handle<0) {
            fprintf(stderr, "error: negative papi handle returned by pw_new_collector\n");
            exit(1);
        }
    }
    else {
        printf("calling foo() again  :: ");
    }

    pw_start_collector(papi_handle);
    // perform dot product
    for(i=0; i<N; i++) {
        sum += a[i]/b[i];
    }
    pw_stop_collector(papi_handle);

    printf("%g\n", sum);
}

int main(void){
    int i;

    for(i=0; i<10; ++i) foo();

    pw_print();

    return 0;
}

