#pragma once

#include <cstddef>
#include <vector>
#include <iostream>

template<class T>
class BitonicMerge
{
private:
    void Merge(size_t l, size_t r, std::vector<T> &D);

public:
    void operator()(size_t l, size_t r, std::vector<T> &D);
    
};

template<class T>
void BitonicMerge<T>::Merge(size_t l, size_t r, std::vector<T> &D)
{
    if(r <= l) return ;

    size_t m = (r+l) / 2 + 1;

    for(size_t i = l; i < m; i ++) {
        size_t e = (i-l) + m;
        if(D[i] > D[e]) {
            std::swap(D[i], D[e]);
        }
    }

    this->Merge(l, m-1, D);
    this->Merge(m, r, D);
}

template<class T>
void BitonicMerge<T>::operator()(size_t l, size_t r, std::vector<T> &D)
{
    this->Merge(l, r, D);
}

