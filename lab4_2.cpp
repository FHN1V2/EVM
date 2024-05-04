#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <omp.h>

using namespace std;

// Function to fill matrix with random values
void fillMatrix(double* matrix, int size) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            matrix[i * size + j] = static_cast<double>(rand()) / 10; // Random values between 0 and 10
        }
    }
}

// Function to print matrix
void printMatrix(double* matrix, int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            cout << matrix[i * size + j] << "\t";
        }
        cout << endl;
    }
}

// Function for matrix multiplication
void matrixMultiply(double* A, double* B, double* C, int size) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            double sum = 0.0;
            for (int k = 0; k < size; ++k) {
                sum += A[i * size + k] * B[k * size + j];
            }
            C[i * size + j] = sum;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " matrix_size num_threads" << endl;
        return 1;
    }

    int size = atoi(argv[1]);
    int num_threads = atoi(argv[2]);

    if (size <= 0 || num_threads <= 0) {
        cerr << "Matrix size and number of threads must be positive integers" << endl;
        return 1;
    }

    double* A = new double[size * size];
    double* B = new double[size * size];
    double* C = new double[size * size];

    srand(time(nullptr)); // Initialize random number generator
    fillMatrix(A, size);
    fillMatrix(B, size);

    omp_set_num_threads(num_threads); // Set number of threads

    auto start = chrono::high_resolution_clock::now();

    // Perform matrix multiplication
    matrixMultiply(A, B, C, size);

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Time: " << duration.count() << " milliseconds" << endl;

    // Print result if needed
    // printMatrix(C, size);

    delete[] A;
    delete[] B;
    delete[] C;

    return 0;
}
