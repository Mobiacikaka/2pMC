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

	Circuit* circ = sharings[sharing]->GetCircuitBuildRoutine();

	//for(int times = 0; times < 5; times ++)
	//{
		share *s_alice_money, *s_bob_money, *s_out;
		uint32_t alice_money, bob_money, output;
		srand(time(NULL));
		alice_money = rand();
		bob_money = rand();
	
		if(role == SERVER) {
			s_alice_money = circ->PutDummyINGate(bitlen);
			s_bob_money = circ->PutINGate(bob_money, bitlen, SERVER);
		} else { //role == CLIENT
			s_alice_money = circ->PutINGate(alice_money, bitlen, CLIENT);
			s_bob_money = circ->PutDummyINGate(bitlen);
		}
	
		//s_out = BuildMillionaireProbCircuit(s_alice_money, s_bob_money,
		//		(BooleanCircuit*) circ);
		s_out = ((BooleanCircuit*) circ)->PutGTGate(s_alice_money, s_bob_money);
	
		s_out = circ->PutOUTGate(s_out, ALL);
	
		party->ExecCircuit();
	
		output = s_out->get_clear_value<uint32_t>();
	
		std::cout << "Testing Millionaire's Problem in " << get_sharing_name(sharing)
					<< " sharing: " << std::endl;
		std::cout << "\nAlice Money:\t" << alice_money;
		std::cout << "\nBob Money:\t" << bob_money;
		std::cout << "\nCircuit Result:\t" << (output ? ALICE : BOB);
		std::cout << "\nVerify Result: \t" << ((alice_money > bob_money) ? ALICE : BOB)
					<< "\n";
	//}
	
	delete party;
	return 0;
}

share* BuildMillionaireProbCircuit(share *s_alice, share *s_bob,
		BooleanCircuit *bc) {

	share* out;

	/** Calling the greater than equal function in the Boolean circuit class.*/
	//out = bc->PutEQGate(s_alice, s_bob);
	out = bc->PutGTGate(s_alice, s_bob);

	return out;
}

