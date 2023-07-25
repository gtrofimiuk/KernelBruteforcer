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
#include "LinearTools.h"

class SyndromeTrellis
{
	LinearCode *m_pCode;
	unsigned *m_pCosetWeights;
	Word *m_pCosetLeaders;

	Word *m_pCosetBuffer;


public:
	SyndromeTrellis(){};
	SyndromeTrellis(LinearCode *pCode)
	: m_pCode(pCode)
	{
		m_pCosetWeights = new unsigned[1 << (m_pCode->N - m_pCode->K)];
		m_pCosetLeaders = new Word[1 << (m_pCode->N - m_pCode->K)];
		m_pCosetBuffer = new Word[1 << (m_pCode->N - m_pCode->K)];
	}

	SyndromeTrellis(LinearCode *pCode, unsigned *pCosetWeights, Word *pCosetLeaders, Word* pCosetBuffer)
	: m_pCode(pCode)
	, m_pCosetWeights(pCosetWeights)
	, m_pCosetLeaders(pCosetLeaders)
	, m_pCosetBuffer(pCosetBuffer)
	{
	}

	void InitCosetWeights()
 	{
		m_pCode->ComputeCheckMatrix();
		//_ASSERT(_CrtCheckMemory());

		std::fill_n(m_pCosetWeights, 1 << (m_pCode->N - m_pCode->K), -1);
		//_ASSERT(_CrtCheckMemory());
		m_pCosetBuffer[0] = 0;
		m_pCosetWeights[0] = 0;
		m_pCosetLeaders[0] = 0;

		unsigned NumOfSyndromes = 1;
		for (unsigned col = 0; col < m_pCode->N; ++col) {
			Word currentCol = m_pCode->HT[col];
			unsigned CurrentNumOfSyndromes = NumOfSyndromes;
			for (unsigned i = 0; i < CurrentNumOfSyndromes; ++i) {
				Word NewCoset = m_pCosetBuffer[i] ^ currentCol;
				if (m_pCosetWeights[NewCoset] == -1) {
					m_pCosetBuffer[NumOfSyndromes++] = NewCoset;
				}

				unsigned prevWeight = m_pCosetWeights[NewCoset];
				unsigned currentWeight = m_pCosetWeights[m_pCosetBuffer[i]] + 1;

				if (currentWeight < prevWeight) {
					m_pCosetWeights[NewCoset] = currentWeight;
					m_pCosetLeaders[NewCoset] = m_pCosetLeaders[m_pCosetBuffer[i]] | ((Word)1 << col);
				}
			}
		}
	}

	const unsigned *GetCosetWeights() { return m_pCosetWeights; }
	const Word *GetCosetLeaders() { return m_pCosetLeaders; }
};
