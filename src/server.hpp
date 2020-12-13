#ifndef __SERVER_HPP_
#define __SERVER_HPP_

#include "party.hpp"

class Server : public Party {
private:

protected:
    // auxiliary functions
    size_t generate_k();
    uint32_t comp_median();

    // main functions
    void Prune();
    void MergeAndShare();

public:
    Server();
    ~Server();

    void Run();
};

#endif