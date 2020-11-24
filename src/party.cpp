
#include <cmath>
#include <cassert>
#include <algorithm>

#include "party.hpp"

Party::Party(e_role _role, const std::string &_address, 
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

Party::~Party()
{
}


void Party::Run()
{
	this->data_set.Init();
	this->data_set.PrintAllElement();

	size_t s = this->Generates();
	size_t k = this->Generatek();

	this->Prune(s, k);

	this->data_set.PrintAllElement();
}


void Party::Prune(size_t s, size_t k)
{
    data_t p;
    
	if(this->role == SERVER) p = std::numeric_limits<data_t>::max();
	else p = std::numeric_limits<data_t>::min();

	this->data_set.Pad(k, p);
	assert(p == kA || p == kB);
	this->data_set.PrintAllElement();
	
	for(size_t i = 0; i < s; i ++)
	{
        std::cout << "Pruning Steps " << i+1 << std::endl;
		data_t median = this->data_set.GetMedian();

		// use garbled circuit to compute c
		bool c(false);

		//? Create ABYcircuit for every comparison or just for one time
		//! Create ABYParty every run.
		c = this->CompareMedianWithAnotherParty(median);

		DEBUG_INFO
		if((this->role == SERVER && c == true) || (this->role == CLIENT && c == false)) 
			this->data_set.KeepUpperHalf();
		else
			this->data_set.KeepLowerHalf();
	}

	std::cout << "Finish Prune" << std::endl;
}


uint32_t Party::CompareMedianWithAnotherParty(data_t median)
{
	std::unique_ptr<ABYParty> abyparty = std::make_unique<ABYParty>(this->role, 
		this->address, this->port, this->seclevel, 
		this->bitlen, this->nthreads, this->mt_alg);

	std::vector<Sharing*>& sharings = abyparty->GetSharings();

	Circuit* circ = sharings[sharing]->GetCircuitBuildRoutine();

	//! a is server, b is client
	share *s_a_median, *s_b_median, *s_out;
	uint32_t output(0);

	if(this->role == SERVER)
	{
		s_b_median = circ->PutDummyINGate(this->bitlen);
		s_a_median = circ->PutINGate(median, bitlen, SERVER);
	}
	else // this->role == CLIENT
	{
		s_b_median = circ->PutINGate(median, bitlen, CLIENT);
		s_a_median = circ->PutDummyINGate(bitlen);
	}

	s_out = static_cast<BooleanCircuit*>(circ)->PutGTGate(s_a_median, s_b_median);

	s_out = circ->PutOUTGate(s_out, ALL);

	abyparty->ExecCircuit();

	output = s_out->get_clear_value<uint32_t>();

	delete abyparty;

	return output;
}


size_t Party::Generates()
{
	return kS;
}


size_t Party::Generatek()
{
	std::unique_ptr<CSocket> tsocket; // temporary socket
	size_t sizea(this->data_set.GetSizeofDataSet());
	size_t sizeb(0);

	switch (this->role)
	{

	case SERVER:
		tsocket = Listen(this->address, this->port);
		if(!tsocket) {
			std::cerr << "Listen failed!" << std::endl;
			std::exit(1);
		}
		
		tsocket->Receive(static_cast<void*>(&sizeb), sizeof(size_t));
		tsocket->Send(static_cast<void*>(&sizea), sizeof(size_t));
		tsocket->Close();
		break;

	case CLIENT:
		tsocket = Connect(this->address, this->port);
		if(!tsocket) {
			std::cerr << "Listen failed!" << std::endl;
			std::exit(1);
		}

		tsocket->Send(static_cast<void*>(&sizea), sizeof(size_t));
		tsocket->Receive(static_cast<void*>(&sizeb), sizeof(size_t));
		tsocket->Close();
		break;
	
	default:
		break;
	}

	return (sizea + sizeb) / 2;
}