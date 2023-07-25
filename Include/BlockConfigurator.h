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
#include "BlockConfig.h"

#include "SpectrumFilter.h"
#include "CosetFilter.h"
#include "SpectrumAndCosetFilter.h"
#include "SignatureCodeFilter.h"
#include "CompeteCodeFilter.h"

#include <iostream>
#include <set>

class BlockConfigurator
{
public:
	BlockConfigurator(){};
	BlockConfigurator(std::istream &config, unsigned blockSize, unsigned CLWTbound, unsigned maxBlockR, std::string filter, bool isUseMixPD);
	void setCodeFilterReset(bool isResetCodeFilters)
	{
		for (auto &&elem : vBlockConfigs) {
			elem.m_IsResetCodeFilter = isResetCodeFilters;
		}
	}

protected:
	unsigned *m_PD;
	unsigned N;
	unsigned m_NumOfBlocks;

	vector<BlockConfig> vBlockConfigs;
};
// we have a file, which should have the following structure
/*
N
[N integers separated by space (Partial distances)]
*/
