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

class CompleteCodeFilter : public CodeFilter
{
public:
	CompleteCodeFilter(unsigned n)
	{
		N = n;
		Len = (N + 1) * 2;
		pHasher = new VectorHash(Len);
		pHashedArrays = new HashedArray[N];
		pSpectrumBuffer = new unsigned[Len];
		pHashBuffer = new unsigned[N];
	}
	virtual bool Filter(LinearCode &C)
	{
		C.ComputeSignature();
		C.ComputeCLWD();

		unsigned *spectrum = C.spectrum;
		unsigned *pCLWD = C.pCLWD;

		copy_n(spectrum, C.N + 1, pSpectrumBuffer);
		copy_n(pCLWD, C.N + 1, pSpectrumBuffer + C.N + 1);

		auto hashedArray = HashedArray(*pHasher, CompressedEnumerator(pSpectrumBuffer, Len));

		for (unsigned i = 0; i < N; ++i) {
			pHashedArrays[i] = HashedArray(*pHasher, CompressedEnumerator(C.ppSignatureArray[i], N + 1));
		}

		std::sort(pHashedArrays, pHashedArrays + N);

		for (unsigned i = 0; i < N; ++i) {
			pHashBuffer[i] = pHashedArrays[i].hash;
		}

		auto hashedSignature = HashedSignature(pHasher->Hash(pHashBuffer, N), pHashedArrays, N);

		auto &&elem = make_pair(hashedArray, hashedSignature);
		auto &&result = Storage.find(elem);
		if (result == Storage.end()) {
			pAllocatedPointers.push_back(elem.first.AllocateCompressedArray());

			for (unsigned i = 0; i < N; ++i) {
				pAllocatedPointers.push_back(elem.second.pHashedSigns[i].AllocateCompressedArray());
			}

			Storage.insert(elem);

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
	unsigned N;
	unsigned Len;
	VectorHash *pHasher;
	HashedArray *pHashedArrays;
	unsigned *pHashBuffer;
	unsigned *pSpectrumBuffer;
	set<pair<HashedArray, HashedSignature>> Storage;
	vector<unsigned *> pAllocatedPointers;
};
