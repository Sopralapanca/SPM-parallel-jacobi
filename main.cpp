#include <iostream>
#include<stdlib.h>
#include <vector>
#include <random>

#include "./utils/utimer.cpp"


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


vector<double> GenerateRandomVector(int size,int min, int max) {
    random_device rd;
    mt19937 gen(rd());

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

vector<vector<double>> GenerateRandomMatrix(double n){
    vector<vector<double>> matrix(n, vector<double>(n));

    int i;
    for( i = 0; i < n; i++){
        matrix[i] = GenerateRandomVector(n, 0.0, 10.0);
    }
    return matrix;
}

int main(int argc, char * argv[]) {
    int n = atoi(argv[1]); // matrix size
    int k = atoi(argv[2]); // number of iterations

    // generate a random linear system
    vector<vector<double>> matrix = GenerateRandomMatrix(n);

    vector<double> real_x(n);
    real_x = GenerateRandomVector(n, 0.0, 1.0);

    // compute the linear system to find b
    vector<double> b(n);
    b=compute_b(matrix, real_x, n);

    // initialize a vector with all zeroes to be used to find a solution using Jacobi
    vector<double> x(n, 0.0);

    long u;

    // sequential part
    {
        utimer tseq("Seq", &u);
        // numero di iterazioni
        for (int it=0; it<k; it++) {

            //iterate over b and x
            for(int i=0; i<n;i++) {
                //compute sum
                int sum = 0;
                for (int j = i + 1; j < n; j++) {
                    sum = matrix[i][j] * x[j];
                }

                x[i] = (b[i] - sum) / matrix[i][i];
            }
        }
    }

    cout << "time per iteration = " << u/k << endl;

    cout << "solution" << endl;
    print_vector(x);

    /*
    for (int j=0; j<k; j++){
        for (int w = 0; w < n; w++){
            int sum=0;
            for(int row= 0; row < n; row++){
                for( int col=row+1;  col < n; col++){
                    sum += a[i][j]*x[col];
                }
            }
            temp_x[w]=(b[w]-sum)/a[w][w];
        }
        for(int l= 0; l < n; l++){
            x[l]=temp_x[l];
        }
    }

    for( i = 0; i < n; ++i){
        std::cout<<x[i]<<'\n';
    }*/




    return 0;
}
