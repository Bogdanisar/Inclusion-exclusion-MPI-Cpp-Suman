# Distributed C++ solution in MPI for the 'suman' problem

## General Idea:
In competitive programming / algorithmic problems, a subset of problems are solved using the idea of the mathematical formula of the [Inclusion–exclusion principle](https://en.wikipedia.org/wiki/Inclusion%E2%80%93exclusion_principle), which refers to set cardinality.

Introduction to the usage of this concept in competitive programming can be found [here](https://infoarena.ro/problema/pinex) (infoarena, romanian) or [here](https://codeforces.com/blog/entry/64625) (codeforces, english).

The useful observation for us is that the computation needed for solving such a problem can generally be divided into many and rather independent parts, which is why it is easily open to a distributed solution in MPI (Message Passing Interface).

One such application is the 'Suman' [problem](https://infoarena.ro/problema/suman) on infoarena.
**Briefly, we want to compute the sum of natural numbers in the interval [1,N] which are divisible by at least one of v<sub>1</sub>, v<sub>2</sub>, ..., v<sub>K</sub>**. Different solutions for this are in the `./suman` directory.


## Suman

I've implemented several solutions for the 'Suman' problem:
- A sequential solution (no MPI).
- A solution using a concurrent (lock-free) stack.
- A solution using a concurrent (lock-free) queue.
- A MPI solution where each process gets and equal amount of work and the results are joined using an MPI_Reduce call.
- A MPI solution where each process dynamically gets new workload as soon as the last one has finished which might be preferential when the distributed nodes have unequal processing power.

Since the numeric results of big inputs can be quite large, regular 32bit or 64bit integers might not be sufficient. As such, each of the solutions here has a variant implementation using Big Integers from GMP (GNU Multiple Precision Arithmetic Library). Such a variant is denoted with the "_bigNumber" suffix in the file name.


## Setup (Ubuntu)

We need to install the MPI (Message Passing Interface) and the GMP (GNU Multiple Precision Arithmetic Library) libraries:

`sudo apt-get install mpich`

`sudo apt-get install libgmp3-dev`


## Executing on one machine (Ubuntu)

The programs take input as text from `./suman/suman.in` and write output as text to `./suman/suman.out` (and to stdout). They may also take args on the command-line which specify the number of threads or the debug logging level (0 - none, 1, 2, ...).



### Running the sequential variant (inside `./suman`):
- `$: source=suman_sequential_[bigNumber]`
- `$: g++ -std=c++11 ./$source.cpp -o ./$source.exe -lgmpxx -lgmp` - compilation
- `$: [time] ./$source.exe DEBUG_LEVEL` - execution

&nbsp;

### Running the concurrent variant (inside `./suman`):
- `$: source=suman_concurrent_stack[_bigNumber]`
- `$: g++ --std=c++11 ./$source.cpp -o ./$source.exe -pthread -latomic -lgmpxx -lgmp` - compilation
- `$: [time] ./$source.exe THREAD_NUMBER DEBUG_LEVEL` - execution

The source variable can by any of the concurrent-implementation files, so:
- `suman_concurrent_stack[_bigNumber]`;
- `suman_concurrent_queue_bigNumber`; - no non-bigNumber version here

&nbsp;

### Running the MPI variant (inside `./suman`):
- `$: source=suman_dynamic`
- `$: mpicxx $source.cpp -o $source.exe -lgmpxx -lgmp && chmod 755 ./$source.exe` - compilation
- `$: [time] mpirun -n NUM_PROCESSES ./$source.exe DEBUG_LEVEL` - execution

or, in one line:

`$: source=suman_dynamic; mpicxx $source.cpp -o $source.exe -lgmpxx -lgmp && chmod 755 ./$source.exe && [time] mpirun -n NUM_PROCESSES ./$source.exe DEBUG_LEVEL`

The source variable can by any of the MPI-implementation files, so:
- `suman_reduce[_bigNumber]`;
- `suman_dynamic[_bigNumber]`;

&nbsp;

## Distributed execution (MPI)

In order to run the computation in a distributed manner with the MPI variants, you need to have a SSH server enabled on the secondary machines (and the dependencies installed). Given that, you need to compile the source locally (see above) and then run:

`$: ./copy_to_other.sh IP_OF_MACHINE2 [IP_OF_MACHINE3 [...]]`

This will copy the executable (and the rest of the project) to the other machines. Care should be taken because the script might overwrite some files on the target machine at the same path as the project path on the local machine.

Then, the run command is as following:

`$: [time] mpirun -n NUM_PROCESSES -host IP1:NUM1,IP2:NUM2,IP3:NUM3 ./$source.exe DEBUG_LEVEL`

MPI will spawn up to NUM1 procs on the first machine, then up to NUM2 procs on the second machine and so forth until NUM_PROCESSES is reached.

An alternative to specifying the hosts on the command line is using a machinefile and using the `-f FILE_NAME` option with the `mpirun` command.

In a virtual machine environment, it might be useful to do the setup on one machine and then clone it as many times as needed.
