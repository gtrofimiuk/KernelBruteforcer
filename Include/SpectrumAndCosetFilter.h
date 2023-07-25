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

#include "SingleBufferFilter.h"

class SpectrumAndCosetFilter : public SingleBufferFilter
{
public:
	SpectrumAndCosetFilter(unsigned N)
	: SingleBufferFilter((N + 1) * 2)
	{}

	virtual void FillBuffer(LinearCode &C)
	{
		assert(Len == (C.N + 1) * 2);
		assert(_CrtCheckMemory());

		C.ComputeSpectrum(C.spectrum);
		C.ComputeCLWD();
		unsigned *spectrum = C.spectrum;
		unsigned *pCLWD = C.pCLWD;

		copy_n(spectrum, C.N + 1, pBuffer);
		copy_n(pCLWD, C.N + 1, pBuffer + C.N + 1);
	}
};
