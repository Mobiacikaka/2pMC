
#include <vector>
#include <algorithm>

#include "pad.hpp"

template<class T>
T select_median(const std::vector<T> D)
{
    std::vector<T>::iterator first = D.begin();
    std::vector<T>::iterator end   = D.end()  ;
    std::vector<T>::iterator mid   = (first + end) / 2;

    return std::nth_element(first, mid, end);
}


template<class T>
  std::pair<std::vector<T>, std::vector<T>> 
    prune(const std::vector<T> D_A, const std::vector<T> D_B, size_t s, size_t k)
{
    std::vector<T> iD_A = pad(D_A, k, std::numeric_limits<T>::max());
    std::vector<T> iD_B = pad(D_B, k, std::numeric_limits<T>::min());

    for(size_t i = 0; i < s; i ++) 
    {
        // select median from DA
        T mA = select_median(iD_A);
        
        // select median from DB
        T mB = select_median(iD_B);

        // c <- mA < mB

        // A cut half

        // B cut half
    }
}
