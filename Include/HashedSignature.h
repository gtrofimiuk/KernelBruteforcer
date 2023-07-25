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

#include "HashedArray.h"

class HashedSignature
{
public:
	HashedSignature(){};
	HashedSignature(unsigned OverallHash, HashedArray *hashedSigns, unsigned N)
	: overallHash(OverallHash)
	, n(N)
	, pHashedSigns(hashedSigns){};
	unsigned overallHash;
	HashedArray *pHashedSigns;
	unsigned n;

	friend bool operator==(const HashedSignature &a, const HashedSignature &b)
	{
		if (a.overallHash != b.overallHash) return false;
		bool isEqual = true;
		for (unsigned i = 0; i < a.n; ++i) {
			if (a.pHashedSigns[i] != b.pHashedSigns[i]) {
				isEqual = false;
				break;
			}
		}
		return isEqual;
	}

	friend bool operator<(const HashedSignature &a, const HashedSignature &b)
	{
		if (a.overallHash < b.overallHash) return true;
		if (a.overallHash > b.overallHash) return false;

		return std::lexicographical_compare(a.pHashedSigns, a.pHashedSigns + a.n, b.pHashedSigns, b.pHashedSigns + b.n);
	}
};
