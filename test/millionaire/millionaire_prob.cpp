/**
 \file 		millionaire_prob.cpp
 \author 	sreeram.sadasivam@cased.de
 \copyright	ABY - A Framework for Efficient Mixed-protocol Secure Two-party Computation
			Copyright (C) 2019 Engineering Cryptographic Protocols Group, TU Darmstadt
			This program is free software: you can redistribute it and/or modify
            it under the terms of the GNU Lesser General Public License as published
            by the Free Software Foundation, either version 3 of the License, or
            (at your option) any later version.
            ABY is distributed in the hope that it will be useful,
            but WITHOUT ANY WARRANTY; without even the implied warranty of
            MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
            GNU Lesser General Public License for more details.
            You should have received a copy of the GNU Lesser General Public License
            along with this program. If not, see <http://www.gnu.org/licenses/>.
 \brief		Implementation of the millionaire problem using ABY Framework.
 */

#define TEST_LINE \
    //std::cout << "No." << times << " test: " << line_no++ << std::endl;

#include "millionaire_prob.h"
#include <abycore/circuit/booleancircuits.h>
#include <abycore/circuit/arithmeticcircuits.h>
#include <abycore/sharing/sharing.h>
#include <abycore/aby/abyparty.h>

int32_t test_millionaire_prob_circuit(
	e_role role, 
	const std::string& address, 
	uint16_t port, 
	seclvl seclvl,
	uint32_t bitlen, 
	uint32_t nthreads, 
	e_mt_gen_alg mt_alg, 
	e_sharing sharing
) 
{

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg);

	std::vector<Sharing*>& sharings = party->GetSharings();

	// Circuit* arithcirc = sharings[sharing]->GetCircuitBuildRoutine();
	BooleanCircuit* bcirc = static_cast<BooleanCircuit*>(sharings[S_BOOL]->GetCircuitBuildRoutine());

	// share* shr_a, *shr_b, *shr_c;
	// // uint32_t a = 1000, b = 100, c=999;
	// uint32_t a = 0b11, b = 0b1;
	// if(role == SERVER) {
	// 	// shr_a = bcirc->PutSIMDINGate(bitlen, a, 1, SERVER);
	// 	// shr_b = bcirc->PutSIMDINGate(bitlen, b, 1, SERVER);
	// 	// shr_b = bcirc->PutDummySIMDINGate(bitlen, 1);
	// 	shr_a = bcirc->PutINGate(a, bitlen, SERVER);
	// 	shr_b = bcirc->PutDummyINGate(bitlen);
	// }
	// else {
	// 	shr_a = bcirc->PutDummyINGate(bitlen);
	// 	shr_b = bcirc->PutINGate(b, bitlen, CLIENT);
	// 	// shr_a = bcirc->PutDummySIMDINGate(bitlen, 1);
	// 	// shr_b = bcirc->PutDummySIMDINGate(bitlen, 1);
	// 	// shr_b = bcirc->PutSIMDINGate(bitlen, b, 1, CLIENT);
	// }

	// share* shr_out;
	// shr_out = bcirc->PutADDGate(shr_a, shr_b);
	// shr_out = bcirc->PutINVGate(shr_out);
	// shr_out = bcirc->PutOUTGate(shr_out, CLIENT);

	// share* shr_one = bcirc->PutINGate((uint32_t)1, bitlen, ALL);
	share* shr_one;
	if(role == SERVER) {
		shr_one = bcirc->PutINGate(uint32_t(1), 1, SERVER);
	}
	else {
		shr_one = bcirc->PutDummyINGate(1);
	}
	share* shr_out = bcirc->PutINVGate(shr_one);
	shr_out = bcirc->PutOUTGate(shr_out, CLIENT);

	party->ExecCircuit();
	std::cout << "success" << std::endl;

	// std::cout << shr_a->get_wires().size() << std::endl;

	if(role == CLIENT) {
		uint32_t output = shr_out->get_clear_value<uint32_t>();
		std::cout << output << std::endl;
	}

	// std::cout << "Testing Millionaire's Problem in " << get_sharing_name(sharing)
	// 			<< " sharing: " << std::endl;
	// std::cout << "\nMy Money:\t" << money;
	// std::cout << "\nCircuit Result:\t" << output << std::endl;
	
	delete party;
	return 0;
}
