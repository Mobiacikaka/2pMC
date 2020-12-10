#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>

#include <abycore/sharing/sharing.h>
#include <abycore/aby/abyparty.h>
#include <abycore/circuit/booleancircuits.h>

#include "merge_and_sort.h"

//CondSwapGates for vectorwise processing
std::vector<uint32_t> PutVectorCondSwapGate(uint32_t a, uint32_t b, uint32_t s, BooleanCircuit* circ) {
	std::vector<uint32_t> avec(1, a);
	std::vector<uint32_t> bvec(1, b);
	std::vector<uint32_t> out(2);
	//uint32_t svec = circ->PutRepeaterGate(s, 32);
	std::vector<std::vector<uint32_t>> temp = circ->PutCondSwapGate(avec, bvec, s, true);
	out[0] = temp[0][0];
	out[1] = temp[1][0];
	return out;
}

//vector<uint32_t> PutVectorBitonicSortGate(vector<uint32_t>& a, vector<uint32_t>& b, uint32_t bitlen, BooleanCircuit* circ) {
std::vector<uint32_t> PutVectorBitonicSortGate(share** srv_set, share** cli_set, uint32_t neles,
		uint32_t bitlen, BooleanCircuit* circ) {

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

int32_t test_merge_and_sort(e_role role, const std::string& address, uint16_t port, 
        seclvl seclevel, uint32_t neles, uint32_t bitlen, uint32_t nthreads, 
        e_mt_gen_alg mt_alg, uint32_t prot_version, bool verify)
{
	uint32_t *m_set;
	share **shr_srv_set, **shr_cli_set, **shr_out;
	e_sharing sharing_merge(S_ARITH);
	uint64_t mask = 0b11111;

	assert(neles < mask);
	assert(sharing_merge == S_ARITH);

	ABYParty* party = new ABYParty(role, address, port, seclevel, bitlen, nthreads, mt_alg, 4000000);

	std::vector<Sharing*>& sharings = party->GetSharings();

	BooleanCircuit* bcirc = (BooleanCircuit*) sharings[S_BOOL]->GetCircuitBuildRoutine();

	m_set = (uint32_t*) malloc(sizeof(uint32_t) * neles);
	shr_srv_set = (share**) malloc(sizeof(share*) * neles);
	shr_cli_set = (share**) malloc(sizeof(share*) * neles);

	srand(time(0));
	for (size_t i = 0; i < neles; i ++) {
		uint32_t rndval;
		do {
			rndval = rand() & mask;
		} while (std::find(m_set, m_set+neles, rndval) != m_set+neles);

		m_set[i] = rndval;
	}

	std::sort(m_set, m_set + neles);

	for (size_t i = 0; i < neles; i ++) 
		std::cout << m_set[i] << " ";
	std::cout << std::endl;

	for (uint32_t i = 0; i < neles; i++) {
		// Version 1
		if(role == SERVER) {
			shr_srv_set[i] = bcirc->PutSIMDINGate(bitlen, m_set[i], 1, SERVER);
			shr_cli_set[i] = bcirc->PutDummySIMDINGate(bitlen, 1);
		}
		else {
			shr_srv_set[i] = bcirc->PutDummySIMDINGate(bitlen, 1);
			shr_cli_set[i] = bcirc->PutSIMDINGate(bitlen, m_set[neles-1-i], 1, CLIENT);
		}

	}

	std::vector<uint32_t> out = PutVectorBitonicSortGate(shr_srv_set, shr_cli_set, neles, bitlen, bcirc);

	shr_out = (share**) malloc(sizeof(share*) * out.size());
	for (size_t i = 0; i < out.size(); i ++) {
		shr_out[i] = new boolshare(1, bcirc);
		shr_out[i]->set_wire_id(0, out[i]);
		shr_out[i] = bcirc->PutOUTGate(shr_out[i], CLIENT);
	}

	party->ExecCircuit();

	if(role == CLIENT) {
		for(uint32_t i = 0; i < out.size(); i ++) {
			std::cout << shr_out[i]->get_clear_value<uint32_t>() << " ";
		}
		std::cout << std::endl;
	}

	delete party;

	free(m_set);
	free(shr_srv_set);
	free(shr_cli_set);
	free(shr_out);

	return 0;
}