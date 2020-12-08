#ifndef __PARTY_HPP__
#define __PARTY_HPP__

//Utility libs
#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>
#include <ENCRYPTO_utils/socket.h>
#include <ENCRYPTO_utils/connection.h>
//ABY Party class
#include <abycore/sharing/sharing.h>
#include <abycore/circuit/arithmeticcircuits.h>
#include <abycore/aby/abyparty.h>

#include "config.h"
#include "dataset.hpp"

/**
 * \warning suppose A is SERVER and B is CLIENT
 */ 

class Party 
{
private:
    DataSet data_set;

	/**
	 * @param	role SERVER or CLIENT 
	 * @param	address
	 * @param	port
	 * @param	seclvl security level
	 * @param	bitlen
	 * @param	nthreads number of threads
	 * @param	mt_alg
	 * @param	sharing sharing type
	 */ 
    e_role role;
	const std::string address;
	uint16_t port;
	seclvl seclevel;
	uint32_t bitlen;
	uint32_t nthreads;
	e_mt_gen_alg mt_alg;
	e_sharing sharing;

	/** 
	 * @brief Algorithm One
	 * @param s pruning steps s
	 * @param k median rank k = ceil( (|DA|+|DB|)/2 )
	*/
    void Prune(size_t s, size_t k);
		uint32_t CompareMedianWithAnotherParty(data_t median);
	/**
	 * @brief Algorithm Two
	 */ 
	void MergeAndShare();
		std::vector<uint32_t> PutVectorBitonicSortGate(share** srv_set, share** cli_set, uint32_t neles,
			uint32_t bitlen, ArithmeticCircuit* circ);
		std::vector<uint32_t> PutVectorCondSwapGate(uint32_t a, uint32_t b, uint32_t s, ArithmeticCircuit* circ);

	// Generate s and k for prune
	size_t Generates();
	size_t Generatek();

public:
    Party(e_role role, const std::string &address, 
		uint16_t port, seclvl seclevel, uint32_t bitlen, 
		uint32_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing);

    ~Party();

	void Run();
};


#endif