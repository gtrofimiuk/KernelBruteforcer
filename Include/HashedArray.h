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

#include <VectorHash.h>
#include <algorithm>

// Stores only nonzero elements from N-length array with nonzero indices at mask,
class CompressedEnumerator
{
public:
	unsigned *pEnumerator;
	uint64_t mask;
	// numer of nonzero elements from original array
	unsigned s;
	CompressedEnumerator()
	: pEnumerator(nullptr)
	, mask(0)
	, s(0)
	{
	}
	CompressedEnumerator(unsigned *pInputArray, unsigned N)
	{
		mask = 0, s = 0, pEnumerator = pInputArray;
		for (unsigned i = 0; i < N; ++i) {
			if (pEnumerator[i] != 0) s++;
		}
		unsigned I = 0;
		for (unsigned i = 0; i < N; ++i) {
			if (pEnumerator[i] != 0) {
				pEnumerator[I++] = pEnumerator[i];
				mask |= (uint64_t)1 << (uint64_t)i;
			}
		}
		_ASSERT(s == I);
	}

	CompressedEnumerator(unsigned *pCompressedArray, uint64_t Mask, unsigned S)
	: pEnumerator(pCompressedArray)
	, mask(Mask)
	, s(S)
	{
	}
	~CompressedEnumerator(){};
};

class HashedArray : public CompressedEnumerator
{
public:
	HashedArray(){};
	HashedArray(unsigned Hash, CompressedEnumerator &&_compressedEnumerator)
	: hash(Hash)
	, CompressedEnumerator(_compressedEnumerator)
	{
	}
	HashedArray(VectorHash &hasher, CompressedEnumerator &&_compressedEnumerator)
	: CompressedEnumerator(_compressedEnumerator)
	{
		hash = hasher.Hash(pEnumerator, s);
	}
	HashedArray(unsigned Hash, unsigned *pCompressedArray, uint64_t Mask, unsigned S)
	: hash(Hash)
	, CompressedEnumerator(pCompressedArray, Mask, S)
	{
	}

	unsigned hash;

	unsigned *AllocateCompressedArray()
	{
		unsigned *newArray = new unsigned[s];
		copy_n(pEnumerator, s, newArray);
		pEnumerator = newArray;
		return newArray;
	}

	friend bool operator==(const HashedArray &a, const HashedArray &b)
	{
		if (a.hash != b.hash) return false;
		if (a.mask != b.mask) return false;
		bool isEqual = true;
		for (unsigned i = 0; i < a.s; ++i) {
			if (a.pEnumerator[i] != b.pEnumerator[i]) {
				isEqual = false;
				break;
			}
		}
		return isEqual;
	}
	friend bool operator!=(const HashedArray &a, const HashedArray &b) { return !(a == b); }
	friend bool operator<(const HashedArray &a, const HashedArray &b)
	{
		if (a.hash < b.hash) return true;
		if (a.hash > b.hash) return false;

		if (a.mask < b.mask) return true;
		if (a.mask > b.mask) return false;

		return std::lexicographical_compare(a.pEnumerator, a.pEnumerator + a.s, b.pEnumerator, b.pEnumerator + b.s);
	}
};
