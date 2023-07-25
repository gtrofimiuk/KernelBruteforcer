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

#include <string>

#include "BlockKernelBruteforcer.h"
#include "CodingTools.h"

#include "BlockRowGenerator.h"

#include "CLWTRecomputer.h"
#include "SyndromeTrellis.hpp"

#include <intrin.h>

#pragma intrinsic(_BitScanReverse)

using namespace std;

//#define USE_TEST_BLOCKS

// TODO: check different block grouping parameters and diff block row generators
CBlockKernelBruteforcer::CBlockKernelBruteforcer(std::ifstream &config,
                                                 std::string filter,
                                                 unsigned CLWTbound,
                                                 unsigned blockSize,
                                                 unsigned returnRow,
                                                 unsigned long long timeLimit,
                                                 unsigned maxBlockR,
                                                 bool isUseMixPD)
: BlockConfigurator(config, blockSize, CLWTbound, maxBlockR, filter, isUseMixPD)
, m_TimeLimit(timeLimit)
{
	m_pCurrentKernel = new Word[N];

	m_pCLWDist = new unsigned[N + 1];
	m_pCosetBuffer = new Word[1u << N];
	m_NumOfKernels = 0;

	m_CurrentBlock = 0;
	pTmpMatrix = new Word[N + 1];

	m_pRowBuffer = new Word[N + 1];

	m_ReturnBlock = min(returnRow, m_NumOfBlocks - 1);

	start = std::chrono::high_resolution_clock::now();
	for (auto &&elem : vBlockConfigs) {
		elem.m_pBlockRowGenerator->m_TimeLimit = m_TimeLimit;
		elem.m_pBlockRowGenerator->start = start;
	}

	tmpTime = start;
}

void CBlockKernelBruteforcer::StartSearch()
{
	auto prevDiff = 0;
	while (m_CurrentBlock >= 0) {
		SearchLoop();
		// I can provide here another type of loop

		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
		if (prevDiff != diff && diff % 2 == 0) {
			system("cls");
			cout << commandLine << endl;
			cout << "Running time time: " << diff << " seconds" << endl;

			cout << "Search stat:" << endl;
			for (int i = 0; i < m_NumOfBlocks; ++i) {
				if (!vBlockConfigs[i].m_IsBlockReached) break;
				cout << i << "\t";
				for (int j = vBlockConfigs[i].vPartialDistances.size() - 1; j >= 0; --j) {
					cout << vBlockConfigs[i].vPartialDistances[j] << ", ";
				}
				cout << "\t" << vBlockConfigs[i].m_SearchStat << "\t" << vBlockConfigs[i].m_RetrunStat;
				if (vBlockConfigs[i].m_pCodeFilter != nullptr)
					cout << "\t" << vBlockConfigs[i].m_pCodeFilter->GetSize();
				cout << endl;
			}
			cout << endl;
			cout << m_NumOfKernels << " kernels found" << endl;
		}
		prevDiff = diff;

		if (diff > m_TimeLimit) break;
	}
}

