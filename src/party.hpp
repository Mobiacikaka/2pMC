#pragma once

#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>

//Utility libs
#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>
//ABY Party class
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

    e_role role;

    void Prune(size_t s, size_t k);

	//void InitABYParty();

public:
    Party();

    ~Party();

	// Run Program
	void Run(int, char**);

};

template<class T>
void Party<T>::Run(int argc, char **argv)
{
	e_role role;
	uint32_t bitlen = 32, nvals = 31, secparam = 128, nthreads = 1;
	uint16_t port = 7766;
	std::string address = "127.0.0.1";
	int32_t test_op = -1;
	e_mt_gen_alg mt_alg = MT_OT;

	ReadTestOptions(&argc, &argv, &role, &bitlen, &nvals, &secparam, &address, &port, &test_op);

	seclvl seclvl = get_sec_lvl(secparam);

	//evaluate the millionaires circuit using Yao
	//test_millionaire_prob_circuit(role, address, port, seclvl, 32, nthreads, mt_alg, S_YAO);
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
		assert(0);

		if((this->role == SERVER && c == 1)&&(this->role == CLIENT && c == 0)) 
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
