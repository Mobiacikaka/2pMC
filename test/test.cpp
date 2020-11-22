#include "dataset.hpp"

void TestFunctionA()
{
    DataSet<int> dataset;

    dataset.Init();
    dataset.PrintAllElement();
    dataset.KeepLowerHalf();
    dataset.PrintAllElement();
    dataset.KeepUpperHalf();
    dataset.PrintAllElement();
    
}

void TestFunctionB()
{
    DataSet<int> Da, Db;

    Da.Init();
    Db.Init();

    size_t sizea = Da.GetSizeofDataSet();
    size_t sizeb = Db.GetSizeofDataSet();

    size_t k = (sizea + sizeb) / 2;

    Da.Pad(k, std::numeric_limits<int>::max());
    Db.Pad(k, std::numeric_limits<int>::min());

    Da.PrintAllElement();
    Db.PrintAllElement();
}

int main()
{
    TestFunctionB();
    return 0;
}
