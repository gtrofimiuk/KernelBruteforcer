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
#include "VectorHash.h"
#include <HashedArray.h>
#include <set>
#include <vector>

class SingleBufferFilter : public CodeFilter
{
public:
	SingleBufferFilter(unsigned len)
	{
		Len = len;
		pHasher = new VectorHash(Len);
		pBuffer = new unsigned[Len];
	}

	virtual void FillBuffer(LinearCode &C) = 0;

	virtual bool Filter(LinearCode &C)
	{
		FillBuffer(C);

		auto hashedArray = HashedArray(*pHasher, CompressedEnumerator(pBuffer, Len));

		auto &&result = storage.find(hashedArray);
		if (result == storage.end()) {
			pAllocatedPointers.push_back(hashedArray.AllocateCompressedArray());
			storage.insert(hashedArray);

			return true;
		} else
			return false;
	}

	virtual void Reset()
	{
		storage.clear();
		for (unsigned *pElem : pAllocatedPointers) {
			delete[] pElem;
		}
		pAllocatedPointers.clear();
	}

	virtual unsigned GetSize() { return storage.size(); }

protected:
	VectorHash *pHasher;
	set<HashedArray> storage;
	unsigned Len;
	unsigned *pBuffer;
	vector<unsigned *> pAllocatedPointers;
};
