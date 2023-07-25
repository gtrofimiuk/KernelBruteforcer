/*------------------------------------------------------------------------
Copyright 2023 Grigorii Trofimiuk

Licensed under the Apache License,
Version 2.0(the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
--------------------------------------------------------------------------*/

#pragma once
#include "BinomialTools.h"
#include <immintrin.h>

class WeightedRowGenerator
{
public:
	WeightedRowGenerator(){};
	WeightedRowGenerator(unsigned _weight, unsigned _N)
	: weight(_weight)
	, N(_N)
	{
		limit = Binomial(N, weight);
		currentWord = (weight == 32)?~0:((1 << weight) - 1);
		counter = 0;
	}

	// bit hack from https://graphics.stanford.edu/~seander/bithacks.html#NextBitPermutation

	Word GetNextRow()
	{
		_ASSERT(counter < limit);

		Word v = currentWord;
		_ASSERT(_mm_popcnt_u32(v) == weight);

		//ul t = v | (v - 1);
		//currentWord = (t + 1) | (((~t & -~t) - 1) >> (__lzcnt64(v) + 1)); TODO: find correct function
		//tzcnt u64
		Word t = (v | (v - (unsigned)1)) + (unsigned)1;  
		currentWord = t | ((((t & -t) / (v & -v)) >> (unsigned)1) - (unsigned)1);  
		counter++;

		return v;
	}
	void Reset()
	{
		currentWord = (weight == 32) ? ~0 : ((1 << weight) - 1);
		counter = 0;
	}
	bool IsNextWordAvailable() { return counter < limit; }

private:
	Word currentWord;
	unsigned counter;
	unsigned weight;
	unsigned N;
	unsigned limit;
};