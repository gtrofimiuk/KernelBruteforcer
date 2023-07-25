/*------------------------------------------------------------------------
Simulation, entry point of application

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

#include <iostream>

#include "BlockKernelBruteforcer.h"
#include "BinomialTools.h"

#include "CmdOptionVal.h"

using namespace std;

int main(int argc, char *argv[])
{
	CMDOPTIONS_BEGIN("BlockKernelBruteforcer");

	CMDOPTION_VALUE(string, ConfigFileName, "c", "configuration file");
	CMDOPTION_VALUE_DEF(string, filter, "f", "type of filter used. \nA: weight spectrum; \nW: weight distribution of coset leaders\nS: spectrums of codes shortened at all single positions\nAW: A and W\nAWS: S, A and W", "");
	CMDOPTION_VALUE_DEF(unsigned, returnBlock, "r", "index of return block", "100000");
	CMDOPTION_VALUE_DEF(unsigned, firstBlock, "s", "Size of the first block", "5")
	CMDOPTION_VALUE_DEF(unsigned long long, timeLimit, "t", "Time limit for execution", "18446744073709551614")
	CMDOPTION_VALUE_DEF(unsigned, blockSize, "b", "Block size for bruteforcer", "3")
	//CMDOPTION_VALUE_DEF(unsigned, maxBlockR, "m", "Maximal redundancy of code, when it is allowed to use several blocks", "21")
	CMDOPTION_VALUE(bool, mixPD, "", "Allow blocks with different partial distances")
	CMDOPTION_VALUE_DEF(unsigned, maxNumOfKernels, "k", "Max number of kernels to be returned","1")
	CMDOPTION_VALUE(bool, resetCodeFilter, "l", "Reset code filter after backtracking")

	ifstream Config(ConfigFileName);
	string commandLine = "";
	for (unsigned i = 0; i < argc; ++i) {
		commandLine += string(argv[i]) + " ";
	}

	_ASSERT(_CrtCheckMemory());
	CBlockKernelBruteforcer bruteforcer(Config, filter, firstBlock, blockSize, returnBlock, timeLimit, 0, mixPD);
	bruteforcer.SetCommandLine(commandLine);
	bruteforcer.ConfigFileName = ConfigFileName;
	unsigned N = bruteforcer.GetN();
	InitBinomialTable();
	InitKrawtchoukTable(N);
	bruteforcer.MaxNumOfKernels = maxNumOfKernels;
	bruteforcer.setCodeFilterReset(resetCodeFilter);

	bruteforcer.StartSearch();


	CMDOPTIONS_END
	catch (const exception &ex)
	{
		cerr << ex.what() << endl;
		return 5;
	};

	return 0;
}
