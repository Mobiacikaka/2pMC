#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>
#include <abycore/aby/abyparty.h>
#include "party.hpp"

int main(int argc, char** argv) {
   
    e_role role;
    uint32_t bitlen = 32, nvals = 31, secparam = 128, nthreads = 1;
    uint16_t port = 7766;
    std::string address = "127.0.0.1";
    int32_t test_op = -1;
    e_mt_gen_alg mt_alg = MT_OT;

    ReadTestOptions(&argc, &argv, &role, &bitlen, &nvals, &secparam, &address, &port, &test_op);

    seclvl seclvl = get_sec_lvl(secparam);


}
