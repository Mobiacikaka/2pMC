#ifndef __PARTY_HPP__
#define __PARTY_HPP__

// ABY Libaraies
#include <abycore/circuit/share.h>
#include <abycore/circuit/booleancircuits.h>

#include "config.h"
#include "dataset.hpp"

class Party 
{
private:

protected:
	DataSet data_set;

	// auxiliary functions
	static inline size_t generate_s() {
		return kS;
	}
	std::vector<uint32_t> PutVectorBitonicSortGate(share** srv_set, share** cli_set, uint32_t neles, uint32_t bitlen, BooleanCircuit* circ);
	std::vector<uint32_t> PutVectorCondSwapGate(uint32_t a, uint32_t b, uint32_t s, BooleanCircuit* circ);
	std::vector<uint32_t> BuildMergeAndSortCircuit(share** srv_set, share** cli_set, share** r_srv_set,
		uint32_t neles, uint32_t bitlen, BooleanCircuit* bcirc);

	// Algorithm One
	virtual void Prune() = 0;
	// Algorithm Two
	virtual void MergeAndShare() = 0;
	// Algorithm Three
	// Algorithm Four

public:
	Party();
	virtual ~Party() = 0;

	void Run();

};

#endif