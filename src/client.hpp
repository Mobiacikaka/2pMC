#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include "party.hpp"
class Client : public Party {
private:
    const e_role role;
	const std::string address;
	uint16_t port;
	seclvl seclevel;
	uint32_t bitlen;
	uint32_t nthreads;
	e_mt_gen_alg mt_alg;

    // Arithmetic Share
    data_t* shr_dataset;

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