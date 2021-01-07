#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>

#include <abycore/sharing/sharing.h>
#include <abycore/aby/abyparty.h>
#include <abycore/circuit/arithmeticcircuits.h>
#include <abycore/circuit/booleancircuits.h>

#include "simd_add.h"

int32_t test_simd_add(e_role role, const std::string& address, uint16_t port, 
        seclvl seclevel, uint32_t neles, uint32_t bitlen, uint32_t nthreads, 
        e_mt_gen_alg mt_alg, uint32_t prot_version, bool verify)
{
	uint32_t *m_set, *r_m_set;
	share **shr_srv_set, **shr_cli_set, **r_shr_srv_set, **shr_out;
	e_sharing sharing_merge(S_ARITH);
	uint64_t mask = 0b11111;
	std::string str_role = role == SERVER ? "SERVER" : "CLIENT";

	ABYParty* party = new ABYParty(role, address, port, seclevel, bitlen, nthreads, mt_alg, 4000000);
	std::vector<Sharing*>& sharings = party->GetSharings();
	BooleanCircuit* bcirc = (BooleanCircuit*) sharings[S_BOOL]->GetCircuitBuildRoutine();

	uint32_t list_srv[] = {1, 1, 1, 1, 1};
	uint32_t list_cli[] = {1, 2, 3, 4, 5};

	share *srv, *cli;
	if(role == SERVER) {
		srv = bcirc->PutSIMDINGate(5, list_srv, bitlen, role);
		cli = bcirc->PutDummySIMDINGate(5, bitlen);
	}
	else {
		srv = bcirc->PutDummySIMDINGate(5, bitlen);
		cli = bcirc->PutSIMDINGate(5, list_cli, bitlen, role);
	}

	bcirc->PutPrintValueGate(srv, "srv");
	bcirc->PutPrintValueGate(cli, "cli");

	share *tmp_shr = bcirc->PutCONSGate((uint32_t)-0, bitlen);
	bcirc->PutPrintValueGate(tmp_shr, "-1");

	// shr_srv_set = (share**) malloc(sizeof(share*) * 5);
	// shr_cli_set = (share**) malloc(sizeof(share*) * 5);
	// shr_out		= (share**) malloc(sizeof(share*) * 5);

	// for(size_t i = 0; i < 5; i ++) {
	// 	if (role == SERVER) {
	// 		shr_srv_set[i] = bcirc->PutSIMDINGate(bitlen, list_srv[i], 1, role);
	// 		shr_cli_set[i] = bcirc->PutDummySIMDINGate(bitlen, 1);
	// 	}
	// 	else {
	// 		shr_srv_set[i] = bcirc->PutDummySIMDINGate(bitlen, 1);
	// 		shr_cli_set[i] = bcirc->PutSIMDINGate(bitlen, list_cli[i], 1, role);
	// 	}
	// 	share* hor = bcirc->PutSplitterGate(shr_cli_set[i]);
	// 	bcirc->PutPrintValueGate(hor, "horizon");
	// 	shr_out[i] = bcirc->PutADDGate(shr_srv_set[i], shr_cli_set[i]);
	// 	shr_out[i] = bcirc->PutOUTGate(shr_out[i], ALL);
	// }

	// for(size_t i = 0; i < 5; i ++) {
	// 	share *srv, *cli, *add;
	// 	if(role == SERVER) {
	// 		srv = bcirc->PutINGate(list_srv[i], bitlen, role);
	// 		cli = bcirc->PutDummyINGate(bitlen);
	// 	}
	// 	else {
	// 		srv = bcirc->PutDummyINGate(bitlen);
	// 		cli = bcirc->PutINGate(list_cli[i], bitlen, role);
	// 	}
	// 	bcirc->PutPrintValueGate(srv, "srv");
	// 	bcirc->PutPrintValueGate(cli, "cli");
	// 	add = bcirc->PutADDGate(srv, cli);
	// 	shr_out[i] = bcirc->PutOUTGate(add, ALL);
	// 	delete srv, cli, add;
	// }

	party->ExecCircuit();

	// for(size_t i = 0; i < 5; i ++) 
	// 	std::cout << shr_out[i]->get_clear_value<uint32_t>() << std::endl;

	delete party;

	// for(size_t i = 0; i < 5; i ++) delete shr_srv_set[i];
	// free(shr_srv_set);
	// for(size_t i = 0; i < 5; i ++) delete shr_cli_set[i];
	// free(shr_cli_set);
	// for(size_t i = 0; i < 5; i ++) delete shr_out[i];
	// free(shr_out);

	return 0;
}
