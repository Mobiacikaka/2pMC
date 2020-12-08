
#include <cmath>
#include <cassert>
#include <vector>
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

	this->MergeAndShare();
}


void Party::Prune(size_t s, size_t k)
{
    data_t p;
    
	if(this->role == SERVER) p = std::numeric_limits<data_t>::max();
	else p = std::numeric_limits<data_t>::min();

	this->data_set.Pad(k, p);
	assert(p == kA || p == kB);
	this->data_set.PrintAllElement();
	
	// TODO: optimization - pre-select some s and put in the circuit, compute at one time
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

	Circuit* circ = sharings[this->sharing]->GetCircuitBuildRoutine();

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

	// delete abyparty;
	abyparty = nullptr;

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

void Party::MergeAndShare()
{
	std::unique_ptr<ABYParty> party = std::make_unique<ABYParty>(this->role, 
		this->address, this->port, this->seclevel, 
		this->bitlen, this->nthreads, this->mt_alg);

	std::vector<Sharing*>& sharings = party->GetSharings();

	assert(this->sharing == S_ARITH);

	ArithmeticCircuit* circ = static_cast<ArithmeticCircuit*>(sharings[this->sharing]->GetCircuitBuildRoutine());
	assert(circ->GetCircuitType() == C_ARITHMETIC);
	
	// the data set must be sorted
	assert(this->data_set.IsSorted() == true);

	size_t neles;
	share **shr_srv_set, **shr_cli_set, **shr_out;

	neles		 = this->data_set.GetSizeofDataSet();
	shr_srv_set  = static_cast<share**>(malloc(sizeof(share*) * neles));
	shr_cli_set  = static_cast<share**>(malloc(sizeof(share*) * neles));
	shr_out		 = static_cast<share**>(malloc(sizeof(share*) * neles));

	for (size_t i = 0; i < neles; i ++) {
		if(this->role == SERVER) {
			shr_srv_set[i] = this->data_set.PutSIMDINGate(circ, bitlen, i, 1, this->role);
			shr_cli_set[i] = circ->PutDummySIMDINGate(bitlen, 1);
		}
		else {
			shr_srv_set[i] = circ->PutDummySIMDINGate(bitlen, 1);
			shr_cli_set[i] = this->data_set.PutSIMDINGate(circ, bitlen, i, 1, this->role);
		}
	}

	std::vector<uint32_t> out = this->PutVectorBitonicSortGate(shr_srv_set, shr_cli_set, neles, bitlen, circ);

	party->ExecCircuit();

	party = nullptr;
}

std::vector<uint32_t> Party::PutVectorBitonicSortGate(share** srv_set, share** cli_set, uint32_t neles,
		uint32_t bitlen, ArithmeticCircuit* circ) {

	uint32_t seqsize = 2*neles;
	uint32_t selbitsvec;
	uint32_t i, k, ctr;
	int32_t j;

	std::vector<uint32_t> compa(seqsize / 2);
	std::vector<uint32_t> compb(seqsize / 2);
	std::vector<uint32_t> posa(seqsize / 2);
	std::vector<uint32_t> posb(seqsize / 2);
	//share **c, *selbits;

	std::vector<uint32_t> selbits;
	std::vector<uint32_t> c(seqsize);
	std::vector<uint32_t> temp;
	std::vector<uint32_t> tempcmpveca(bitlen);
	std::vector<uint32_t> tempcmpvecb(bitlen);

	std::vector<uint32_t> parenta(seqsize / 2);
	std::vector<uint32_t> parentb(seqsize / 2);


	//c = (share**) malloc(sizeof(share*) * seqsize);

	//Combine all values of a and b into a single vector c
	for (i = 0; i < neles; i++) {
		c[i] = srv_set[i]->get_wire_id(0);
		c[i + neles] = cli_set[i]->get_wire_id(0);
	}

	//Build bitonic sort gate for all values in C
	for (i = 1 << floor_log2(seqsize - 1); i > 0; i >>= 1) {
		ctr = 0;
		for (j = seqsize - 1, ctr = 0; j >= 0; j -= 2 * i) {
			for (k = 0; k < i && j - i - k >= 0; k++) {
				compa[ctr] = j - i - k;
				compb[ctr] = j - k;
				ctr++;
			}
		}

		//TODO: Introduce specific gate that allows the permutation of vector gates from different input gates + bit positions

		for (uint32_t l = 0; l < bitlen; l++) {
			//cout << "l = " << l << endl;
			for (k = 0; k < ctr; k++) {
				parenta[k] = c[compa[k]];
				parentb[k] = c[compb[k]];
				posa[k] = l;
				posb[k] = l;
			}
			tempcmpveca[l] = circ->PutCombineAtPosGate(parenta, l);
			tempcmpvecb[l] = circ->PutCombineAtPosGate(parentb, l);
		}

		selbitsvec = circ->PutGTGate(tempcmpveca, tempcmpvecb);

		selbits = circ->PutSplitterGate(selbitsvec);
		for (k = 0; k < ctr; k++) {
			temp = PutVectorCondSwapGate(c[compa[k]], c[compb[k]], selbits[k], circ);
			c[compa[k]] = temp[0];
			c[compb[k]] = temp[1];
		}
	}

	return c;
}

std::vector<uint32_t> Party::PutVectorCondSwapGate(uint32_t a, uint32_t b, uint32_t s, ArithmeticCircuit* circ) {
	std::vector<uint32_t> avec(1, a);
	std::vector<uint32_t> bvec(1, b);
	std::vector<uint32_t> out(2);
	//uint32_t svec = circ->PutRepeaterGate(s, 32);
	std::vector<std::vector<uint32_t> > temp = circ->PutCondSwapGate(avec, bvec, s, true);
	out[0] = temp[0][0];
	out[1] = temp[1][0];
	return out;
}