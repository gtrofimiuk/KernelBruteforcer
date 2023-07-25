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

#include <map>
#include <sstream>
#include <string>

#include "BlockConfigurator.h"

#include "ListOfBlockRowGenerators.h"
using namespace std;

BlockConfigurator::BlockConfigurator(istream &config, unsigned blockSize, unsigned CLWTbound, unsigned maxBlockR, std::string filter, bool isUseMixPD)
{
	config >> N;
	m_PD = new unsigned[N];

	unsigned I = 0;
	while (config.peek() != EOF && I < N) {
		config >> m_PD[I++];
	}

	if (I != N) throw Exception("Invalid number of partial distances");

	vector<unsigned> vBlockPhasesReverse;

	int curPhase = 0;
	int curBlock = 0;
	while (curPhase < N) {
		if (curPhase == N - 1 - CLWTbound) {
			vBlockPhasesReverse.push_back(curPhase);
			break;
		}
		curBlock++;
		if (curBlock == blockSize || (curPhase != N - 1 && m_PD[curPhase] != m_PD[curPhase + 1] && !isUseMixPD) ){
		   // || curPhase >= maxBlockR) {
			vBlockPhasesReverse.push_back(curPhase);
			curBlock = 0;
		}

		curPhase++;
	}

	if (*vBlockPhasesReverse.rbegin() != N - 1) vBlockPhasesReverse.push_back(N - 1);

	m_NumOfBlocks = vBlockPhasesReverse.size();
	vBlockConfigs = vector<BlockConfig>(m_NumOfBlocks);

	unsigned curStart = 0, curEnd = N - 1;
	for (unsigned i = 0; i < m_NumOfBlocks; ++i) {
		curEnd = vBlockPhasesReverse[i];
		BlockConfig &CurrentConfig = vBlockConfigs[m_NumOfBlocks - i - 1];

		CurrentConfig.m_StartPhase = curStart;
		CurrentConfig.m_EndPhase = curEnd;
		CurrentConfig.m_NumOfRows = curEnd - curStart + 1;

		CurrentConfig.m_C = new LinearCode(N, N - curStart);
		CurrentConfig.m_pCodeFilter = nullptr;

		if (filter == "A")
			CurrentConfig.m_pCodeFilter = new SpectrumFilter(N);
		else if (filter == "W")
			CurrentConfig.m_pCodeFilter = new CosetFilter(N);
		else if (filter == "AW")
			CurrentConfig.m_pCodeFilter = new SpectrumAndCosetFilter(N);
		else if (filter == "S")
			CurrentConfig.m_pCodeFilter = new SignatureCodeFilter(N);
		else if (filter == "AWS")
			CurrentConfig.m_pCodeFilter = new CompleteCodeFilter(N);
		else if (filter == "")
			CurrentConfig.m_pCodeFilter = nullptr;
		else
			throw Exception("Unkown filter type");

		CurrentConfig.m_SearchStat = 0;
		CurrentConfig.m_RetrunStat = 0;

		CurrentConfig.m_IsBlockReached = false;

		set<unsigned> diffPD;
		for (int j = (int)CurrentConfig.m_EndPhase; j >= (int)CurrentConfig.m_StartPhase; --j) {
			CurrentConfig.vPartialDistances.push_back(m_PD[j]);
			diffPD.insert(m_PD[j]);
		}

		if (i == m_NumOfBlocks - 1)
			CurrentConfig.m_pBlockRowGenerator = new StartBlockRowGenerator();
		else {
			if (diffPD.size() > 1 || CurrentConfig.m_NumOfRows > 4)
				CurrentConfig.m_pBlockRowGenerator = new CLWTBlockRowGenerator();
			else {
				switch (CurrentConfig.m_NumOfRows) {
				case 1: CurrentConfig.m_pBlockRowGenerator = new OneWeightBlockRowGenerator1(); break;
				case 2: CurrentConfig.m_pBlockRowGenerator = new OneWeightBlockRowGenerator2(); break;
				case 3: CurrentConfig.m_pBlockRowGenerator = new OneWeightBlockRowGenerator3(); break;
				case 4: CurrentConfig.m_pBlockRowGenerator = new OneWeightBlockRowGenerator4(); break;
				default: break;
				}
			}
		}

		CurrentConfig.m_pBlockRowGenerator->Init(&CurrentConfig);

		CurrentConfig.m_pCosetLeaders = new Word[1u << (curEnd + 1)];
		CurrentConfig.m_pCLWTable = new unsigned[1u << (curEnd + 1)];

		curStart = curEnd + 1;
	}
}
