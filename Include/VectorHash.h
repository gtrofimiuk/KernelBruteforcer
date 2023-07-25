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
#include <random>

class VectorHash
{
public:
	inline static unsigned seed_counter = 8;

	VectorHash(unsigned initialN)
	{
		generator.seed(seed_counter++);

		std::uniform_int_distribution<ul> distribution(1, limit);
		for (unsigned i = 0; i < initialN; ++i) m.push_back(distribution(generator));
		m1 = distribution(generator);
	}

	unsigned Hash(unsigned *s, unsigned length)
	{
		Extend(length);

		ul sum = m1;

		unsigned q = length / 2;
		unsigned r = length % 2;

		if (r == 1) sum += s[length - 1] * m[length - 1];

		for (unsigned i = 0; i < q; ++i) {
			sum += (m[2 * i] + (ul)s[2 * i]) * (m[2 * i + 1] + (ul)s[2 * i + 1]);
		}
		return (unsigned)(sum >> 32);
	}

	void Extend(unsigned NewSize)
	{
		if (m.size() >= NewSize) return;
		size_t diff = NewSize - m.size();

		std::uniform_int_distribution<ul> distribution(1, limit);
		for (unsigned i = 0; i < diff; ++i) m.push_back(distribution(generator));
	}

private:
	vector<ul> m;
	ul m1;
	const ul limit = ~0;
	std::mt19937_64 generator;
};
