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
	e_sharing sharing,
	uint32_t money
) 
{

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg);

	std::vector<Sharing*>& sharings = party->GetSharings();

	// Circuit* arithcirc = sharings[sharing]->GetCircuitBuildRoutine();
	ArithmeticCircuit* arithcirc = static_cast<ArithmeticCircuit*>(sharings[S_ARITH]->GetCircuitBuildRoutine());
	BooleanCircuit* boolcirc = static_cast<BooleanCircuit*>(sharings[S_BOOL]->GetCircuitBuildRoutine());

	share *s_alice_money, *s_bob_money, *s_out;
	uint32_t output;

	if(role == SERVER) {
		s_alice_money = arithcirc->PutDummyINGate(bitlen);
		s_bob_money = arithcirc->PutINGate(money, bitlen, SERVER);
	} else { //role == CLIENT
		s_alice_money = arithcirc->PutDummyINGate(bitlen);
		s_bob_money = arithcirc->PutINGate(money, bitlen, CLIENT);
	}

	s_out = arithcirc->PutADDGate(s_alice_money, s_bob_money);
	// s_out = boolcirc->PutGTGate(s_alice_money, s_bob_money);

	s_out = arithcirc->PutOUTGate(s_out, ALL);

	party->ExecCircuit();

	output = s_out->get_clear_value<uint32_t>();

	std::cout << "Testing Millionaire's Problem in " << get_sharing_name(sharing)
				<< " sharing: " << std::endl;
	std::cout << "\nMy Money:\t" << money;
	std::cout << "\nCircuit Result:\t" << output << std::endl;
	
	delete party;
	return 0;
}
