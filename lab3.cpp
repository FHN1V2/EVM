#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <xmmintrin.h>

void fillMatrix(double* matrix, int size) {
    for (int i = 0; i < size * size; ++i) {
        matrix[i] = static_cast<double>(rand()) / 10; // случайные значения от 0 до 1
    }
}

void printMatrix(double* matrix, int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            std::cout << matrix[i * size + j] << "\t";
        }
        std::cout << std::endl;
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


void dgemm_opt_2(double* A, double* B, double* C, int size, int blockSize) {
    for (int bi = 0; bi < size; bi += blockSize) {
        for (int bj = 0; bj < size; bj += blockSize) {
            for (int bk = 0; bk < size; bk += blockSize) {
                for (int i = bi; i < std::min(bi + blockSize, size); ++i) {
                    for (int j = bj; j < std::min(bj + blockSize, size); ++j) {
                        double tempC = C[i * size + j];
                        for (int k = bk; k < std::min(bk + blockSize, size); ++k) {
                            tempC += A[i * size + k] * B[k * size + j];
                        }
                        C[i * size + j] = tempC;
                    }
                }
            }
        }
    }
}


void dgemm_opt_3(double* A, double* B, double* C, int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            __m128d tempC = _mm_setzero_pd(); // Инициализация регистра SSE с нулевыми значениями
            for (int k = 0; k < size; k += 2) {
                // Загрузка 2 значений из матрицы A и B
                __m128d tempA = _mm_loadu_pd(&A[i * size + k]);
                __m128d tempB = _mm_loadu_pd(&B[k * size + j]);
                // Умножение значений и аккумуляция
                tempC = _mm_add_pd(tempC, _mm_mul_pd(tempA, tempB));
            }
            // Суммирование элементов вектора
            double result[2];
            _mm_storeu_pd(result, tempC);
            C[i * size + j] = result[0] + result[1];
        }
    }
}



int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " matrix_size" << std::endl;
        return 1;
    }

    int size = std::atoi(argv[1]);

    if (size <= 0) {
        std::cerr << "Matrix size must be a positive integer" << std::endl;
        return 1;
    }

    double* A = new double[size * size];
    double* B = new double[size * size];
    double* C = new double[size * size];

    srand(time(nullptr)); // инициализация генератора случайных чисел
    fillMatrix(A, size);
    fillMatrix(B, size);


    //std::cout << "Matrix A:" << std::endl;
    //printMatrix(A, size);

    //std::cout << "Matrix B:" << std::endl;
    //printMatrix(B, size);
    auto start = std::chrono::high_resolution_clock::now();

    matrixMultiply(A, B, C, size);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time: " << duration.count() << " miliseconds" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    //4 задача
    dgemm_opt_1(A, B, C, size);

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time: " << duration.count() << " miliseconds" << std::endl;


    int blockSize = 250;

    start = std::chrono::high_resolution_clock::now();
    //5 задача
    dgemm_opt_2(A, B, C, size, blockSize);

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time: " << duration.count() << " miliseconds" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    //5 задача
    dgemm_opt_3(A, B, C, size);

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time: " << duration.count() << " miliseconds" << std::endl;


    //std::cout << "Result Matrix C:" << std::endl;
    //printMatrix(C, size);

    delete[] A;
    delete[] B;
    delete[] C;

    return 0;
}
