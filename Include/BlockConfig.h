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
#include "LinearCode.h"
#include "CodeFilter.h"
#include "BlockRowGenerator.h"

class BlockConfig
{
public:
	// m_StartPhase < m_EndPhase
	unsigned m_StartPhase;
	unsigned m_EndPhase;
	unsigned m_NumOfRows;
	
	ul m_RetrunStat;
	ul m_SearchStat;

	bool m_IsBlockReached;
	bool m_IsResetCodeFilter;

	vector<unsigned> vPartialDistances;

	//Linear code to be stored at the end of phase
	LinearCode *m_C;

	CodeFilter *m_pCodeFilter;

	BlockRowGenerator* m_pBlockRowGenerator;

	Word *m_pCosetLeaders;
	unsigned *m_pCLWTable;

	void Reset() { 
		m_pBlockRowGenerator->Reset();
		if (m_IsResetCodeFilter && m_pCodeFilter != nullptr) m_pCodeFilter->Reset();
	}
};
