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

#include "BlockConfigurator.h"

#include <chrono>
#include <vector>

class CBlockKernelBruteforcer : public BlockConfigurator
{
public:
	CBlockKernelBruteforcer(){};
	CBlockKernelBruteforcer(std::ifstream &config, std::string filter, unsigned CLWTbound, unsigned blockSize, unsigned returnRow, unsigned long long timeLimit, unsigned maxBlockR, bool isUseMixPD);
	void StartSearch();
	unsigned GetN() { return N; }
	std::string ConfigFileName;
	void SetCommandLine(string cmd) { commandLine = cmd; }
	unsigned MaxNumOfKernels;

private:
	int m_CurrentBlock;
	unsigned *m_pCLWDist;

	Word *m_pCosetBuffer;

	Word *m_pCurrentKernel;
	Word *m_pRowBuffer;
	std::string commandLine;

	// internal functions
	void SearchLoop();
	void ResetBlock();
	void ReadyKernelUtility();
	void PrintKernels();

	// TODO: >= 1
	unsigned m_ReturnBlock;

	unsigned m_NumOfKernels;
	unsigned m_LPTimer;

	Word *pTmpMatrix;

	unsigned long long m_TimeLimit;

	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point tmpTime;
};
