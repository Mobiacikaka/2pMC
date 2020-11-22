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
    void Pad(size_t k, T p);

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
void DataSet<T>::Pad(size_t k, T p)
{
    std::cout << "Entering Pad" << std::endl;

    const T kP_INFINITY = std::numeric_limits<T>::max(); // positive infinity
    const T kN_INFINITY = std::numeric_limits<T>::min(); // negative infinity

	assert(p == kP_INFINITY || p == kN_INFINITY);

    // 1. Sort D_p and retain only the k smallest values
	if(this->sorted == false) 
	{
		this->SortDataSet();
		this->sorted = true;
	}

    // 2. Pad D_p with +\infinity until |D_p|=k
	if(k < this->data_set.size())
	{
		this->data_set.erase(this->data_set.begin()+k, this->data_set.end());
	}
	else
	{
		this->data_set.insert(this->data_set.end(), k-this->data_set.size(), kP_INFINITY);
	}

    // 3. Pad D_p with \hat{p} until |D_p|=2^{\log{2}{(k)}}
	k = pow( 2, std::ceil( std::log2( k ) ) );
    size_t padsize = k - this->data_set.size();
	if (p == kP_INFINITY)
	{
		for(size_t i = 0; i < padsize; i ++)
			this->data_set.insert(this->data_set.end(),   p);
	}
	else
	{
		for(size_t i = 0; i < padsize; i ++)
			this->data_set.insert(this->data_set.begin(), p);
	}

    // 4. return D_p
}

template<class T>
void DataSet<T>::Init()
{
    size_t n = std::rand() % 30;
    std::srand(n);
    // if(n < 50) n += 50;
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
