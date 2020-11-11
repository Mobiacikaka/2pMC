#include <random>
#include <vector>
#include <algorithm>
#include <iostream>

#include "bitonicmerge.hpp"

void test_bitonicmerge() {
    int a[] = {3,5,8,9,10,12,14,20,
                95,90,60,40,35,23,18,0};

    std::vector<int> D(a, a+16);
    
    BitonicMerge<int> bitonicmerge;
    bitonicmerge(0, 15, D);

    for(size_t i = 0; i < D.size(); i ++) 
        std::cout << D[i] << " ";
    std::cout << std::endl;
}


int main() {
    test_bitonicmerge();
    std::greater<int>();
    std::less<int>();
    return 0;
}