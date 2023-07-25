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

class CLWTRecomputer
{
public:
	LinearCode *m_pOldCode;
	unsigned *m_pOldCosetWeights;
	Word *m_pOldCosetLeaders;

	LinearCode *m_pNewCode;
	unsigned *m_pNewCosetWeights;
	Word *m_pNewCosetLeaders;

	CLWTRecomputer(LinearCode *pOldCode,
	               LinearCode *pNewCode,
	               unsigned *pOldCosetWeights,
	               unsigned *pNewCosetWeights,
	               Word *pOldCosetLeaders,
	               Word *pNewCosetLeaders)
	: 
	  m_pOldCode(pOldCode)
	, m_pNewCode(pNewCode)
	, m_pOldCosetWeights(pOldCosetWeights)
	, m_pNewCosetWeights(pNewCosetWeights)
	, m_pOldCosetLeaders(pOldCosetLeaders)
	, m_pNewCosetLeaders(pNewCosetLeaders)
	{}
	void InitCosetWeights()
	{
		m_pNewCode->ComputeCheckMatrix();
		Word *pHT = m_pNewCode->HT;
		unsigned N = m_pNewCode->N;
		unsigned oldR = (N - m_pOldCode->K);
		unsigned limOld = 1u << oldR;
		unsigned limNew = 1u << (N - m_pNewCode->K);

		fill_n(m_pNewCosetWeights, limNew, -1);
		fill_n(m_pNewCosetLeaders, limNew, 0);

		unsigned pBasis[64];
		for (unsigned i = 0; i < oldR; ++i) {
			Word vector = m_pOldCosetLeaders[1u << i];
			pBasis[i] = CalculateSyndrome(vector, pHT, N);
		}

		m_pNewCosetWeights[0] = 0;
		m_pNewCosetLeaders[0] = 0;

		unsigned C = 0;
		unsigned s = 0;
		for (unsigned X = 1; X < limOld; X++) {
			unsigned l = _mm_popcnt_u32(~X & (X - 1)); 
			C ^= pBasis[l];
			s ^= 1u << l;
			Word cosetLeader = m_pOldCosetLeaders[s];

			unsigned oldCosetWeight = m_pOldCosetWeights[s];
			unsigned &newCosetWeight = m_pNewCosetWeights[C];

			if (oldCosetWeight < newCosetWeight) {
				newCosetWeight = oldCosetWeight;
				m_pNewCosetLeaders[C] = cosetLeader;
			}
		};
	}
};
