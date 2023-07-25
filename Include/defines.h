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

#include <cassert>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <algorithm>
#include <csetjmp>
#include <exception>
#include <fstream>
#include <functional>
#include <immintrin.h>
#include <intrin.h>
#include <iostream>
#include <nmmintrin.h>
#include <string>

#include "exception.h"

#define MAX_SIZE 64
#define MAX_NUM_OF_ACTIVE_BITS 28u
#define BINOMIAL_TABLE_SIZE 50

#define AVX

typedef unsigned __int64 ul;
typedef __int64 il;
typedef unsigned Word;
typedef unsigned __int64 Word64;
typedef unsigned char tBit;

//extern std::jmp_buf JUMP_BUFFER;