void CBlockKernelBruteforcer::SearchLoop()
{
	BlockConfig &CurBlockConfig = vBlockConfigs[m_CurrentBlock];
	BlockRowGenerator *pBlockRowGenerator = CurBlockConfig.m_pBlockRowGenerator;

	bool IsBlockReturned = pBlockRowGenerator->GetNextBlock(m_pRowBuffer);

	if (!IsBlockReturned) {
		ResetBlock();
		return;
	}

	unsigned endPhase = CurBlockConfig.m_EndPhase;
	unsigned numOfRows = CurBlockConfig.m_NumOfRows;

	for (int i = 0; i < numOfRows; ++i) {
		m_pCurrentKernel[endPhase - i] = m_pRowBuffer[i];
#ifdef USE_TEST_BLOCKS
		if (_mm_popcnt_u32(m_pRowBuffer[i]) != m_PD[endPhase - i]) {
			cout << "row buffer" << i << endl;
			cout << _mm_popcnt_u32(m_pRowBuffer[i]) << " " << m_PD[endPhase - i] << endl;
		}
#endif
	}
	LinearCode *pC = CurBlockConfig.m_C;
	unsigned K = pC->K;

	for (int i = 0; i < K; ++i) {
		m_pRowBuffer[i] = m_pCurrentKernel[N - 1 - i];  // TODO: change it
#ifdef USE_TEST_BLOCKS
		if (_mm_popcnt_u32(m_pRowBuffer[i]) != m_PD[N - 1 - i]) {
			cout << "!" << endl;
		}
#endif
	}
	pC->UpdateGenMatrix(m_pRowBuffer);

	// Do not forget about trellis

	CurBlockConfig.m_IsBlockReached = true;
	CurBlockConfig.m_SearchStat++;
	// We succesfully generated kernels
	if (m_CurrentBlock == m_NumOfBlocks - 1) {
		ReadyKernelUtility();
		return;
	}

	// TODO: try to extract more bounds from the CLWD table
	if (m_CurrentBlock <= 1) {
		SyndromeTrellis syndromeTrellis(pC, CurBlockConfig.m_pCLWTable, CurBlockConfig.m_pCosetLeaders, m_pCosetBuffer);
		syndromeTrellis.InitCosetWeights();
	} else {
		BlockConfig &prevBlockConfig = vBlockConfigs[m_CurrentBlock - 1];
		CLWTRecomputer clwt_recomputer(prevBlockConfig.m_C, pC, prevBlockConfig.m_pCLWTable, CurBlockConfig.m_pCLWTable,
		                               prevBlockConfig.m_pCosetLeaders, CurBlockConfig.m_pCosetLeaders);
		clwt_recomputer.InitCosetWeights();
	}

#ifdef USE_TEST_BLOCKS
	for (unsigned i = 0; i < lim; ++i) {
		unsigned syndrome = CalculateSyndrome(CurBlockConfig.m_pCosetLeaders[i], pC->HT, N);
		if (syndrome != i) {
			cout << "USE_TEST_BLOCKS syndrome " << i << endl;
		}
		if (CurBlockConfig.m_pCLWTable[i] != _mm_popcnt_u32(CurBlockConfig.m_pCosetLeaders[i])) {
			cout << "USE_TEST_BLOCKS weight " << i << endl;
		}
		unsigned spectrum[33];
		unsigned pd = GetPartialDistance(pC->G, CurBlockConfig.m_pCosetLeaders[i], pC->K, pC->N, spectrum);
		if (pd != CurBlockConfig.m_pCLWTable[i]) {
			cout << "USE_TEST_BLOCKS pd " << i << " " << pd << " " << CurBlockConfig.m_pCLWTable[i]
			     << _mm_popcnt_u32(CurBlockConfig.m_pCosetLeaders[i]) << endl;
		}
	}
#endif

	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
	if (diff > m_TimeLimit) std::exit(0);

	//TODO: check work for geterogenious PD!!!
	unsigned minPD = ~0;
	for (auto &&elem : vBlockConfigs[m_CurrentBlock + 1].vPartialDistances) minPD = min(elem, minPD);

	unsigned lim = 1u << (pC->N - pC->K);
	bool isPDFound = false;
	for (unsigned i = 0; i < lim; ++i) {
		if (CurBlockConfig.m_pCLWTable[i] == minPD) {
			isPDFound = true;
			break;
		}
	}

	if (!isPDFound) return;

	pC->SetCosetWeightsPointer(CurBlockConfig.m_pCLWTable);

	// check global filters
	if (CurBlockConfig.m_pCodeFilter != nullptr) {
		bool isUnique = CurBlockConfig.m_pCodeFilter->Filter(*pC);
		if (!isUnique) return;
	}

	vBlockConfigs[m_CurrentBlock + 1].m_pBlockRowGenerator->UpdateGenerator(CurBlockConfig.m_pCLWTable, CurBlockConfig.m_pCosetLeaders);

	_ASSERT(_CrtCheckMemory());
	m_CurrentBlock++;
	return;
}

void CBlockKernelBruteforcer::ResetBlock()
{
	if (m_CurrentBlock == 0) {
		m_CurrentBlock--;
		return;
	}

	vBlockConfigs[m_CurrentBlock].Reset();
	vBlockConfigs[m_CurrentBlock].m_RetrunStat++;

	m_CurrentBlock--;
}

void CBlockKernelBruteforcer::ReadyKernelUtility()
{
	PrintKernels();

	m_NumOfKernels++;
	if (m_NumOfKernels >= MaxNumOfKernels) std::exit(0);
	int retBlock = max(0, (int)m_NumOfBlocks - 1 - (int)m_ReturnBlock);
	for (int phi = m_NumOfBlocks - 1; phi > retBlock; --phi) {
		ResetBlock();
	}
}

void CBlockKernelBruteforcer::PrintKernels()
{
	cout << "kernel " << m_NumOfKernels << " found" << endl;

	ofstream out("kernel_" + to_string(m_NumOfKernels) + "_" + ConfigFileName + ".kern");
	out << N << endl;
	for (ul i = 0; i < N; ++i) {
		for (ul j = 0; j < N; ++j) {
			out << ((m_pCurrentKernel[i] >> j) & 1) << " ";
		}
		out << endl;
	}
	//out << "N-K-1\tPD\tK\t";
	//for (unsigned i = 0; i < N + 1; ++i) {
	//	out << i << "\t";
	//}
	//out << endl;

	out.close();
}
