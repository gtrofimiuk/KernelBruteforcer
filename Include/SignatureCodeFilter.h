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

#include "CodeFilter.h"
#include "HashedSignature.h"
#include "VectorHash.h"
#include <algorithm>
#include <map>
#include <set>
#include <vector>

using namespace std;

class SignatureCodeFilter : public CodeFilter
{
public:
	SignatureCodeFilter(unsigned N)
	{
		n = N;
		pHasher = new VectorHash(N+1);
		pHashedArrays = new HashedArray[N];
		pHashBuffer = new unsigned[N];
	}
	virtual bool Filter(LinearCode &C)
	{
		C.ComputeSignature();

		for (unsigned i = 0; i < n; ++i) {
			pHashedArrays[i] = HashedArray(*pHasher, CompressedEnumerator(C.ppSignatureArray[i], n + 1));
		}

		std::sort(pHashedArrays, pHashedArrays + n);

		for (unsigned i = 0; i < n; ++i) {
			pHashBuffer[i] = pHashedArrays[i].hash;
		}

		auto hashedSignature = HashedSignature(pHasher->Hash(pHashBuffer, n), pHashedArrays, n);

		auto &&result = Storage.find(hashedSignature);
		if (result == Storage.end()) {

			for (unsigned i = 0; i < n; ++i) {
				pAllocatedPointers.push_back(hashedSignature.pHashedSigns[i].AllocateCompressedArray());
			}

			Storage.insert(hashedSignature);

			return true;
		} else
			return false;
	}
	virtual void Reset()
	{
		Storage.clear();
		for (unsigned *pElem : pAllocatedPointers) {
			delete[] pElem;
		}
		pAllocatedPointers.clear();
	}

	virtual unsigned GetSize() { return Storage.size(); }

private:
	unsigned n;
	VectorHash *pHasher;
	HashedArray *pHashedArrays;
	unsigned *pHashBuffer;
	set<HashedSignature> Storage;
	vector<unsigned *> pAllocatedPointers;
};
