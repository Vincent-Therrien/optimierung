// Baby's first cache optimization example.
// Build:
// >mkdir build
// >cd build
// >cmake ..
// >cmake --build . --config Release

#include <vector>
#include <random>
#include <iostream>
#include <chrono>
#include <map>
#include <string>
#include <utility>

using namespace std::chrono;
typedef std::vector<float> Matrix;
typedef void (*voidFunctionType)(Matrix &, Matrix &, Matrix &, unsigned int);

Matrix create_square_matrix(int N)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    Matrix matrix(N * N);
    for (unsigned int i = 0; i < N; i++) {
        for (unsigned int j = 0; j < N; j++) {
            matrix[i * N + j] = dist(gen);
        }
    }
    return matrix;
}

// Version 1: No cache awareness!
void vanilla(Matrix &A, Matrix &B, Matrix &C, unsigned int N)
{
    for (unsigned int i = 0; i < N; i++) {
        for (unsigned int j = 0; j < N; j++) {
            C[i * N + j] = 0.0f;
            for (unsigned int k = 0; k < N; k++) {
                C[i * N + j] += A[i * N + k] * B[k * N + j];
            }
        }
    }
}

// Version 2: With transposition! It actually modifies B in place.
void transpose(Matrix &M, unsigned int N)
{
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            std::swap(M[i * N + j], M[j * N + i]);
        }
    }
}

void vanilla_transposition(Matrix &A, Matrix &B, Matrix &C, unsigned int N)
{
    transpose(B, N);
    for (unsigned int i = 0; i < N; i++) {
        for (unsigned int j = 0; j < N; j++) {
            C[i * N + j] = 0.0f;
            for (unsigned int k = 0; k < N; k++) {
                C[i * N + j] += A[i * N + k] * B[j * N + k];
            }
        }
    }
}

// Version 3: Same as 2, but optimized transposition with the cache.
void cache_optimized_transpose(float *M, int n, int N) {
    if (n <= 32) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < i; j++) {
                std::swap(M[i * N + j], M[j * N + i]);
            }
        }
    }
    else {
        int k = n / 2;
        cache_optimized_transpose(M, k, N);
        cache_optimized_transpose(M + k, k, N);
        cache_optimized_transpose(M + k * N, k, N);
        cache_optimized_transpose(M + k * N + k, k, N);
        for (int i = 0; i < k; i++) {
            for (int j = 0; j < k; j++) {
                std::swap(M[i * N + (j + k)], M[(i + k) * N + j]);
            }
        }
        if (n & 1) {
            for (int i = 0; i < n - 1; i++) {
                std::swap(M[i * N + n - 1], M[(n - 1) * N + i]);
            }
        }
    }
}

void cache_optimized_transposition(Matrix &A, Matrix &B, Matrix &C, unsigned int N)
{
    cache_optimized_transpose(B.data(), N, N);
    for (unsigned int i = 0; i < N; i++) {
        for (unsigned int j = 0; j < N; j++) {
            C[i * N + j] = 0.0f;
            for (unsigned int k = 0; k < N; k++) {
                C[i * N + j] += A[i * N + k] * B[j * N + k];
            }
        }
    }
}

// version 4
// From https://en.algorithmica.org/hpc/external-memory/oblivious/
void matmul(const float *a, const float *b, float *c, int n, int N)
{
    if (n <= 32) {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                for (int k = 0; k < n; k++)
                    c[i * N + j] += a[i * N + k] * b[k * N + j];
    } else {
        int k = n / 2;
        matmul(a,     b,         c, k, N);
        matmul(a + k, b + k * N, c, k, N);
        matmul(a,     b + k,         c + k, k, N);
        matmul(a + k, b + k * N + k, c + k, k, N);
        matmul(a + k * N,     b,         c + k * N, k, N);
        matmul(a + k * N + k, b + k * N, c + k * N, k, N);
        matmul(a + k * N,     b + k,         c + k * N + k, k, N);
        matmul(a + k * N + k, b + k * N + k, c + k * N + k, k, N);
        if (n & 1) {
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                    for (int k = (i < n - 1 && j < n - 1) ? n - 1 : 0; k < n; k++)
                        c[i * N + j] += a[i * N + k] * b[k * N + j];
        }
    }

}

void cache_oblivious(Matrix &A, Matrix &B, Matrix &C, unsigned int N)
{
    matmul(A.data(), B.data(), C.data(), N, N);
}


int main()
{
    std::vector lengths = {4, 32, 64, 128, 256, 512};
    std::map<std::string, voidFunctionType> functions = {
        {"1_vanilla", *vanilla},
        {"2_vanilla_transposition", *vanilla_transposition},
        {"3_cache_optimized_transposition", *cache_optimized_transposition},
        {"4_cache_oblivious", *cache_oblivious},
    };
    std::map<std::string, std::vector<double>> durations;
    for (auto const& [name, f] : functions) {
        durations[name] = std::vector<double>();
        for (auto &N : lengths) {
            auto A = create_square_matrix(N);
            auto B = create_square_matrix(N);
            Matrix C(N * N);
            auto start = high_resolution_clock::now();
            f(A, B, C, N);
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start);
            durations[name].push_back(duration.count());
        }
    }

    for (auto const& [name, f] : functions) {
        std::cout << name << ": " << std::endl;
        unsigned int i = 0;
        for (auto &N : lengths) {
            std::cout << N << ": " << durations[name][i] << " microseconds" << std::endl;
            i++;
        }
        std::cout << std::endl;
    }
}
