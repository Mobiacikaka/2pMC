#pragma once

#include <iostream>
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

    friend void TestFunctionA();
    friend void TestFunctionB();

private:
    void GenerateRandomDataSet(size_t n);
    typename std::vector<T>::iterator GetMedianIterator();
    void SortDataSet();

    std::vector<T> data_set;

    bool sorted;

protected:
    // This pad version is not optimized
    std::vector<T>* Pad(size_t k, T p);

    // get the median element of the data set
    T GetMedian() const;

    // retain only the upper half
    void KeepUpperHalf();
    
    // retain only the lower half
    void KeepLowerHalf();

    //! Following function only for test
    void PrintAllElement();

    size_t GetSizeofDataSet() const;

public:
    DataSet();
    ~DataSet();

    // Use Random Function to generate random int list
    void Init();
};

template<class T>
DataSet<T>::DataSet()
{

}

template<class T>
DataSet<T>::~DataSet()
{

}

template<class T>
void DataSet<T>::PrintAllElement()
{
    for(auto it = this->data_set.begin(); it < this->data_set.end(); it ++)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

template<class T>
void DataSet<T>::GenerateRandomDataSet(size_t n)
{
    this->data_set.resize(n);
    for(size_t i = 0; i < n; i ++)
    {
        int tmp = std::rand();
        this->data_set[i] = tmp;
        std::srand(time(NULL));
        std::srand(tmp);
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
    std::vector<T>& datasetoutput(*datasetoutput_);
    
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
    size_t n = std::rand() % 101;
    std::srand(n);
    if(n < 50) n += 50;
    std::cout << "Generate length of " << n << " list." << std::endl;
    this->GenerateRandomDataSet(n);
    this->SortDataSet();
}

template<class T>
typename std::vector<T>::iterator DataSet<T>::GetMedianIterator()
{
    size_t size = this->data_set.size();
    size_t middlepostion = size / 2;
    return this->data_set.begin() + middlepostion;
}

template<class T>
T DataSet<T>::GetMedian() const
{
    assert(sorted == true);
    typename std::vector<T>::iterator middleiterator = this->GetMedianIterator();
    return *middleiterator;
}

template<class T>
size_t DataSet<T>::GetSizeofDataSet() const
{
    return this->data_set.size();
}

template<class T>
void DataSet<T>::KeepUpperHalf()
{
    typename std::vector<T>::iterator middleiterator = this->GetMedianIterator();
    this->data_set.erase(middleiterator, this->data_set.end());
}

template<class T>
void DataSet<T>::KeepLowerHalf()
{
    typename std::vector<T>::iterator middleiterator = this->GetMedianIterator();
    this->data_set.erase(this->data_set.begin(), middleiterator);
}

template<class T>
void DataSet<T>::SortDataSet()
{
    std::sort(this->data_set.begin(), this->data_set.end());
    this->sorted = true;
}
