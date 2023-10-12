# KernelBruteforcer
 Implementation of the fast search method for large polarization kernels. Now it is considered for publication in IEEE Transactions on Communications. The algorithm is also described in my PhD thesis "Methods for design and decoding of polar codes with
large kernels" https://dissovet.itmo.ru/dissertation/?number=715759

## Build

The sources can be built with Visual Studio 2017

## Running

### Usage
  BlockKernelBruteforcer.exe [OPTION...]
```
  -c, --ConfigFileName arg   configuration file
  -f, --filter arg           type of filter used. 
                             A: weight spectrum; 
                             W: weight distribution of coset leaders
                             S: spectrums of codes shortened at all single 
                             positions
                             AW: A and W
                             AWS: S, A and W (default: "")
  -r, --returnBlock arg      index of return block (default: 100000)
  -s, --firstBlock arg       Size of the first block (default: 5)
  -t, --timeLimit arg        Time limit for execution (default: 18446744073709551614)
  -b, --blockSize arg        Block size for bruteforcer (default: 3)
      --mixPD                Allow blocks with different partial distances
  -k, --maxNumOfKernels arg  Max number of kernels to be returned (default: 1)
  -l, --resetCodeFilter      Reset code filter after backtracking
```

Configuration file is following:

First line - kernel length
Second line - whitespace separated partial distances

```
26
1 2 2 2 2 2 4 4 4 4 4 6 6 6 6 6 8 8 8 8 12 12 12 12 16 16
```

Filters are described in paper, referred as code equivalence invariants

Return block is the index of kernel row to continue search after the kernel is obtained. Sometimes it is useful to obtain kernels with different scaling exponents
### Example usage
```
BlockKernelBruteforcer.exe -c c24.txt -f AWS -b 3
```
### Output

Kernels are output to separate txt files


