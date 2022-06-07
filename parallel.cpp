#include <iostream>
#include <stdlib.h>
#include <vector>
#include <random>
#include <barrier>
#include <atomic>
#include <queue>
#include <mutex>
#include <functional>
#include <future>
#include <optional>

#include "./utils/utimer.cpp"
#include <fstream>

/*
The Jacobi iterative method computes the result of a system of equations
Ax = B (with x vector of variable of length n, A matrix of coefficients of dimension n by n
and B vector of known terms of length n)
iteratively computing a new approximation of the values of the different variables according to the
formula:

x_i^(k+1) = (1/a_ii) * (b_i - sum_j>i a_ij*x_j^k) for all i = 1, ..., n

starting from some initial assignment of each xi (e.g. 0).
We require to implement the Jacobi method with both native C++ threads and FastFlow.

 */

using namespace std;

random_device rd;
mt19937 gen(rd());

void print_matrix(vector<vector<double>> matrix, int size){
    // Printing the  matrix
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++)
            cout << matrix[i][j] << " ";
        cout << endl;
    }
}

void print_vector(vector<double> vector){
    // Printing the  matrix
    for (int i = 0; i < vector.size(); i++){
        cout << vector[i] << " ";
        cout << endl;
    }
}


vector<double> GenerateRandomVector(int size,int min, int max, int seed) {
    gen.seed(seed);

    vector<double> vec(size);
    uniform_real_distribution<> dis(min, max);
    generate(vec.begin(), vec.end(), [&](){ return dis(gen); });
    return vec;
}

vector<double> compute_b(vector<vector<double>> a, vector<double> x, int size) {
    vector<double> b(size);
    int i, j, sum;
    for (i=0;i<size;i++){
        sum=0;
        for(j=0;j<size;j++){
            sum+=a[i][j]*x[j];
        }
        b[i]=sum;
    }

    return b;
}

vector<vector<double>> GenerateRandomMatrix(int n, int seed){
    vector<vector<double>> matrix(n, vector<double>(n));

    vector<int> seeds_vector(n);
    gen.seed(seed);

    uniform_int_distribution<int> dis(0, 10000);
    generate(seeds_vector.begin(), seeds_vector.end(), [&](){ return dis(gen); });

    cout << "seeds" << endl;
    // Printing the  matrix
    for (int i = 0; i < seeds_vector.size(); i++){
        cout << seeds_vector[i] << " ";
        cout << endl;
    }

    for( int i = 0; i < n; i++){
        matrix[i] = GenerateRandomVector(n, 0.0, 10.0, seeds_vector[i]);
    }
    return matrix;
}




int main(int argc, char * argv[]) {
    int n = atoi(argv[1]); // matrix size
    int k = atoi(argv[2]); // number of iterations
    int nw = atoi(argv[3]);
    int seed = 123; //int seed = atoi(argv[3]);

    // generate a random linear system
    vector<vector<double>> matrix = GenerateRandomMatrix(n, seed);

    vector<double> real_x(n);
    real_x = GenerateRandomVector(n, 0.0, 1.0, seed);

    // compute the linear system to find b
    vector<double> b(n);
    b=compute_b(matrix, real_x, n);

    // initialize a vector with all zeroes to be used to find a solution using Jacobi
    vector<double> x(n, 0.0);

    cout << "matrix" << endl;
    print_matrix(matrix, n);

    cout << "real x" << endl;
    print_vector(real_x);

    mutex ll;
    condition_variable cond, dispatcher;
    bool stop = false;
    // insert type of the args inside the void

    deque<function<void()>> bind_tasks;

    auto bind_submit = [&] (function<void()> f){
        {
            unique_lock<mutex> lock(ll);
            bind_tasks.push_back(f);
        }
        cond.notify_all();
    };

    // readers
    auto body = [&](int tid){
        while(true){
            function<void()> t = []() {return 0; };
            {
                unique_lock<mutex> lock(ll);
                cond.wait(lock,
                          [&]() { return (!bind_tasks.empty() || stop); }
                );
                if (!bind_tasks.empty()) {
                    t = bind_tasks.front();
                    bind_tasks.pop_front();
                }

                if (stop)
                    return;
            }
            t();
        }
    };

    vector<double> new_x(n, 0.0);


    // function to be executed by the threads
    auto f = [&](int index){
        int sum = 0;
        for (int j = index + 1; j < n; j++) {
            sum = matrix[index][j] * x[j];
        }
        new_x[index] = (b[index] - sum) / matrix[index][index];

        dispatcher.notify_one();
    };

    vector<thread> tids(nw);

    for(int tid=0; tid<nw; tid++){
        tids[tid] = thread(body, tid);
    }

    for(int it=0; it<k; it++){
        for(int index=0; index<n; index++){
            auto fx = (bind(f, index));
            bind_submit(fx);
        }

        while(true){
            {
                unique_lock<mutex> lock(ll);
                dispatcher.wait(lock,
                          [&]() { return (bind_tasks.empty()); }
                );

                if (bind_tasks.empty()) {
                    break;
                }
            }
        }

        x=new_x;
    }

    cout << "x" << endl;
    print_vector(x);

    auto stopTp = [&](){
        {
            unique_lock<mutex> lock(ll);
            stop=true;
        }
        cond.notify_all();
    };

    stopTp();

    for(int i=0; i<nw; i++)
        tids[i].join();




    // to fix
    /*
    ofstream myfile;
    myfile.open ("sequential_times.txt");

    myfile << "Total execution time: " << u <<"usec\n";
    myfile << "Time per iteration: " << u/k <<"usec\n";

    myfile.close();
    */


    //cout << "Total execution time: " << u <<"usec" << endl;
    //cout << "Time per iteration = " << u/k <<"usec"<< endl;


    // compute some overheads

    return 0;
}
