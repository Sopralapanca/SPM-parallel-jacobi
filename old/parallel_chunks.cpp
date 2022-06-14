#include <iostream>
#include <stdlib.h>
#include <vector>
#include <atomic>
#include <queue>
#include <mutex>
#include <functional>
#include <algorithm>
#include <future>

#include "../utils/utimer.cpp"
#include "../utils/utility.h"


// execute with
//for((i=1;i<33;i*=2)); do ./parallel2 20000 10 $i; done

// if matrix is small Tseq < Tsetup parallel scrivere nel report

using namespace std;


int main(int argc, char * argv[]) {
    if(argc != 4){
        cout << "Usage: matrix_size number_of_iterations number_of_threads" << endl;
        return (0);
    }

    int n = atoi(argv[1]); // matrix size
    int k = atoi(argv[2]); // number of iterations
    int nw = atoi(argv[3]); // number of threads
    int seed = 123; //int seed = atoi(argv[3]);

    // generate a random linear system
    vector<vector<float>> matrix = GenerateRandomMatrix(n, seed);

    vector<float> acutal_x(n);
    acutal_x = GenerateRandomVector(n, 0.0, 1.0, seed);

    // compute the linear system to find b
    vector<float> b(n);
    b=compute_b(matrix, acutal_x, n);

    // initialize a vector with all zeroes to be used to find a solution using Jacobi
    vector<float> x(n, 0.0);

    mutex ll;
    condition_variable worker, dispatcher;
    bool stop = false;

    deque<function<void()>> bind_tasks;
    auto bind_submit = [&] (function<void()> f){
        {
            unique_lock<mutex> lock(ll);
            bind_tasks.push_back(f);
        }
        worker.notify_all();
    };

    //evita di riempire una coda, fai semplicemente aspettare i thread su una variabile di condizionamento?
    // dovrei gestire un contatore per contare i thread che hanno finito

    // reader
    auto body = [&](int tid){
        while(true){
            function<void()> t = []() {return 0; };
            {
                unique_lock<mutex> lock(ll);
                worker.wait(lock,
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
            dispatcher.notify_one();
        }
    };

    vector<float> new_x(n, 0.0);
    vector<pair<int,int>> ranges(nw);                     // vector to compute the ranges
    int delta { n / nw };

    //rivedere load unbalanced ultimo chunk
    for(int i=0; i<nw; i++) {
        ranges[i] = make_pair(i*delta,(i != (nw-1) ? (i+1)*delta : n));
    }


    // function to be executed by the threads
    // calcola tempi somma, mostra che sono inferiori ai tempi di creazione threads, inutile parallelizzare
    auto f = [&](pair<int,int> ranges){
        float sum = 0;
        for (int i=ranges.first; i< ranges.second; i++){
            for (int j = i + 1; j < n; j++) {
                sum = matrix[i][j] * x[j];
            }
        new_x[i] = (b[i] - sum) / matrix[i][i];
        }
    };

    vector<thread> tids(nw);

    for(int tid=0; tid<nw; tid++){
        tids[tid] = thread(body, tid);
    }
    long u;
    {
        utimer tpar("Par", &u);
        for (int it = 0; it < k; it++) {
            for (int index = 0; index < nw; index++) {
                auto fx = (bind(f, ranges[index]));
                bind_submit(fx);
            }

            {
                unique_lock<mutex> lock(ll);
                dispatcher.wait(lock,
                                [&]() { return (bind_tasks.empty()); }
                );
                x = new_x;
            }
            worker.notify_all();

        }
    }


    cout << "Total execution time: " << u <<" usec" << endl;
    cout << "Time per iteration = " << u/k <<"usec"<< endl;

    auto stopTp = [&](){
        {
            unique_lock<mutex> lock(ll);
            stop=true;
        }
        worker.notify_all();
    };

    stopTp();

    for(int i=0; i<nw; i++)
        tids[i].join();



    //RIMUOVI EXTERN
    //SOSTITUISCI DOUBLE CON FLOAT PER RENDERE IL CODICE VETTORIZZABILE
    //fai la moltiplicazione per chunk e non per riga per cache locality
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
    // disabilita cache coherence protocol
    // pinna i thread sui core

    cout << "MATRIX A" << endl;
    print_matrix(matrix, n);

    cout << "vector b" << endl;
    print_vector(b);

    cout << "vector x" << endl;
    print_vector(x);

    return 0;
}
