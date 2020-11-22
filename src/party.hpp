#pragma once

#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>

//Utility libs
#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>
//ABY Party class
#include <abycore/sharing/sharing.h>
#include <abycore/circuit/booleancircuits.h>
#include <abycore/aby/abyparty.h>

#include "dataset.hpp"

int32_t ReadTestOptions(int32_t*, char***, e_role*,	uint32_t*, 	
	uint32_t*, uint32_t*, std::string*,	uint16_t*, int32_t*);

/**
 * \warning suppose A is SERVER and B is CLIENT
 */ 

template<class T>
class Party 
{
private:
    DataSet<T> data_set;

    void Prune(size_t s, size_t k);

	//void InitABYParty();
	bool CompareMedianWithAnotherParty(T median);

	/**
	 * @param	role 
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

public:
    Party(e_role role, const std::string &address, uint16_t port, seclvl seclevel, uint32_t bitlen, uint32_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing);

    ~Party();

};

template<class T>
Party<T>::Party(e_role _role, const std::string &_address, 
		uint16_t _port, seclvl _seclevel, uint32_t _bitlen, 
		uint32_t _nthreads, e_mt_gen_alg _mt_alg, e_sharing _sharing)
	:role(_role), address(_address), port(_port), 
		seclevel(_seclevel), bitlen(_bitlen), 
		nthreads(_nthreads), mt_alg(_mt_alg), sharing(_sharing)
{
}

template<class T>
Party<T>::~Party()
{
}

template<class T>
void Party<T>::Prune(size_t s, size_t k)
{
    T p;
    
	if(this->role == SERVER) p = std::numeric_limits<T>::max();
	else if(this->role == CLIENT) p = std::numeric_limits<T>::min();
	else assert(0);

	this->data_set.Pad(k, p);
	
	for(size_t i = 0; i < s; i ++)
	{
		T median = this->data_set.GetMedian();

		// use garbled circuit to compute c
		bool c(false);
		//? Create ABYcircuit for every comparison or just for one time
		//assert(0);

		//! Create ABYParty every run.
		c = this->CompareMedianWithAnotherParty(median);

		if((this->role == SERVER && c == 1) || (this->role == CLIENT && c == 0)) 
		{
			// retain only the upper half
			this->data_set.KeepUpperHalf();
		}
		else
		{
			// retain only the lower half
			this->data_set.KeepLowerHalf();
		}
		
	}
}

template<class T>
bool Party<T>::CompareMedianWithAnotherParty(T median)
{
	ABYParty* abyparty = new ABYParty(this->role, 
		this->address, this->port, this->seclevel, 
		this->bitlen, this->nthreads, this->mt_alg);

	std::vector<Sharing*>& sharings = abyparty->GetSharings();

	Circuit* circ = sharings[sharing]->GetCircuitBuildRoutine();

	//! a is server, b is client
	share *s_a_median, *s_b_median, *s_out;

	if(this->role == SERVER)
	{
		s_a_median = circ->PutINGate(median, bitlen, SERVER);
		s_b_median = circ->PutDummyINGate(this->bitlen);
	}
	else // this->role == CLIENT
	{
		s_a_median = circ->PutDummyINGate(bitlen);
		s_b_median = circ->PutINGate(median, bitlen, CLIENT);
	}

	s_out = ((BooleanCircuit*) circ)->PutGTGate(s_a_median, s_b_median);

	s_out = circ->PutOUTGate(s_out, ALL);

	abyparty->ExecCircuit();

	delete abyparty;

	return s_out->get_clear_value<bool>();
}
