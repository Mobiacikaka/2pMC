#pragma once

#include <algorithm>
#include <cmath>
#include <vector>
#include <cassert>
#include <random>
#include <exception>

template<class T>
class Party;

template<class T>
class DataSet
{
    friend Party<T>;

private:
    std::vector<T> data_set;
    void GenerateRandomDataSet(size_t n);

protected:
    // This pad version is not optimized
    std::vector<T>* Pad(size_t k, T p);

    // get the median element of the data set
    T GetMedian() const;

    // retain only the upper half
    void KeepUpperHalf();
    
    // retain only the lower half
    void KeepLowerHalf();

public:
    DataSet();
    ~DataSet();

    // Use Random Function to generate random int list
    void Init();

};

template<class T>
void DataSet<T>::GenerateRandomDataSet(size_t n)
{
    this->data_set.reserve(n);
    for(size_t i = 0; i < n; i ++)
    {
        std::srand(time(0));
        this->data_set[i] = std::rand();
    }
}

template<class T>
std::vector<T>* DataSet<T>::Pad(size_t k, T p)
{
    const T P_INFINITY = std::numeric_limits<T>::max(); // positive infinity
    const T N_INFINITY = std::numeric_limits<T>::min(); // negative infinity

    std::vector<T> &datasetinput(this->data_set);

    size_t size_in = k < datasetinput.size() ? k : datasetinput.size();
    size_t size_out = pow(2, ceil(log2(k)));

    std::vector<T>* datasetoutput_ = new std::vector<T>(k);
    std::vector<T>& datasetoutput(datasetoutput_);
    
    // 1. Sort D_p and retain only the k smallest values
    /**
     * \warning the follow operation will disturb the original order
     */
    std::make_heap(datasetinput.begin(), datasetinput.end(), std::less<T>());
    for(size_t i = 0; i < size_in; i ++) 
    {
        datasetoutput[i] = datasetinput.front();
        std::pop_heap(datasetinput.begin(), datasetinput.end()-i, std::less<T>());
    }
    
    // 2. Pad D_p with +\infinity until |D_p|=k
    for(size_t i = size_in; i < k; i ++)
    {
        datasetoutput[i] = P_INFINITY;
    }

    // 3. Pad D_p with \hat{p} until |D_p|=2^{\log{2}{(k)}}
    if(p < 0) // p is -\infinity
    {
        datasetoutput.insert(datasetoutput.begin(), size_out-k, N_INFINITY);
    }
    else // p is +\infinity
    {
        datasetoutput.insert(datasetoutput.end(),   size_out-k, P_INFINITY);
    }

    // 4. return D_p
    return datasetoutput_;
}

template<class T>
void DataSet<T>::Init()
{
    std::srand(time(0));
    size_t n = std::rand() % 101;
    this->GenerateRandomDataSet(n);
}

template<class T>
T DataSet<T>::GetMedian() const
{
    typename std::vector<T>::iterator begin = this->data_set.cbegin();
    typename std::vector<T>::iterator end   = this->data_set.cend();
    typename std::vector<T>::iterator median= (begin + end) / 2;
    return   *median;
}

template<class T>
void DataSet<T>::KeepUpperHalf()
{
    typename std::vector<T>::iterator first = this->data_set.begin();
	typename std::vector<T>::iterator last  = this->data_set.end();
	typename std::vector<T>::iterator median= (first + last) / 2;
    this->data_set.erase(median, last);
}

template<class T>
void DataSet<T>::KeepLowerHalf()
{
    typename std::vector<T>::iterator first = this->data_set.begin();
	typename std::vector<T>::iterator last  = this->data_set.end();
	typename std::vector<T>::iterator median= (first + last) / 2;
    this->data_set.erase(first, median);
}


