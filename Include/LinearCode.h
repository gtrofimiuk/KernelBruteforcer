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

#include "CodingTools.h"
#include "LinearTools.h"
#include "defines.h"

#include <vector>
#ifdef AVX
#include <immintrin.h>
#endif

using namespace std;
class LinearCode
{
public:
	unsigned N;
	unsigned K;

	/* Generator matrix */
	Word *G;
	/*Temporary generator matrix for various functions*/
	Word *TmpG;

	/* Check matrix */
	Word *H;

	/* Transposed check matrix */
	Word *HT;

	bool isCheckMatrixComputed;
	bool isSpectrumComputed;
	bool isSignatureComputed;
	bool isCLWDComputed;

	unsigned *spectrum;
	unsigned *dualSpectrum;
	unsigned *pCLWD;
	unsigned *TmpSpectrum;
	unsigned *pCosetWeights;

	// std::vector<std::pair<Word64, unsigned *>> Signature;
	unsigned **ppSignatureArray;
	std::vector<unsigned> SignatureVector;
	std::vector<unsigned> TmpVector;

#ifdef AVX
	__m256i **ppShortenedSpectrums256;
	__m256i **ppG256;
#endif

	LinearCode(unsigned _N, unsigned _K)
	{
		isCheckMatrixComputed = false;
		isSpectrumComputed = false;
		isSignatureComputed = false;

		N = _N;
		K = _K;
		G = new Word[K];
		TmpG = new Word[K];
		H = new Word[N - K];
		HT = new Word[N];
		spectrum = new unsigned[N + 1];
		dualSpectrum = new unsigned[N + 1];
		pCLWD = new unsigned[N + 1];
		TmpSpectrum = new unsigned[N + 1];

		memset(H, (Word)0, sizeof(Word) * (N - K));
		memset(HT, (Word)0, sizeof(Word) * N);
		memset(spectrum, 0, sizeof(unsigned) * (N + 1));
		memset(dualSpectrum, 0, sizeof(unsigned) * (N + 1));
		memset(pCLWD, 0, sizeof(unsigned) * (N + 1));

		spectrum[0] = 1;
		dualSpectrum[0] = 1;

		ppSignatureArray = nullptr;
#ifdef AVX
		ppShortenedSpectrums256 = nullptr;
		ppG256 = nullptr;
#endif
	}

	LinearCode(unsigned _N, unsigned _K, Word *_G)
	: LinearCode(_N, _K)
	{
		memcpy(G, _G, sizeof(Word) * K);
	}

	void ComputeCheckMatrix()
	{
		if (isCheckMatrixComputed) return;

		memcpy(TmpG, G, sizeof(Word) * K);
		_ASSERT(_CrtCheckMemory());

		unsigned prevK = K;
		NullSpaceInt(TmpG, HT, N, K, GPermutation);
		_ASSERT(prevK == K);

		TransposeMatrixInt(HT, H, N, N - K);

		isCheckMatrixComputed = true;
	}

	void UpdateGenMatrix(Word *pG)
	{
		isCheckMatrixComputed = false;
		isSpectrumComputed = false;
		isSignatureComputed = false;
		memcpy(G, pG, sizeof(Word) * K);
	}

	void ComputeSpectrum(unsigned *pSpectrum)
	{
		if (isSpectrumComputed) return;

		unsigned tmpK = K;
		if (K >= (N - 1) / 2) {
			ComputeCheckMatrix();
			SimpleComputeSpectrum(H, N-K, N, pSpectrum);
			//GetMinimalDistanceDualFromCheck(H, N - K, N, pSpectrum);
		} else
			SimpleComputeSpectrum(G, K, N, pSpectrum);

		if (pSpectrum != spectrum) copy_n(pSpectrum, N + 1, spectrum);
		isSpectrumComputed = true;
	}

	void SetCosetWeightsPointer(unsigned *CosetWeights) { 
		pCosetWeights = CosetWeights; 
		isCLWDComputed = false;
	}

