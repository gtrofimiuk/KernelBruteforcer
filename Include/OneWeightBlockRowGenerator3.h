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
#include "OneWeightBlockRowGenerator.h"

using namespace std;

class OneWeightBlockRowGenerator3 : public OneWeightBlockRowGenerator
{
public:
	OneWeightBlockRowGenerator3()
	: OneWeightBlockRowGenerator(3){};

	virtual bool GetNextBlock(Word *pReturnedRows)
	{
		bool isRowsFound = false;

		for (unsigned i0 = m_pCounters[0]; i0 < m_CurrentLim; ++i0) {
			unsigned syndrome0 = m_VectorStorage[i0];

			for (unsigned i1 = m_pCounters[1]; i1 < m_CurrentLim; ++i1) {
				unsigned syndrome1 = m_VectorStorage[i1];
				unsigned syndromeXor01 = syndrome0 ^ syndrome1;

				if (isCosetUnsuitable(syndromeXor01, syndrome1)) continue;

				for (unsigned i2 = m_pCounters[2]; i2 < m_CurrentLim; ++i2) {
					unsigned syndrome2 = m_VectorStorage[i2];

					unsigned syndromeXor012 = syndromeXor01 ^ syndrome2;
					if (syndromeXor012 == 0) continue;

					if (isCosetUnsuitable(syndrome0 ^ syndrome2, syndrome2)) continue;
					if (isCosetUnsuitable(syndrome1 ^ syndrome2, syndrome2)) continue;
					if (isCosetUnsuitable(syndromeXor012, syndrome2)) continue;

					isRowsFound = true;
					m_pCounters[0] = i0, m_pCounters[1] = i1, m_pCounters[2] = i2 + 1;
					pReturnedRows[0] = m_pCosetLeaders[syndrome0], pReturnedRows[1] = m_pCosetLeaders[syndrome1],
					pReturnedRows[2] = m_pCosetLeaders[syndrome2];
					break;
				}

				if (isRowsFound) break;

				m_pCounters[2] = i1 + 2;
				CheckTimeLimit();
			}
			if (isRowsFound) break;

			m_pCounters[1] = i0 + 2;
			m_pCounters[2] = m_pCounters[1] + 1;

			CheckTimeLimit();
		}

		return isRowsFound;
	}
};
