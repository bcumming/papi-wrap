#include <iostream>
#include <vector>

extern "C" int test(int N){
    int sum=0;
    std::vector<int> vec(N);
    for( int i=0; i<N; i++ )
        vec[i]=i;
    for( int i=0; i<N; i++ )
        sum += vec[i];
    std::cout << "internal : sum is " << sum << std::endl;
    return sum;
}

