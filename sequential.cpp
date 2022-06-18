#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>

#include "./utils/utimer.cpp"
#include "./utils/utility.h"

// to compile run
// g++ ./utils/utility.cpp sequential.cpp -O3 -o sequential
//if it is vectorizable
// g++ sequential.cpp -O3 -funroll-loops -ftree-vectorize -fopt-info-vec-all -o sequential |& grep sequential

using namespace std;

int main(int argc, char * argv[]) {
    if(argc != 4){
        cout << "Usage: matrix_size number_of_iterations check_flag" << endl;
        return (0);
    }

    int n = atoi(argv[1]); // matrix size
    int k = atoi(argv[2]); // number of iterations
    bool check = (atoi(argv[3]) == 0 ? false : true); // Boolean for debugging purposes
    int seed = 123;

    // generate a random linear system
    vector<vector<float>> matrix = GenerateRandomMatrix(n, seed);

    vector<float> actual_x(n);
    actual_x = GenerateRandomVector(n, 0.0, 1.0, seed);

    vector<float> b(n);
    b=compute_b(matrix, actual_x, n);

    // initialize a vector with all zeroes to be used to find a solution using Jacobi
    vector<float> x(n, 0.0), new_x(n, 0.0);

    long u;
    {
        utimer tseq("Sequential", &u);
        float sum=0;
        // numero di iterazioni
        for (int it=0; it<k; it++) {
            //iterate over b and x

            for(int i=0; i<n;i++) {
                for (int j = 0; j < n; j++) {
                    if(j!=i){
                        sum = matrix[i][j] * x[j];
                    }

                }

                x[i] = (b[i] - sum) / matrix[i][i];
            }
        }
    }

    ofstream myfile;
    myfile.open ("./results/sequential.txt", std::ios_base::app);

    myfile << "Matrix size: " << n <<" iterations: "<< k << "\n";
    myfile << "Total execution time: " << u <<" usec\n";
    myfile << "Time per iteration: " << u/k <<" usec\n\n";

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
