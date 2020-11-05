/*
 */

#include <iostream>

#include <vector>
#include <algorithm>
#include <cmath>

#include "pad.hpp"

struct _Iter_more_iter
{
    template<class _Iterator1, class _Iterator2>
        bool
        operator()(_Iterator1 __it1, _Iterator2 __it2) const
      { return __it1 > __it2; }
};


template<class T>
std::vector<T> pad(std::vector<T> Di, size_t k, T p)
{
    const T P_INFINITY = std::numeric_limits<T>::max(); // positive infinity
    const T N_INFINITY = std::numeric_limits<T>::min(); // negative infinity

    size_t size_in = k < Di.size() ? k : Di.size();
    size_t size_out = pow(2, ceil(log2(k)));

    std::vector<T> Do(k);
    
    // 1. Sort D_p and retain only the k smallest values
    // __gnu_cxx::__ops::_Iter_less_iter __comp;
    _Iter_more_iter __comp;

    std::make_heap(Di.begin(), Di.end(), __comp);
    for(size_t i = 0; i < size_in; i ++) 
    {
        Do[i] = Di.front();
        std::pop_heap(Di.begin(), Di.end()-i, __comp);
    }
    
    
    // 2. Pad D_p with +\infinity until |D_p|=k

    for(size_t i = size_in; i < k; i ++)
    {
        // ? how to pad with +\infinity
        Do[i] = P_INFINITY;
    }

    // 3. Pad D_p with \hat{p} until |D_p|=2^{\log{2}{(k)}}

    // std::vector<_Tp, _Alloc>::iterator insert(std::vector<_Tp, _Alloc>::const_iterator __position, std::size_t __n, const std::vector<_Tp, _Alloc>::value_type &__x)
    if(p < 0) // p is -\infinity
    {
        // pre-pad with -\infinity
        Do.insert(Do.begin(), size_out-k, N_INFINITY);
    }
    else // p is +\infinity
    {
        // pad with +\infinity
        Do.insert(Do.end(),   size_out-k, P_INFINITY);
    }

    // 4. return D_p
    return Do;
}


template<class T>
void test(T arr[], size_t n)
{
    std::vector<T> Di(arr, arr+n);
    std::vector<T> Do = pad(Di, 3, std::numeric_limits<T>::max());

    for(auto it = Do.begin(); it < Do.end(); it ++)
        std::cout << *it << std::endl;
}

int main()
{
    //int a[] = {1, 2, 3, 4, 5};
    //long long a[] = {1, 2, 3, 4, 5};
    double a[] = {1.1, 2.2, 3.3, 4.4, 5.5};

    test(a, 5);

    return 0;
}
