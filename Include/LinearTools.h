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

extern unsigned GPermutation[MAX_SIZE];
extern Word GTmp[MAX_SIZE];
extern Word HTmp2[MAX_SIZE];

void GaussInt(Word *pMatrix, unsigned &NumOfRows, unsigned NumOfColumns, bool ReversePass, unsigned *pPermutation);


void NullSpaceInt(Word *pMatrix  /// the matrix to be considered
                  ,
                  Word *pNullspace  //
                  ,
                  unsigned N  /// number of columns in the matrix
                  ,
                  unsigned &K  /// number of rows in the input matrix. On return, the dimension of the nullspace
                  ,
                  unsigned *pPermutation  // preferred column permutation
);


void TransposeMatrixInt(const Word *pMatrix, Word *pTransposedMatrix, unsigned NumOfRows, unsigned NumOfColumns);