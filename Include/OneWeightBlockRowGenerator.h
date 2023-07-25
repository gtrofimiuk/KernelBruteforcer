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
#include "BlockRowGenerator.h"
#include "WeightedRowGenerator.h"
#include <functional>
#include <map>

using namespace std;

class OneWeightBlockRowGenerator : public BlockRowGenerator
{
public:
	OneWeightBlockRowGenerator(unsigned NumOfRows)
	: NUM_ROWS(NumOfRows)
	{
	}
	virtual void Init(BlockConfig *pBlockConfig)
	{
		m_pBlockConfig = pBlockConfig;
		_ASSERT(m_pBlockConfig->m_NumOfRows == NUM_ROWS);
		m_Distance = m_pBlockConfig->vPartialDistances[0];

		m_pCounters = new unsigned[NUM_ROWS];
	};

	virtual void UpdateGenerator(unsigned *pCLWTable = nullptr, Word *pCosetLeaders = nullptr)
	{
		Reset();
		m_pCLWTable = pCLWTable;
		m_pCosetLeaders = pCosetLeaders;

		unsigned lim = 1u << (m_pBlockConfig->m_EndPhase + 1);
		for (unsigned i = 0; i < lim; ++i) {
			if (m_pCLWTable[i] == m_Distance) {
				m_VectorStorage.push_back(i);
			}
		}

		m_CurrentLim = m_VectorStorage.size();
	}

	virtual bool GetNextBlock(Word *pReturnedRows) = 0;

	virtual void Reset()
	{
		for (unsigned i = 0; i < NUM_ROWS; ++i) m_pCounters[i] = i;
		m_VectorStorage.clear();
	}

protected:
	const unsigned NUM_ROWS;

	vector<unsigned> m_VectorStorage;
	unsigned m_Distance;
	unsigned *m_pCounters;

	unsigned *m_pCLWTable;
	Word *m_pCosetLeaders;

	unsigned m_CurrentLim;

	bool isCosetUnsuitable(unsigned currSyndrome, unsigned rowSyndrome)
	{
		unsigned w = m_pCLWTable[currSyndrome];
		return w < m_Distance || (w == m_Distance && currSyndrome < rowSyndrome);
	}
};
