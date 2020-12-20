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

uint64_t Client::generate_R() {
    std::unique_ptr<CSocket> tsocket;
    double mass_srv(0);
	double mass_cli(this->shr_mass[this->shr_mass.size()-1]);

    tsocket = Connect(this->address, this->port);
    if(!tsocket) {
		std::cerr << "Listen failed!" << std::endl;
		std::exit(1);
    }

    tsocket->Send   (static_cast<void*>(&mass_cli), sizeof(double));
    tsocket->Receive(static_cast<void*>(&mass_srv), sizeof(double));
    tsocket->Close();

    return static_cast<uint64_t>(mass_srv+mass_cli);
}

data_t Client::xor_nonces(data_t nonces_cli) {
    ABYParty* party = new ABYParty(role, address, port, seclevel, bitlen, nthreads, mt_alg, 4000000);
    std::vector<Sharing*>& sharings = party->GetSharings();

    BooleanCircuit* circ = (BooleanCircuit*) sharings[S_BOOL]->GetCircuitBuildRoutine();

    share *shr_srv, *shr_cli, *shr_xor, *shr_out;

    shr_srv = circ->PutDummyINGate(bitlen);
    shr_cli = circ->PutINGate(nonces_cli, bitlen, role);

    shr_xor = circ->PutXORGate(shr_srv, shr_cli);
    shr_out = circ->PutOUTGate(shr_xor, ALL);

    party->ExecCircuit();

    uint32_t o(shr_out->get_clear_value<data_t>());

    delete party, shr_srv, shr_cli, shr_xor, shr_out;

    return o;
}

void Client::Prune() {
    const data_t padding = kB;
    const size_t s = this->generate_s();
    const size_t k = this->generate_k();
	bool comp(false);

    assert(padding == kA || padding == kB);

    this->m_k = k;

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
    share** shr_srv_set = (share**) malloc(sizeof(share*) * neles);
    share** shr_cli_set = (share**) malloc(sizeof(share*) * neles);
    share** shr_rnd_srv_set = (share**) malloc(sizeof(share*) * shrsize);
    share** shr_out = (share**) malloc(sizeof(share*) * shrsize);

    for (size_t i = 0; i < neles; i ++) {
        shr_srv_set[i] = bcirc->PutDummySIMDINGate(bitlen, 1);
        shr_cli_set[i] = bcirc->PutSIMDINGate(bitlen, data_set[neles-1-i], 1, role);
    }

    for (size_t i = 0; i < shrsize; i ++) {
        shr_rnd_srv_set[i] = bcirc->PutDummySIMDINGate(bitlen, 1);
    }

	std::vector<share*> out = BuildMergeAndSortCircuit(shr_srv_set, shr_cli_set, shr_rnd_srv_set, neles, bitlen, bcirc);

    for (size_t i = 0; i < shrsize; i ++) {
        shr_out[i] = bcirc->PutOUTGate(out[i], CLIENT);
    }

    party->ExecCircuit();

    shr_dataset.resize(shrsize);
    for(size_t i = 0; i < shrsize; i ++) {
        shr_dataset[i] = shr_out[i]->get_clear_value<data_t>();
    }

    // delete operation
    delete party;
    free(shr_srv_set);
    free(shr_cli_set);
    free(shr_rnd_srv_set);
    for(size_t i = 0; i < shrsize; i ++) delete shr_out[i];
    free(shr_out);
}

void Client::SelectionProbability()
{
    shr_dataset.insert(shr_dataset.begin(), kA);
    shr_dataset.insert(shr_dataset.end(), kB);

    size_t length(shr_dataset.size());
    shr_gap.resize(length);
    shr_mass.resize(length);

    // compute gaps(share) first
    size_t mpos(length/2); // median position
    for(size_t i = 0; i < mpos-1; i ++) 
        shr_gap[i] = static_cast<int>(shr_dataset[i+1] - shr_dataset[i]);
    shr_gap[mpos-1] = 1;
    for(size_t i = mpos; i < length; i ++)
        shr_gap[i] = static_cast<int>(shr_dataset[i] - shr_dataset[i-1]);

    // compute other utility
    int utility;
    double weight, shift;
    for(size_t i = 0; i < length; i ++) {
        utility = i < mpos ? i - mpos + 1 : mpos - i;
        weight = exp(kEPSILON * utility);
        shift = i > 0 ? shr_mass[i-1] : 0;
        shr_mass[i] = shift + weight * shr_gap[i];
    }

    nonces1.resize(this->m_k);
    for(size_t i = 0; i < nonces1.size(); i ++)
        nonces1[i] = random_range(0, kB-kA);
    nonces2.resize(this->m_k);
    for(size_t i = 0; i < nonces2.size(); i ++)
        nonces2[i] = random_range(0, kB-kA);
}

// void Client::MedianSelection() {
//     uint64_t R = this->generate_R();
//     uint64_t r = this->RandomDraw(R+1, this->nonces1);

//     size_t length = this->shr_dataset.size();
//     size_t bitlen(64);

//     ABYParty* party = new ABYParty(role, address, port, seclevel, bitlen, nthreads, mt_alg, 4000000);
// 	std::vector<Sharing*>& sharings = party->GetSharings();
// 	BooleanCircuit* bcirc = (BooleanCircuit*) sharings[S_BOOL]->GetCircuitBuildRoutine();

//     share *tmp_srv, *tmp_cli;

