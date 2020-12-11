#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>

#include <abycore/sharing/sharing.h>
#include <abycore/aby/abyparty.h>
#include <abycore/circuit/arithmeticcircuits.h>
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

/*
std::vector<uint32_t> BuildMergeAndSortCircuit(share** srv_set, share** cli_set, share** r_srv_set,
		uint32_t neles, uint32_t bitlen, ArithmeticCircuit* acirc, BooleanCircuit* bcirc) {

	std::vector<uint32_t> a = PutVectorBitonicSortGate(srv_set, cli_set, neles, bitlen, bcirc);

	// std::vector<uint32_t> b(2 * neles);
	share** b = (share**) malloc(sizeof(share*) * 2 * neles);

	for(size_t i = 0; i < 2*neles; i ++) {
		b[i] = new 
	}

	for(size_t i = 0; i < 2*neles; i ++) {
		bcirc->PutSUBGate()
	}

	// for(size_t i = 0; i < a.size(); i ++)
	// 	std::cout << a[i] << " ";
	// std::cout << std::endl;
	// for(size_t i = 0; i < b.size(); i ++)
	// 	std::cout << b[i] << " ";
	// std::cout << std::endl;

	// std::vector<uint32_t> c = bcirc->PutSUBGate(a, b, 2*neles);
	// std::vector<uint32_t> c(2*neles);
	// for(size_t i = 0; i < 2 * neles; i ++) {
	// 	c[i] = acirc->PutSUBGate(a[i], b[i]);
	// }
	// std::cout << "c size: " << c.size() << std::endl;
	assert(c.size() == 2*neles);

	// for(size_t i = 0; i < c.size(); i ++) 
	// 	std::cout << c[i] << " ";
	// std::cout << std::endl;

	// std::cout << "b size: " << b.size() << std::endl;

	return a;
}
*/

std::vector<uint32_t> BuildMergeAndSortCircuit2(share** srv_set, share** cli_set, share** r_srv_set,
		uint32_t neles, uint32_t bitlen, BooleanCircuit* bcirc) {

	std::vector<uint32_t> a = PutVectorBitonicSortGate(srv_set, cli_set, neles, bitlen, bcirc);

	// bcirc->PutSUBGate()

	return std::vector<uint32_t>(1);
	
}

std::vector<uint32_t> BuildMergeAndSortCircuit(share** srv_set, share** cli_set, share** r_srv_set,
		uint32_t neles, uint32_t bitlen, BooleanCircuit* bcirc) {

	std::vector<uint32_t> a = PutVectorBitonicSortGate(srv_set, cli_set, neles, bitlen, bcirc);

	std::vector<uint32_t> c(2*neles);

	std::vector<uint32_t> b = r_srv_set[0]->get_wires();
	for(size_t i = 0; i < b.size(); i ++) std::cout << b[i] << " ";
	std::cout << std::endl;

	for(size_t i = 0; i < 2*neles; i ++) {
		boolshare tmp_share(1, bcirc);
		tmp_share.set_wire_id(0, a[i]);
		c[i] = bcirc->PutSUBGate(r_srv_set[i], &tmp_share)->get_wire_id(0);
	}

	return c;
}

int32_t test_merge_and_sort(e_role role, const std::string& address, uint16_t port, 
        seclvl seclevel, uint32_t neles, uint32_t bitlen, uint32_t nthreads, 
        e_mt_gen_alg mt_alg, uint32_t prot_version, bool verify)
{
	uint32_t *m_set, *r_m_set;
	share **shr_srv_set, **shr_cli_set, **r_shr_srv_set, **shr_out;
	e_sharing sharing_merge(S_ARITH);
	uint64_t mask = 0b11111;

	assert(sharing_merge == S_ARITH);

	ABYParty* party = new ABYParty(role, address, port, seclevel, bitlen, nthreads, mt_alg, 4000000);

	std::vector<Sharing*>& sharings = party->GetSharings();

	BooleanCircuit* bcirc = (BooleanCircuit*) sharings[S_BOOL]->GetCircuitBuildRoutine();

	m_set = (uint32_t*) malloc(sizeof(uint32_t) * neles);
	r_m_set = (uint32_t*) malloc(sizeof(uint32_t) * 2 * neles);
	shr_srv_set = (share**) malloc(sizeof(share*) * neles);
	shr_cli_set = (share**) malloc(sizeof(share*) * neles);
	r_shr_srv_set = (share**) malloc(sizeof(share*) * 2 * neles);

	srand(time(0));
	for (size_t i = 0; i < neles; i ++) {
		m_set[i] = rand() & mask;
	}

	std::sort(m_set, m_set + neles);

#ifdef NDEBUG
	for (size_t i = 0; i < neles; i ++) 
		std::cout << m_set[i] << " ";
	std::cout << std::endl;
#endif

	if(role == SERVER) {
		for (size_t i = 0; i < 2*neles; i ++) {
			r_m_set[i] = rand() & mask;
		}
	}

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

	for (size_t i = 0; i < 2 * neles; i ++) {
		if(role == SERVER) r_shr_srv_set[i] = bcirc->PutSIMDINGate(bitlen, r_m_set[i], 1, SERVER);
		else r_shr_srv_set[i] = bcirc->PutDummySIMDINGate(bitlen, 1);
	}

	// ! Build Circuit
	std::vector<uint32_t> out = BuildMergeAndSortCircuit(shr_srv_set, shr_cli_set, r_shr_srv_set, neles, bitlen, bcirc);

	shr_out = (share**) malloc(sizeof(share*) * out.size());
	for (size_t i = 0; i < out.size(); i ++) {
		shr_out[i] = new boolshare(1, bcirc);
		shr_out[i]->set_wire_id(0, out[i]);
		shr_out[i] = bcirc->PutOUTGate(shr_out[i], CLIENT);
	}

	party->ExecCircuit();
	std::cout << "ExecCircuit Success" << std::endl;

	if(role == CLIENT) {
		assert(out.size() == 2*neles);
		for(uint32_t i = 0; i < out.size(); i ++) {
			r_m_set[i] = shr_out[i]->get_clear_value<uint32_t>();
		}
	}

#ifdef NDEBUG
	for(uint32_t i = 0; i < 2*neles; i ++) {
		std::cout << r_m_set[i] << "\t";
	}
	std::cout << std::endl;
#endif

	delete party;

	free(m_set);
	free(r_m_set);
	free(shr_srv_set);
	free(shr_cli_set);
	free(r_shr_srv_set);
	for(size_t i = 0; i < 2 * neles; i ++) delete shr_out[i];
	free(shr_out);

	return 0;
}