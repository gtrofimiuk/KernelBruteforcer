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

#include "BinomialTools.h"

ul *binomialTable[BINOMIAL_TABLE_SIZE];
il **KrawtchoukTable;

void InitBinomialTable()
{
	memset(binomialTable, 0, sizeof(ul *) * BINOMIAL_TABLE_SIZE);
	binomialTable[0] = new ul[1];
	binomialTable[0][0] = 1;
	binomialTable[1] = new ul[2];
	binomialTable[1][0] = 1;
	binomialTable[1][1] = 1;
}

ul BinomialS(Word64 n, Word64 k)
{
	if (k > n) return 0;

	ul B = 1;

	for (ul i = 1; i <=(ul)(n - k); i++) {
		B *= (k + i);
		B /= i;
	}

	return B;
}

ul Binomial(Word64 n, Word64 k)
{
	if (k > n) return 0;

	if (n >= BINOMIAL_TABLE_SIZE) return BinomialS(n, k);

	if (binomialTable[n] == 0) {
		binomialTable[n] = new ul[n + 1];
		memset(binomialTable[n], 0, sizeof(ul) * (n + 1));
	}

	if (binomialTable[n][k] == 0) binomialTable[n][k] = BinomialS(n, k);

	return binomialTable[n][k];  // the sizer of the table can be further reduced, because binomial(n,k)=binomial(n,n-k)
}

il Kravchuk(Word64 x, Word64 k, Word64 n)
{
	il R = 0;
	Word64 j;
	for (j = 0; j <= k; j++) {
		il B1, B2, P;
		B1 = (il)Binomial(x, j);
		B2 = (il)Binomial(n - x, k - j);
		P = B1 * B2;
		if (j & (Word64)1)
			R -= P;
		else
			R += P;
	};
	return R;
};

// We assume that N is fixed
void InitKrawtchoukTable(Word64 N)
{
	KrawtchoukTable = new il *[N + 1];
	for (Word64 i = 0; i <= N; ++i) {
		KrawtchoukTable[i] = new il[N + 1];
		for (Word64 j = 0; j <= N; ++j) {
			KrawtchoukTable[i][j] = Kravchuk(i, j, N);
		}
	}
}

inline il KravchukT(Word64 x, Word64 k) { return KrawtchoukTable[x][k]; }

// Cache the Krawtchoul polynomial
void ComputeDualSpectrum(unsigned N, unsigned Dim, unsigned *Spectrum, unsigned *DualSpectrum)
{
	for (unsigned K = 0; K <= N; K++) {
		il A = 0;
		for (unsigned i = 0; i <= N; i++) {
			A += Spectrum[i] * KravchukT(i, K);
		}
		DualSpectrum[K] = (unsigned)A / ((unsigned)1 << Dim);
	}

	return;
}