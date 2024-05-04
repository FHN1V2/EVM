#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>


using namespace std;

void fillMatrix(double* matrix, int size) {
    for (int i = 0; i < size * size; ++i) {
        matrix[i] = static_cast<double>(rand()) / 10; // случайные значения от 0 до 1
    }
}

void printMatrix(double* matrix, int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            cout << matrix[i * size + j] << "\t";
        }
        cout << endl;
    }
}

void matrixMultiply(double* A, double* B, double* C, int size) {
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

void dgemm_opt_1(double* A, double* B, double* C, int size) {
    for (int i = 0; i < size; ++i) {
        for (int k = 0; k < size; ++k) {
            double tempA = A[i * size + k];
            for (int j = 0; j < size; ++j) {
                C[i * size + j] += tempA * B[k * size + j];
            }
        }
    }
}

/*
void dgemm_opt_2(double* A, double* B, double* C, int size, int blockSize) {
    for (int bi = 0; bi < size; bi += blockSize) {
        for (int bj = 0; bj < size; bj += blockSize) {
            for (int bk = 0; bk < size; bk += blockSize) {
                for (int i = bi; i < min(bi + blockSize, size); ++i) {
                    for (int j = bj; j < min(bj + blockSize, size); ++j) {
                        double tempC = C[i * size + j];
                        for (int k = bk; k < min(bk + blockSize, size); ++k) {
                            tempC += A[i * size + k] * B[k * size + j];
                        }
                        C[i * size + j] = tempC;
                    }
                }
            }
        }
    }
}
*/




int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " matrix_size" << endl;
        return 1;
    }

    int size = atoi(argv[1]);

    if (size <= 0) {
        cerr << "Matrix size must be a positive integer" << endl;
        return 1;
    }

    double* A = new double[size * size];
    double* B = new double[size * size];
    double* C = new double[size * size];

    srand(time(nullptr)); // инициализация генератора случайных чисел
    fillMatrix(A, size);
    fillMatrix(B, size);


    //cout << "Matrix A:" << endl;
    //printMatrix(A, size);

    //cout << "Matrix B:" << endl;
    //printMatrix(B, size);
    auto start = chrono::high_resolution_clock::now();

    matrixMultiply(A, B, C, size);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Time: " << duration.count() << " miliseconds" << endl;

    start = chrono::high_resolution_clock::now();
    //4 задача
    dgemm_opt_1(A, B, C, size);

    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Time: " << duration.count() << " miliseconds" << endl;


    //cout << "Result Matrix C:" << endl;
    //printMatrix(C, size);

    delete[] A;
    delete[] B;
    delete[] C;

    return 0;
}
