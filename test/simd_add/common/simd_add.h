#ifndef __MERGE_AND_SORT_
#define __MERGE_AND_SORT_

#include <abycore/ABY_utils/ABYconstants.h>

int32_t test_simd_add(e_role role, const std::string& address, uint16_t port, 
        seclvl seclevel, uint32_t neles, uint32_t bitlen, uint32_t nthreads, 
        e_mt_gen_alg mt_alg, uint32_t prot_version, bool verify);

#endif
