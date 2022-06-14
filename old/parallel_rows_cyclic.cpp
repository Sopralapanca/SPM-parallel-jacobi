#include <iostream>
#include <stdlib.h>
#include <vector>
#include <barrier>
#include <atomic>
#include <queue>
#include <mutex>
#include <functional>
#include <future>
#include <fstream>

#include "../utils/utimer.cpp"
#include "../utils/utility.h"



int main(int argc, char * argv[]) {
    int n = atoi(argv[1]); // matrix size
    int k = atoi(argv[2]); // number of iterations
    int nw = atoi(argv[3]);
    int seed = 123; //int seed = atoi(argv[3]);

    // generate a random linear system
    vector<vector<float>> matrix = GenerateRandomMatrix(n, seed);

    vector<float> actual_x(n);
    actual_x = GenerateRandomVector(n, 0.0, 1.0, seed);


    vector<float> b(n);
    b=compute_b(matrix, actual_x, n);

    // initialize a vector with all zeroes to be used to find a solution using Jacobi
    vector<float> x(n, 0.0);

    mutex ll;
    condition_variable cond, dispatcher;
    bool stop = false;

    deque<function<void()>> bind_tasks;
    auto bind_submit = [&] (function<void()> f){
        {
            unique_lock<mutex> lock(ll);
            bind_tasks.push_back(f);
        }
        cond.notify_all();
    };

    // reader
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

    vector<float> new_x(n, 0.0);


    // function to be executed by the threads
    auto f = [&](int index){
        float sum = 0;
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
    long u;
    {
        utimer tpar("Par", &u);
        for (int it = 0; it < k; it++) {
            for (int index = 0; index < n; index++) {
                auto fx = (bind(f, index));
                bind_submit(fx);
            }

            while (true) {
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

            x = new_x;
        }
    }

    cout << "Total execution time: " << u <<"usec" << endl;
    cout << "Time per iteration = " << u/k <<"usec"<< endl;

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



    ofstream myfile;
    myfile.open ("./results/parallel_rows_cyclic.txt");

    myfile << "Total execution time: " << u <<" usec\n";
    myfile << "Time per iteration: " << u/k <<" usec\n";

    myfile.close();

    cout << "Total execution time: " << u <<" usec" << endl;
    cout << "Time per iteration = " << u/k <<" usec"<< endl;


    cout << "MATRIX A" << endl;
    print_matrix(matrix, n);

    cout << "vector b" << endl;
    print_vector(b);

    cout << "vector x" << endl;
    print_vector(x);

    return 0;
}
