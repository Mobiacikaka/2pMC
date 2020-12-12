//utilities
#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>
#include <ENCRYPTO_utils/socket.h>
#include <ENCRYPTO_utils/connection.h>
//ABY Party class
#include <abycore/sharing/sharing.h>
#include <abycore/circuit/arithmeticcircuits.h>
#include <abycore/circuit/booleancircuits.h>
#include <abycore/aby/abyparty.h>
//system
#include <cassert>

#include "client.hpp"

Client::Client() 
    : role(SERVER)
{

}

Client::~Client() {

}

size_t Client::generate_k() {
    std::unique_ptr<CSocket> tsocket;
    size_t srv_size(0);
	size_t cli_size(this->data_set.GetSizeofDataSet());

    tsocket = Connect(this->address, this->port);
    if(!tsocket) {
		std::cerr << "Listen failed!" << std::endl;
		std::exit(1);
    }

    tsocket->Send(static_cast<void*>(&cli_size), sizeof(size_t));
    tsocket->Receive(static_cast<void*>(&srv_size), sizeof(size_t));
    tsocket->Close();

    return (srv_size + cli_size) / 2;
}

uint32_t Client::comp_median() {
    const data_t median = data_set.GetMedian();

    ABYParty* party = new ABYParty(role, address, port, seclevel, bitlen, nthreads, mt_alg);
    std::vector<Sharing*>& sharings = party->GetSharings();

    BooleanCircuit* circ = (BooleanCircuit*) sharings[S_YAO]->GetCircuitBuildRoutine();

    share *shr_srv_median, *shr_cli_median, *shr_out;

    shr_srv_median = circ->PutDummyINGate(bitlen);
    shr_cli_median = circ->PutINGate(median, bitlen, role);

    shr_out = circ->PutGTGate(shr_srv_median, shr_cli_median);
    shr_out = circ->PutOUTGate(shr_out, ALL);

    party->ExecCircuit();

    uint32_t o(shr_out->get_clear_value<uint32_t>());

    delete party;

    return o;
}

void Client::Prune() {
    const data_t padding = std::numeric_limits<data_t>::min();
    const size_t s = this->generate_s();
    const size_t k = this->generate_k();
	bool comp(false);

    assert(padding == kA || padding == kB);

    data_set.Pad(k, padding);

    for (size_t i = 0; i < s; i ++) {
        // std::cout << "Pruning Steps " << i+1 << std::endl;

		comp = comp_median();
        assert(comp == 1 || comp == 0);

        if(comp == true) data_set.KeepLowerHalf();
        else data_set.KeepUpperHalf();
    }
}

void Client::MergeAndShare() {
    assert(data_set.IsSorted() == true);

	ABYParty* party = new ABYParty(role, address, port, seclevel, bitlen, nthreads, mt_alg, 4000000);
	std::vector<Sharing*>& sharings = party->GetSharings();

	BooleanCircuit* bcirc = (BooleanCircuit*) sharings[S_BOOL]->GetCircuitBuildRoutine();

    size_t neles = data_set.GetSizeofDataSet();
    size_t shrsize = 2 * neles;
    data_t* rnd_cli_set = (data_t*) malloc(sizeof(data_t) * shrsize);
    share** shr_srv_set = (share**) malloc(sizeof(share*) * neles);
    share** shr_cli_set = (share**) malloc(sizeof(share*) * neles);
    share** shr_rnd_srv_set = (share**) malloc(sizeof(share*) * shrsize);
    share** shr_out = (share**) malloc(sizeof(data_t) * shrsize);

    for (size_t i = 0; i < neles; i ++) {
        shr_srv_set[i] = bcirc->PutDummySIMDINGate(bitlen, 1);
        shr_cli_set[i] = bcirc->PutSIMDINGate(bitlen, data_set[i], 1, role);
    }

    for (size_t i = 0; i < shrsize; i ++) {
        shr_rnd_srv_set[i] = bcirc->PutDummySIMDINGate(bitlen, 1);
    }

	std::vector<uint32_t> out = BuildMergeAndSortCircuit(shr_srv_set, shr_cli_set, shr_rnd_srv_set, neles, bitlen, bcirc);

    for (size_t i = 0; i < shrsize; i ++) {
        shr_out[i] = new boolshare(1, bcirc);
        shr_out[i]->set_wire_id(0, out[i]);
        shr_out[i] = bcirc->PutOUTGate(shr_out[i], CLIENT);
    }

    party->ExecCircuit();

    for(size_t i = 0; i < out.size(); i ++) {
        rnd_cli_set[i] = shr_out[i]->get_clear_value<data_t>();
    }

    // delete operation
    delete party;
    shr_dataset = rnd_cli_set;
    free(shr_srv_set);
    free(shr_cli_set);
    free(shr_rnd_srv_set);
    for(size_t i = 0; i < shrsize; i ++) delete shr_out[i];
    free(shr_out);
}
