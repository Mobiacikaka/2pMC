#include "party.hpp"

Party::Party() {
    data_set.Init();
}

Party::~Party() {

}

void Party::SetParameters(e_role role, std::string address, uint16_t port, seclvl seclevel, uint32_t bitlen, uint32_t nthreads, e_mt_gen_alg mt_alg)
{
	this->role = role;
	this->address = address;
	this->port = port;
	this->seclevel = seclevel;
	this->bitlen = bitlen;
	this->nthreads = nthreads;
	this->mt_alg = mt_alg;
}

void Party::Run() {
    this->Prune();
    std::cout << "Pruning Finished Successfully!" << std::endl;

	this->data_set.PrintAllElement();

    this->MergeAndShare();
    std::cout << "Merge and Share Finished Successfully!" << std::endl;

	this->SelectionProbability();
	std::cout << "Selection Probability Finished Successfully!" << std::endl;

	std::cout << std::endl << "Share Dataset" << std::endl;
	PrintElements(this->shr_dataset);
	std::cout << std::endl << "Share Gap" << std::endl;
	PrintElements(this->shr_gap);
	std::cout << std::endl << "Share Mass" << std::endl;
	PrintElements(this->shr_mass);
}

std::vector<uint32_t> Party::PutVectorCondSwapGate(uint32_t a, uint32_t b, uint32_t s, BooleanCircuit* circ) {
	std::vector<uint32_t> avec(1, a);
	std::vector<uint32_t> bvec(1, b);
	std::vector<uint32_t> out(2);
	//uint32_t svec = circ->PutRepeaterGate(s, 32);
	std::vector<std::vector<uint32_t>> temp = circ->PutCondSwapGate(avec, bvec, s, true);
	out[0] = temp[0][0];
	out[1] = temp[1][0];
	return out;
}

std::vector<uint32_t> Party::PutVectorBitonicSortGate(share** srv_set, share** cli_set, uint32_t neles, uint32_t bitlen, BooleanCircuit* circ) {

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

std::vector<share*>
Party::BuildMergeAndSortCircuit(share** srv_set, share** cli_set, share** shr_rnd_srv_set, uint32_t neles, uint32_t bitlen, BooleanCircuit* circ)
{
    std::vector<uint32_t> merge_out = PutVectorBitonicSortGate(srv_set, cli_set, neles, bitlen, circ);

    std::vector<share*> sub_wire(2 * neles);
    for(size_t i = 0; i < 2 * neles; i ++) {
        boolshare tmp_share(1, circ);
        tmp_share.set_wire_id(0, merge_out[i]);
        sub_wire[i] = circ->PutSUBGate(&tmp_share, shr_rnd_srv_set[i]);
    }

    return sub_wire;
}
