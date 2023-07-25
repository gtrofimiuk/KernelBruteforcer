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

#include "CodingTools.h"
#include "BinomialTools.h"
#include "LinearTools.h"

Word CalculateSyndrome(Word vector, Word *pTransposedCheckMatrix, unsigned N)
{
	Word syndrome = 0;
	for (unsigned j = 0; j < N; ++j) {
		unsigned b = ((vector >> j) & 1);
		unsigned mask = 0 - b;
		
		syndrome ^= pTransposedCheckMatrix[j] & mask;
	}

	return syndrome;
}

void SimpleComputeSpectrum(Word *Matrix, unsigned K, unsigned N, unsigned *spectrum)
{
	Word C = 0;

	memset(spectrum, (unsigned)0, sizeof(unsigned) * (N + 1));
	spectrum[0]++;

	for (Word X = 1; X < (1 << K); X++) {
		unsigned l = _mm_popcnt_u32(~X & (X - 1));
		C ^= Matrix[l];
		unsigned d = _mm_popcnt_u32(C);

		spectrum[d]++;
	};
}

unsigned dualSpectrum[MAX_SIZE];

void GetMinimalDistanceDualFromCheck(Word *CheckMatrix, unsigned R, unsigned N, unsigned *spectrum)
{
	SimpleComputeSpectrum(CheckMatrix, R, N, dualSpectrum);
	ComputeDualSpectrum(N, R, dualSpectrum, spectrum);
}
