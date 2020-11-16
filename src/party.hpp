#pragma once

#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <abycore/aby/abyparty.h>
#include "dataset.hpp"

/**
 * \warning suppose A is SERVER and B is CLIENT
 */ 

template<class T>
class Party {
private:
    DataSet<T> data_set;

    e_role role;

    void Prune(size_t s, size_t k);

public:
    Party();

    ~Party();

};

template<class T>
void Party<T>::Prune(size_t s, size_t k)
{
    T p;
    
	if(this->role == SERVER) p = std::numeric_limits<T>::max();
	else if(this->role == CLIENT) p = std::numeric_limits<T>::min();
	else assert(0);

	this->data_set.Pad(k, p);
	
	for(size_t i = 0; i < s; i ++)
	{
		T median = this->data_set.GetMedian();

		// use garbled circuit to compute c
		bool c(flase);
		assert(0);

		if((this->role == SERVER && c == 1)&&(this->role == CLIENT && c == 0)) 
		{
			// retain only the upper half
			this->data_set.KeepUpperHalf();
		}
		else
		{
			// retain only the lower half
			this->data_set.KeepLowerHalf();
		}
		
	}
}
