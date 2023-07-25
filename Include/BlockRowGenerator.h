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
//#include "BlockConfig.h"
//The block generator is guaranteed to return correct rows
class BlockConfig;
#include <chrono>
class BlockRowGenerator
{
public:
	virtual void Init(BlockConfig *BlockConfig) = 0;
	virtual void UpdateGenerator(unsigned *pCLWTable = nullptr, Word *pCosetLeaders = nullptr) = 0;
	virtual bool GetNextBlock(Word *pReturnedRows) = 0;
	virtual void Reset() = 0;
	BlockRowGenerator(){};
	std::chrono::high_resolution_clock::time_point start;
	unsigned m_TimeLimit;

protected:
	BlockConfig *m_pBlockConfig;
	void CheckTimeLimit() {
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
		if (diff > m_TimeLimit) std::exit(0);
	}
};
