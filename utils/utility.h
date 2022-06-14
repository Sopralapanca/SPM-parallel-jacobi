#ifndef JACOBISPM_UTILITY_H
#define JACOBISPM_UTILITY_H

#include <vector>
#include <iostream>

using namespace std;

void print_matrix(vector<vector<float>> matrix, int size);

void print_vector(vector<float> vector);


vector<float> GenerateRandomVector(int size,int min, int max, int seed);

vector<float> compute_b(vector<vector<float>> a, vector<float> x, int size);

vector<vector<float>> GenerateRandomMatrix(int n, int seed);

#endif //JACOBISPM_UTILITY_H
