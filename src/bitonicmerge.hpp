#ifndef __BITONICMERGE_HPP__
#define __BITONICMERGE_HPP__

#include <cstddef>
#include <vector>
#include <iostream>


class BitonicMerge
{
private:
    void Merge(size_t l, size_t r, std::vector<int32_t> &D);

public:
    void operator()(size_t l, size_t r, std::vector<int32_t> &D);
    
};


void BitonicMerge::Merge(size_t l, size_t r, std::vector<int32_t> &D)
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


void BitonicMerge::operator()(size_t l, size_t r, std::vector<int32_t> &D)
{
    this->Merge(l, r, D);
}

#endif