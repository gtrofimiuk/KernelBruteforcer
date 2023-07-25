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
#include <map>
#include <functional>

class CLWTBlockRowGenerator : public BlockRowGenerator
{
public:
	virtual void Init(BlockConfig *pBlockConfig){ 
		m_pBlockConfig = pBlockConfig;
		m_NumOfRows = m_pBlockConfig->m_NumOfRows;
		for (unsigned i = 0; i < m_NumOfRows; ++i)
			m_SyndromeStorage[pBlockConfig->vPartialDistances[i]] = vector<unsigned>();
		m_pWordBuffer = new Word[1u << m_NumOfRows];
		m_pCounters = new unsigned[m_NumOfRows];
		fill_n(m_pCounters, m_NumOfRows, 0);

		m_VectorStorage.resize(m_NumOfRows);
		m_pCurrentSyndromes = new unsigned[m_NumOfRows];
		for (unsigned i = 0; i < m_NumOfRows; ++i) {
			m_VectorStorage[i] = &m_SyndromeStorage[pBlockConfig->vPartialDistances[i]];
		}
	};

	virtual void UpdateGenerator(unsigned *pCLWTable = nullptr, Word *pCosetLeaders = nullptr)
	{
		Reset();
		m_pCLWTable = pCLWTable;
		m_pCosetLeaders = pCosetLeaders;
		
		unsigned lim = 1u << (m_pBlockConfig->m_EndPhase+1);
		for (auto &&elem : m_SyndromeStorage) {
			unsigned w = elem.first;
			auto &&v = elem.second;
			for (unsigned i = 0; i < lim; ++i) {
				if (m_pCLWTable[i] == w) {
					v.push_back(i);
				}
			}
		}
	
	}

	void SearchLoop()
	{
		if (m_CurrentRow < 0) return;

		if (m_pCounters[m_CurrentRow] >= m_VectorStorage[m_CurrentRow]->size()) {
			m_pCounters[m_CurrentRow] = 0;
			m_CurrentRow--;

			return;
		}

		m_pCurrentSyndromes[m_CurrentRow] = (* m_VectorStorage[m_CurrentRow])[m_pCounters[m_CurrentRow]++];
		m_pWordBuffer[1u << m_CurrentRow] = m_pCurrentSyndromes[m_CurrentRow];

		bool res = CheckPD(m_CurrentRow, m_pBlockConfig->vPartialDistances[m_CurrentRow]);
		if (res) {
			m_CurrentRow++;
			return;
		}
	}

	virtual bool GetNextBlock(Word *pReturnedRows)
	{
		while (m_CurrentRow >= 0 && m_CurrentRow < m_NumOfRows) {
			SearchLoop();
			CheckTimeLimit();
		}

		if (m_CurrentRow == m_NumOfRows) {
			for (unsigned i = 0; i < m_NumOfRows; ++i) {
				pReturnedRows[i] = m_pCosetLeaders[m_pCurrentSyndromes[i]];
			}
			m_CurrentRow--;
			return true;
		}

		return false;
	}

	virtual void Reset()
	{
		m_CurrentRow = 0;
		fill_n(m_pCounters, m_NumOfRows, 0);
		fill_n(m_pCurrentSyndromes, m_NumOfRows, 0);
		for (auto &&elem : m_SyndromeStorage) {
			elem.second.clear();
		}
	}

private:
	std::map<unsigned, vector<unsigned>> m_SyndromeStorage;
	vector<vector<unsigned>*> m_VectorStorage;
	
	unsigned* m_pCounters;
	int m_CurrentRow;
	int m_NumOfRows;
	unsigned *m_pCurrentSyndromes;
	Word *m_pWordBuffer;

	unsigned *m_pCLWTable;
	Word *m_pCosetLeaders;

	bool CheckPD(int CurrentRow, int RequiredPD)
	{
		if (CurrentRow == 0) return true;
		int n = 1u << CurrentRow;
		bool isSucceed = true;
		for (int i = 1; i < (1u << CurrentRow); ++i) {
			Word &res = m_pWordBuffer[i + n];
			res = m_pWordBuffer[i] ^ m_pWordBuffer[n];
			unsigned weight = m_pCLWTable[res];  //_mm_popcnt_u32(res);
			if (weight < RequiredPD) {
				isSucceed = false;
				break;
			}
		}

		return isSucceed;
	}
};
