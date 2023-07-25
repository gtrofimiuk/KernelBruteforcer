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

class StartBlockRowGenerator : public BlockRowGenerator {
public:
	virtual void Init(BlockConfig* pBlockConfig) {
		m_pBlockConfig = pBlockConfig;
		m_NumOfRows = m_pBlockConfig->m_NumOfRows;
		m_pWordBuffer = new Word[1u << m_NumOfRows];
		m_pWordBuffer[0] = 0;
		m_pCurrentRows = new Word[m_NumOfRows];
		vRowGenerators.resize(m_NumOfRows);
		for (unsigned i = 0; i < m_NumOfRows; ++i) {
			vRowGenerators[i] = WeightedRowGenerator(m_pBlockConfig->vPartialDistances[i], m_pBlockConfig->m_C->N);
		}
	}

	virtual void UpdateGenerator(unsigned *pCLWTable = nullptr, Word *pCosetLeaders = nullptr) {
		Reset();
	}

	void SearchLoop()
	{
		if (m_CurrentRow < 0) return;
		
		if (!vRowGenerators[m_CurrentRow].IsNextWordAvailable()) {
			vRowGenerators[m_CurrentRow].Reset();
			m_CurrentRow--;

			return;
		}

		m_pCurrentRows[m_CurrentRow] = vRowGenerators[m_CurrentRow].GetNextRow();
		m_pWordBuffer[1u << m_CurrentRow] = m_pCurrentRows[m_CurrentRow];
		bool res = CheckPD(m_CurrentRow, m_pBlockConfig->vPartialDistances[m_CurrentRow]);
		if (res) {
			m_CurrentRow++;
			return;
		}
	}

	virtual bool GetNextBlock(Word* pReturnedRows) {
		while (m_CurrentRow >= 0 && m_CurrentRow < m_NumOfRows) {
			SearchLoop();
			CheckTimeLimit();
		}
		
		if (m_CurrentRow == m_NumOfRows) {
			copy_n(m_pCurrentRows, m_NumOfRows, pReturnedRows);
			m_CurrentRow--;  // m_CurrentRow = 0; //TODO: change this parameter via command line
			return true;
		}

		return false;
	}

	virtual void Reset() {
		m_CurrentRow = 0;
		for (auto &&elem : vRowGenerators) elem.Reset();
		
		fill_n(m_pCurrentRows, m_NumOfRows, 0);
	}

private:
	vector<WeightedRowGenerator> vRowGenerators;
	int m_CurrentRow;
	int m_NumOfRows;
	Word* m_pCurrentRows;
	Word *m_pWordBuffer;

	bool CheckPD(int CurrentRow, int RequiredPD)
	{
		if (CurrentRow == 0) return true;
		int n = 1u << CurrentRow;
		bool isSucceed = true;
		for (int i = 1; i < (1u << CurrentRow); ++i) {
			Word &res = m_pWordBuffer[i + n];
			res = m_pWordBuffer[i] ^ m_pWordBuffer[n];
			unsigned weight = _mm_popcnt_u32(res);
			if (weight < RequiredPD) {
				isSucceed = false;
				break;
			}
		}
		return isSucceed;
	}
};
