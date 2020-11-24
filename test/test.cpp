#include "../src/dataset.hpp"
#include <iostream>

void TFA()
{
    DataSet dataset;

    dataset.Init();
    dataset.PrintAllElement();
    std::cout << dataset.GetMedian() << std::endl;
    dataset.KeepLowerHalf();
    dataset.PrintAllElement();
    std::cout << dataset.GetMedian() << std::endl;
    dataset.KeepUpperHalf();
    dataset.PrintAllElement();
    std::cout << dataset.GetMedian() << std::endl;
    
}

int main()
{
    TFA();
    return 0;
}