	void SetCLWD(unsigned *CLWD) { 
		copy_n(CLWD, N + 1, pCLWD); 
		isCLWDComputed = true;
	}

	void ComputeCLWD()
	{
		if (isCLWDComputed) return;

		fill_n(pCLWD, N + 1, 0);
		for (unsigned i = 0; i < (1u << (N - K)); ++i) {
			pCLWD[pCosetWeights[i]]++;
		}
		isCLWDComputed = true;
	}

	void ComputeSignature()
	{
		if (isSignatureComputed) return;

		if (ppSignatureArray == nullptr) {
			ppSignatureArray = new unsigned *[N];
			for (unsigned i = 0; i < N; ++i) {
				ppSignatureArray[i] = new unsigned[N + 1];
			}
		}

		Word *pMatrix = G;
		unsigned R = K;
		if (K >= (N - 1) / 2) {
			ComputeCheckMatrix();
			pMatrix = H;
			R = N - K;
		}

		for (unsigned i = 0; i < N; ++i) {
			fill_n(ppSignatureArray[i], N + 1, 0);
		}
		fill_n(spectrum, N + 1, 0);
		Word C = 0;

#ifdef AVX
		if (ppShortenedSpectrums256 == nullptr) {
			ppShortenedSpectrums256 = new __m256i *[N + 1];
			for (unsigned i = 0; i <= N; ++i) {
				ppShortenedSpectrums256[i] = new __m256i[4];
			}
		}
		if (ppG256 == nullptr) {
			ppG256 = new __m256i *[R];
			for (unsigned i = 0; i < R; ++i) {
				ppG256[i] = new __m256i[4];
			}
		}

		for (unsigned i = 0; i <= N; ++i) {
			for (unsigned j = 0; j < 4; ++j) {
				ppShortenedSpectrums256[i][j] = _mm256_setzero_si256();
			}
		}
		for (unsigned i = 0; i < R; ++i) {
			for (unsigned j = 0; j < 4; ++j) {
				ppG256[i][j] = _mm256_setzero_si256();
				Word curPiece = (pMatrix[i] >> (j * 8)) & 255;
				for (unsigned b = 0; b < 8; ++b) {
					ppG256[i][j].m256i_i32[b] = ((curPiece >> b) & 1);
				}
			}
		}

		const __m256i Ones = _mm256_set1_epi32(1);
		__m256i tDst;

		__m256i C256[4] = { _mm256_setzero_si256(), _mm256_setzero_si256(), _mm256_setzero_si256(), _mm256_setzero_si256() };
		for (Word X = 1; X < ((Word)1 << R); X++) {
			unsigned l = _mm_popcnt_u32(~X & (X - 1));
			C ^= pMatrix[l];
			unsigned d = _mm_popcnt_u32(C);
			spectrum[d]++;

			for (unsigned j = 0; j < 4; ++j) {
				C256[j] = _mm256_xor_si256(C256[j], ppG256[l][j]);
				tDst = _mm256_xor_si256(C256[j], Ones);
				ppShortenedSpectrums256[d][j] = _mm256_add_epi32(ppShortenedSpectrums256[d][j], tDst);
			}
		};

		for (unsigned d = 1; d <= N; ++d) {
			for (unsigned j = 0; j < 4; ++j) {
				for (unsigned b = 0; b < 8; ++b) {
					unsigned index = j * 8 + b;
					if (index >= N) continue;
					ppSignatureArray[index][d] = ppShortenedSpectrums256[d][j].m256i_i32[b];
				}
			}
		}
#else
		for (Word X = 1; X < (1 << R); X++) {
			unsigned l = _mm_popcnt_u32(~X & (X - 1));
			C ^= pMatrix[l];
			unsigned d = _mm_popcnt_u32(C);
			spectrum[d]++;

			for (unsigned b = 0; b < N; ++b) {
				unsigned bit = (C >> b) & 1;
				pSignatureArray[b][d] += !bit;
			}
		};
#endif

		isSpectrumComputed = true;
		isSignatureComputed = true;
	}
};
