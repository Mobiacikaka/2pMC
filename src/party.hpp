#pragma once

#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>

//Utility libs
#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>
#include <ENCRYPTO_utils/socket.h>
#include <ENCRYPTO_utils/connection.h>
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

	/**
	 * @param s pruning steps s
	 * @param k median rank k = ceil( (|DA|+|DB|)/2 )
	*/
    void Prune(size_t s, size_t k);

	bool CompareMedianWithAnotherParty(T median);

	// Generate s and k for prune
	size_t Generates();
	size_t Generatek();

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

public:
    Party(e_role role, const std::string &address, uint16_t port, seclvl seclevel, uint32_t bitlen, uint32_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing);

    ~Party();

	void Run();
};

template<class T>
Party<T>::Party(e_role _role, const std::string &_address, 
		uint16_t _port, seclvl _seclevel, uint32_t _bitlen, 
		uint32_t _nthreads, e_mt_gen_alg _mt_alg, e_sharing _sharing)
		:role(_role), address(_address), 
		port(_port), seclevel(_seclevel), bitlen(_bitlen), 
		nthreads(_nthreads), mt_alg(_mt_alg), sharing(_sharing)
{
	assert(this->role == SERVER || this->role == CLIENT);

	// ! bit length must be 32
	assert(bitlen == 32);
	
	// ! sharing type must be YAO's
	assert(this->sharing == S_YAO);
}

template<class T>
Party<T>::~Party()
{
}

template<class T>
void Party<T>::Run()
{
	size_t s = this->Generates();
	size_t k = this->Generatek();
	this->Prune(s, k);
}

template<class T>
void Party<T>::Prune(size_t s, size_t k)
{
    T p;
    
	if(this->role == SERVER) p = std::numeric_limits<T>::max();
	else p = std::numeric_limits<T>::min();

	this->data_set.Pad(k, p);
	
	for(size_t i = 0; i < s; i ++)
	{
		T median = this->data_set.GetMedian();

		// use garbled circuit to compute c
		bool c(false);

		//? Create ABYcircuit for every comparison or just for one time
		//! Create ABYParty every run.
		c = this->CompareMedianWithAnotherParty(median);

		if((this->role == SERVER && c == 1) || (this->role == CLIENT && c == 0)) 
			this->data_set.KeepUpperHalf();
		else
			this->data_set.KeepLowerHalf();
	}

	std::cout << "Finish Prune" << std::endl;
	this->data_set.PrintAllElement();
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

	s_out = static_cast<BooleanCircuit*>(circ)->PutGTGate(s_a_median, s_b_median);

	s_out = circ->PutOUTGate(s_out, ALL);

	abyparty->ExecCircuit();

	delete abyparty;

	return s_out->get_clear_value<bool>();
}

template<class T>
size_t Party<T>::Generates()
{
	return 10;
}

template<class T>
size_t Party<T>::Generatek()
{
	std::unique_ptr<CSocket> tsocket; // temporary socket
	size_t sizea(this->data_set.GetSizeofDataSet());
	size_t sizeb(0);
	size_t k(0);

	switch (this->role)
	{

	case SERVER:
		tsocket = Listen(this->address, this->port);
		if(!tsocket) {
			std::cerr << "Listen failed!" << std::endl;
			std::exit(1);
		}
		
		tsocket->Receive(static_cast<void*>(&sizeb), sizeof(size_t));
		k = std::ceil((sizea + sizeb) / 2);
		tsocket->Send(static_cast<void*>(&k), sizeof(size_t));
		tsocket->Close();
		break;

	case CLIENT:
		tsocket = Connect(this->address, this->port);
		if(!tsocket) {
			std::cerr << "Listen failed!" << std::endl;
			std::exit(1);
		}

		tsocket->Send(static_cast<void*>(&sizea), sizeof(size_t));
		tsocket->Receive(static_cast<void*>(&k), sizeof(size_t));
		tsocket->Close();
		break;
	
	default:
		break;
	}

	return k;
}