// /**
//  * @param shr_cmb_dataset share combine dataset
//  * @param shr_cmb_gap share combine gap 
//  * @param shr_cmb_mass share combine mass
//  * @param shr_no share vector of {0, 1, 2, 3, 4, 5, 6...}
//  * @brief correspond to operation 5-7
// */
//     share** shr_cmb_dataset = (share**)malloc(sizeof(share*) * length);
//     for(size_t i = 0; i < length; i ++) {
//         tmp_srv = bcirc->PutDummyINGate(bitlen);
//         tmp_cli = bcirc->PutINGate(this->shr_dataset[i], bitlen, role);
//         shr_cmb_dataset[i] = bcirc->PutADDGate(tmp_srv, tmp_cli);
//         delete tmp_srv, tmp_cli;
//     }

//     share** shr_cmb_gap = (share**)malloc(sizeof(share*) * length);
//     for(size_t i = 0; i < length; i ++) {
//         tmp_srv = bcirc->PutDummyINGate(bitlen);
//         tmp_cli = bcirc->PutINGate((uint64_t)this->shr_gap[i], bitlen, role);
//         shr_cmb_gap[i] = bcirc->PutADDGate(tmp_srv, tmp_cli);
//         delete tmp_srv, tmp_cli;
//     }

//     share** shr_cmb_mass = (share**)malloc(sizeof(share*) * length);
//     for(size_t i = 0; i < length; i++) {
//         tmp_srv = bcirc->PutDummyINGate(bitlen);
//         tmp_cli = bcirc->PutINGate((uint64_t)this->shr_mass[i], bitlen, role);
//         shr_cmb_mass[i] = bcirc->PutADDGate(tmp_srv, tmp_cli);
//         delete tmp_srv, tmp_cli;
//     }

//     share** shr_no = (share**) malloc(sizeof(share*) * length);
//     for(size_t i = 0; i < length; i ++) {
//         shr_no[i] = bcirc->PutINGate(i, bitlen, ALL);
//     }

//     share *shr_r = bcirc->PutCONSGate(r, bitlen);
//     share *shr_zero = bcirc->PutCONSGate((uint64_t)0, bitlen);
//     share *shr_one = bcirc->PutCONSGate((uint64_t)((1 << bitlen) - 1), bitlen);

// /**
//  * @param shr_cond1 represents condition value (r < mass[i])
//  * @brief 
// */
//     share** shr_cond1 = (share**)malloc(sizeof(share*) * length); // r < mass[i]
//     for(size_t i = 0; i < length; i ++) {
//         shr_cond1[i] = bcirc->PutGTGate(shr_r, shr_cmb_mass[i]);
//     }

// /**
//  * @param shr_sel select bits - select which  
// */
//     share *shr_new, *shr_prev, *shr_inv;
//     shr_prev = bcirc->PutINGate((uint64_t)0, (uint32_t)1, ALL);
//     share** shr_sel = (share**) malloc(sizeof(share*) * length);
//     for(size_t i = 0; i < length; i ++) {
//         shr_inv = bcirc->PutINVGate(shr_prev);
//         shr_sel[i] = bcirc->PutANDGate(shr_inv, shr_cond1[i]);
//         shr_new = bcirc->PutORGate(shr_prev, shr_sel[i]);
//         shr_prev = shr_new;
//     }

//     share** shr_mask = (share**) malloc(sizeof(share*) * length);
//     share** shr_cmb_dataset_masked = (share**) malloc(sizeof(share*) * length);
//     share** shr_cmb_gap_masked = (share**) malloc(sizeof(share*) * length);
//     share** shr_no_masked = (share**) malloc(sizeof(share*) * length);
//     for(size_t i = 0; i < length; i ++) {
//         shr_mask[i] = bcirc->PutMUXGate(shr_one, shr_zero, shr_sel[i]);
//         shr_cmb_dataset_masked[i] = bcirc->PutANDGate(shr_mask[i], shr_cmb_dataset[i]);
//         shr_cmb_gap_masked[i] = bcirc->PutANDGate(shr_mask[i], shr_cmb_gap[i]);
//         shr_no_masked[i] = bcirc->PutANDGate(shr_mask[i], shr_no[i]);
//     }

//     share *shr_d = shr_cmb_dataset_masked[0];
//     share *shr_g = shr_cmb_gap_masked[0];
//     share *shr_j = shr_no_masked[0];
//     for(size_t i = 1; i < length; i ++) {
//         shr_d = bcirc->PutADDGate(shr_d, shr_cmb_dataset_masked[i]);
//         shr_g = bcirc->PutADDGate(shr_g, shr_cmb_gap_masked[i]);
//         shr_j = bcirc->PutADDGate(shr_j, shr_no_masked[i]);
//     }
    
//     shr_d = bcirc->PutOUTGate(shr_d, ALL);
//     shr_g = bcirc->PutOUTGate(shr_g, ALL);
//     shr_j = bcirc->PutOUTGate(shr_j, ALL);

//     party->ExecCircuit();

//     uint64_t d = shr_d->get_clear_value<uint64_t>();
//     uint64_t g = shr_g->get_clear_value<uint64_t>();
//     uint64_t j = shr_j->get_clear_value<uint64_t>();

//     uint64_t x = this->RandomDraw(g, nonces2);
//     if(j < length/2 - 1) {
//         return /*d + x*/;
//     }
//     else {
//         return /*d - x*/;
//     }

//     std::cerr << "party execute error" << std::endl;
// }
