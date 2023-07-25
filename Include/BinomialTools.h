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

#include "defines.h"

extern ul *binomialTable[BINOMIAL_TABLE_SIZE];
extern il **KrawtchoukTable;

void InitBinomialTable();

ul BinomialS(Word64 n, Word64 k);

ul Binomial(Word64 n, Word64 k);

il Kravchuk(Word64 x, Word64 k, Word64 n);

// We assume that N is fixed
void InitKrawtchoukTable(Word64 N);

inline il KravchukT(Word64 x, Word64 k);

// Cache the Krawtchoul polynomial
void ComputeDualSpectrum(unsigned N, unsigned Dim, unsigned *Spectrum, unsigned *DualSpectrum);