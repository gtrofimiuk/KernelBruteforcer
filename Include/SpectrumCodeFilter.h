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
#include <map>
#include <vector>

class SpectrumCodeFilter : public CodeFilter
{
public:
	SpectrumCodeFilter(unsigned _Len, VectorHash *_hasher)
	{
		Len = _Len;
		hasher = _hasher;
	}
	virtual bool Filter(LinearCode &C)
	{
		assert(C.N + 1 == Len);
		assert(_CrtCheckMemory());

		unsigned *spectrum = C.spectrum;

		unsigned Hash = hasher->Hash(C.spectrum,Len);
		auto &&result = m_Storage.find(Hash);
		if (result == m_Storage.end()) {
			unsigned *newSpectrum = new unsigned[Len];
			memcpy(newSpectrum, C.spectrum, sizeof(unsigned) * Len);

			_ASSERT(newSpectrum[0] == 1);

			m_Storage[Hash] = std::vector<unsigned *>();
			m_Storage[Hash].push_back(newSpectrum);

			return true;
		} else {
			std::vector<unsigned *> &vec = result->second;
			bool isEqual = false;
			for (unsigned *Array : vec) {
				isEqual = isArraysEqual(spectrum, Array);
				if (isEqual == true) break;
			}
			if (isEqual == false) {
				unsigned *newSpectrum = new unsigned[Len];
				memcpy(newSpectrum, C.spectrum, sizeof(unsigned) * Len);
				std::cout << "Hash collizion" << std::endl;

				_ASSERT(newSpectrum[0] == 1);

				result->second.push_back(newSpectrum);
				return true;
			} else {
				return false;
			}
		}
	}
	virtual void Reset()
	{
		for (auto &&elem : m_Storage) {
			for (unsigned *pointer : elem.second) {
				delete[] pointer;
			}
			elem.second.clear();
		}
		m_Storage.clear();
	}

	virtual unsigned GetSize() { return m_Storage.size(); }

private:
	VectorHash *hasher;
	unsigned Len;
	std::map<unsigned, std::vector<unsigned *>> m_Storage;

	bool isArraysEqual(unsigned *Arr1, unsigned *Arr2)
	{
		bool isEqual = true;
		for (unsigned i = 0; i < Len; ++i) {
			if (Arr1[i] != Arr2[i]) {
				isEqual = false;
				break;
			}
		}

		return isEqual;
	}
};