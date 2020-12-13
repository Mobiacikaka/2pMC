#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include "party.hpp"
class Client : public Party {
private:

protected:
    // auxiliary functions
    size_t generate_k();
    uint32_t comp_median();

    // main functions
    void Prune();
    void MergeAndShare();

public:
    Client();
    ~Client();

    void Run();
};

#endif