#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

#include "utility.h"

using namespace std;

random_device rd;
mt19937 gen(rd());

void print_matrix(vector<vector<float>> matrix, int size){
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++)
            cout << matrix[i][j] << " ";
        cout << endl;
    }
}

void print_vector(vector<float> vector){
    for (int i = 0; i < vector.size(); i++){
        cout << vector[i] << " ";
        cout << endl;
    }
}


vector<float> GenerateRandomVector(int size,int min, int max, int seed) {
    gen.seed(seed);
    vector<float> vec(size);
    uniform_real_distribution<> dis(min, max);
    generate(vec.begin(), vec.end(), [&](){ return dis(gen); });
    return vec;
}

vector<float> compute_b(vector<vector<float>> a, vector<float> x, int size) {
    vector<float> b(size);
    int i, j;
    float sum;
    for (i=0;i<size;i++){
        sum=0;
        for(j=0;j<size;j++){
            sum+=a[i][j]*x[j];
        }
        b[i]=sum;
    }

    return b;
}

vector<vector<float>> GenerateRandomMatrix(int n, int seed){
    // given the initial seed, generate a vector of random numbers
    // representing seeds to randomly generate vectors for the matrix
    vector<int> seeds_vector(n);
    gen.seed(seed);
    uniform_int_distribution<int> dis(0, 10000);
    generate(seeds_vector.begin(), seeds_vector.end(), [&](){ return dis(gen); });

    vector<vector<float>> matrix(n, vector<float>(n));
    for( int i = 0; i < n; i++){
        matrix[i] = GenerateRandomVector(n, 0.0, 10.0, seeds_vector[i]);
    }
    return matrix;
}
