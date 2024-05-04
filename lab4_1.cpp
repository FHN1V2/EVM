#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <pthread.h>

using namespace std;

// Structure to hold data for each thread
struct ThreadData {
    double* A;
    double* B;
    double* C;
    int size;
    int threadId;
    int numThreads;
};

// Function to fill matrix with random values
void fillMatrix(double* matrix, int size) {
    for (int i = 0; i < size * size; ++i) {
        matrix[i] = static_cast<double>(rand()) / 10; // Random values between 0 and 1
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

// Function for each thread to perform matrix multiplication
void* multiplyMatrix(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);

    for (int i = data->threadId; i < data->size; i += data->numThreads) {
        for (int j = 0; j < data->size; ++j) {
            double sum = 0.0;
            for (int k = 0; k < data->size; ++k) {
                sum += data->A[i * data->size + k] * data->B[k * data->size + j];
            }
            data->C[i * data->size + j] = sum;
        }
    }

    pthread_exit(nullptr);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " matrix_size num_threads" << endl;
        return 1;
    }

    int size = atoi(argv[1]);
    int numThreads = atoi(argv[2]);

    if (size <= 0 || numThreads <= 0) {
        cerr << "Matrix size and number of threads must be positive integers" << endl;
        return 1;
    }

    double* A = new double[size * size];
    double* B = new double[size * size];
    double* C = new double[size * size];

    srand(time(nullptr)); // Initialize random number generator
    fillMatrix(A, size);
    fillMatrix(B, size);

    // Create an array of thread IDs
    pthread_t threads[numThreads];
    ThreadData threadData[numThreads];

    auto start = chrono::high_resolution_clock::now();

    // Create threads
    for (int i = 0; i < numThreads; ++i) {
        threadData[i] = {A, B, C, size, i, numThreads};
        pthread_create(&threads[i], nullptr, multiplyMatrix, &threadData[i]);
    }

    // Join threads
    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], nullptr);
    }

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
