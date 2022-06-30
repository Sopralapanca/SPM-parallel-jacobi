#include <iostream>
#include <stdlib.h>
#include <vector>
#include <atomic>
#include <queue>
#include <mutex>
#include <functional>
#include <algorithm>
#include <barrier>
#include <fstream>
#include <sstream>

#include "./utils/utimer.cpp"
#include "./utils/utility.h"


// to compile run
// g++ ./utils/utility.cpp parallel_chunks_barrier.cpp -O3 -pthread -o parallel_chunks_barrier.out


using namespace std;


int main(int argc, char * argv[]) {
    if(argc != 5){
        cout << "Usage: matrix_size number_of_iterations number_of_threads check_flag" << endl;
        return (0);
    }

    int n = atoi(argv[1]); // matrix size
    int k = atoi(argv[2]); // number of iterations
    int nw = atoi(argv[3]); // number of threads
    bool check = (atoi(argv[4]) == 0 ? false : true);
    int seed = 123;

    // generate a random linear system
    vector<vector<float>> matrix = GenerateRandomMatrix(n, seed);

    vector<float> acutal_x(n);
    acutal_x = GenerateRandomVector(n, 0.0, 1.0, seed);

    // compute the linear system to find b
    vector<float> b(n);
    b=compute_b(matrix, acutal_x, n);

    // initialize a vector with all zeroes to be used to find a solution using Jacobi
    vector<float> x(n, 0.0), new_x(n, 0.0);

    // callback barrier
    int iterations = k;
    auto on_completion = [&]() noexcept {
        iterations--;
        x=new_x;

    };

    barrier sync_point(nw, on_completion);

    // fucntion executed by the threads
    auto body = [&](int tid, pair<int,int> ranges){
        while(iterations>0){
            for (int i=ranges.first; i< ranges.second; i++){
                float sum = 0;
                for (int j = 0; j < n; j++) {
                    if(j!=i){
                        sum += matrix[i][j] * x[j];
                    }
                }
                new_x[i] = (b[i] - sum) / matrix[i][i];
            }
            sync_point.arrive_and_wait();

        }
    };

    // the matrix is divided into blocks of continuous rows
    vector<pair<int,int>> ranges(nw);                     // vector to compute the ranges
    int delta { n / nw };
    for(int i=0; i<nw; i++) {
        ranges[i] = make_pair(i*delta,(i != (nw-1) ? (i+1)*delta : n));
    }

    vector<thread> tids(nw);

    long u;
    {
        utimer tpar("Parallel Chunks", &u);
        for(int tid=0; tid<nw; tid++){
            tids[tid] = thread(body, tid, ranges[tid]);
        }

        for(int i=0; i<nw; i++)
            tids[i].join();
    }

    ofstream myfile;
    myfile.open ("./results/parallel.csv", std::ios_base::app);

    myfile << n <<","<< nw <<"," << u <<","<<"chunks"<< endl;

    myfile.close();

    cout << "Time per iteration = " << u/k <<" usec"<< endl;

    if (check){
        cout << "MATRIX A" << endl;
        print_matrix(matrix, n);

        cout << "vector b" << endl;
        print_vector(b);

        cout << "vector x" << endl;
        print_vector(x);
    }

    return 0;
}
