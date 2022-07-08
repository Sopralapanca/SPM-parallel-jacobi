# Parallel Jacobi

A comparison between different parallel implementations of the Jacobi algorithm for determining the solutions of a system of linear equations.


## Implementation

The parallel versions of this algorithm that have been implemented are:

1. The rows of the matrix are divided one by one by workers in a cyclic manner.
2. The matrix is divided by chunks of continuous rows, each chunk is assigned to a worker.
3. The matrix is divided by chunks and thread pinning is also used.
4. Parallel implementation using FastFlow library.

A sequential version of the program has also been implemented to perform comparisons.

## Structure

```
📦JacobiSPM
 ┣ 📂results
 ┃ ┣ 📜Completion Time Matrix Size 20000.png
 ┃ ┣ ...
 ┃ ┣ 📜speedup5000.csv
 ┣ 📂utils
 ┃ ┣ 📜utility.cpp
 ┃ ┣ 📜utility.h
 ┃ ┣ 📜utimer.cpp
 ┣ 📜CMakeLists.txt
 ┣ 📜compute_stats.py
 ┣ 📜execute.sh
 ┣ 📜ff_parallel.cpp
 ┣ 📜parallel_chunks_barrier.cpp
 ┣ 📜parallel_chunks_barrier_threadpinned.cpp
 ┣ 📜parallel_row_cyclic_barrier.cpp
 ┣ 📜sequential.cpp
 ┣ 📜test_threadsetup.cpp
 ┣ 📜Makefile
 ┗ 📜README.md                     
```

## Compiling and running

A Makefile was created to compile the various versions. To create the executable files, it is possible to execute the rule

```bash
    make all
```

To run an experiment, it is possible to launch the program and pass the necessary arguments. An example is the following

```bash
    ./sequential.out matrix_size number_of_iterartions check_flag
    ./parallel_chunks_barrier.out matrix_size number_of_iterartions number_of_threads check_flag
``` 

where

- **matrix_size**: is the length of the matrix and vector. A matrix of size matrix_size*matrix_size and a vector of length matrix_size will be created.
- **number_of_iterations**: Number of iterations to be performed for Jacobi's method.
- **nw**: Degree of parallelism to be used.
- **check_flag**: Boolean for testing purposes [0] => information on the linear system will not be printed on the console [1] => information on the linear system will be printed on the console.

To run all experiments at once run the file execute.sh

```bash
    ./execute.sh
``` 

To create speedup, completion time and other tables with useful information, run the python script

```bash
    ./compute_stats.py
``` 

## Results

Below are some results for completion time and speedup with matrixes of size 5000*500

<p align="center">
<img src="results/Completion%20Time%20Matrix%20Size%205000.png" height="500" />
<img src="results/SPEEDUP%20Matrix%20Size%205000.png" height="500" />
</p>