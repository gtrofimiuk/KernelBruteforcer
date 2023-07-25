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

#include "LinearTools.h"
#include <algorithm>
#include <emmintrin.h>
#include <ostream>
#include <xmmintrin.h>

using namespace std;

unsigned GPermutation[MAX_SIZE];
Word GTmp[MAX_SIZE];

void GaussInt(Word* pMatrix, unsigned& NumOfRows, unsigned NumOfColumns, bool ReversePass, unsigned* pPermutation)
{
	for (unsigned i = 0; i < NumOfRows; i++) {
		// identify the leading column
		unsigned c = i;
		bool Success = false;
		for (c; c < NumOfColumns; c++) {
			unsigned C = (pPermutation) ? pPermutation[c] : c;
			for (unsigned j = i; j < NumOfRows; j++) {
				Word t = (pMatrix[j] >> (Word)C);
				if (t & 1) { 
					Success = true;
					if (j > i)
						pMatrix[i] ^= pMatrix[j];  
					break;
				}
			}
			if (Success) {
				if ((c != i) && pPermutation) swap(pPermutation[c], pPermutation[i]);
				break;
			}
		}
		if (!Success) {
			NumOfRows = i;
			break;
		}
		unsigned LoopStart = (ReversePass) ? 0 : (i + 1);
		unsigned C = (pPermutation) ? pPermutation[i] : c;
		for (unsigned j = LoopStart; j < NumOfRows; j++) {
			if (j == i) continue;
			if ((pMatrix[j] >> (ul)C) & 1) 
				pMatrix[j] ^= pMatrix[i];
		}
	}
}

void NullSpaceInt(Word* pMatrix  /// the matrix to be considered
	,
	Word* pNullspace  //
	,
	unsigned N  /// number of columns in the matrix
	,
	unsigned& K  /// number of rows in the input matrix. On return, the dimension of the nullspace
	,
	unsigned* pPermutation  // preferred column permutation
)
{
	for (unsigned i = 0; i < N; ++i) pPermutation[i] = i;

	GaussInt(pMatrix, K, N, true, pPermutation);

	fill_n(pNullspace, N, 0);

	for (unsigned i = 0; i < N - K; i++) {
		pNullspace[pPermutation[K + i]] |= (1u << i);
		for (unsigned j = 0; j < K; j++) {
			pNullspace[pPermutation[j]] |= ((pMatrix[j] >> pPermutation[K + i]) & 1) << i;
		}
	}
}

void TransposeMatrixInt(const Word* pMatrix, Word* pTransposedMatrix, unsigned NumOfRows, unsigned NumOfColumns)
{
	std::fill_n(pTransposedMatrix, NumOfColumns, 0);

	for (unsigned i = 0; i < NumOfRows; ++i)
		for (unsigned j = 0; j < NumOfColumns; ++j)
			pTransposedMatrix[j] |= ((pMatrix[i] >> j) & 1) << i;
}