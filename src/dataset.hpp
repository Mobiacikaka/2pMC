#pragma once

#include <vector>

#include "config.h"

class DataSet
{
    friend class Party;
    friend void TFA();

private:
    void GenerateRandomDataSet(size_t n);

    void SortDataSet();

    std::vector<data_t> data_set;

    bool sorted;

protected:
    // This pad version is not optimized
    void Pad(size_t k, data_t p);

    // get the median element of the data set
    data_t GetMedian() const;

    // retain only the upper half
    void KeepUpperHalf();
    
    // retain only the lower half
    void KeepLowerHalf();

    // Get the size of data set
    size_t GetSizeofDataSet() const;

    //! Following function only for test
    void PrintAllElement();

public:
    DataSet();
    ~DataSet();

    // Use Random Function to generate random int list
    void Init();
};