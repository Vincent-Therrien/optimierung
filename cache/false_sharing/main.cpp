// False sharing simple example.

#include <vector>
#include <random>
#include <iostream>
#include <chrono>
#include <map>
#include <string>
#include <utility>
#include <thread>

using namespace std::chrono;

std::vector<float> create_array(unsigned int N)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    std::vector<float> v(N);
    for (unsigned int i = 0; i < N; i++) {
        v[i] = dist(gen);
    }
    return v;
}

/// @brief Computations carried out by each thread.
/// @param a
/// @param chunk
/// @param n_elements
/// @param results
/// @param cache_line_size offset when writing the results
void partial_sum(
    const std::vector<float> &a,
    int chunk,
    int n_elements,
    std::vector<float> &results,
    unsigned int cache_line_size)
{
    int start = chunk * n_elements;
    int end = std::min(start + n_elements, (int)a.size());
    for (int i = start; i < end; i++) {
        results[chunk * cache_line_size] += a[i];
    }
}

/// @brief  Dispatching
/// @param a
/// @param n_threads
/// @param cache_line_size offset when writing the results
void parallel_sum(
    std::vector<float> &a,
    int n_threads,
    unsigned int cache_line_size)
{
    std::vector<std::thread> threads;
    std::vector<float> results;

    if (n_threads == 1) {
        results.resize(1);
        partial_sum(std::ref(a), 0, a.size(), std::ref(results), cache_line_size);
    }
    else {
        results.resize(n_threads * cache_line_size);
        for (int t = 0; t < n_threads; t++) {
            int n_elements = (a.size() + n_threads - 1) / n_threads;
            threads.emplace_back(partial_sum, std::ref(a), t, n_elements, std::ref(results), cache_line_size);
        }
        float result = 0.0;
        for (int t = 0; t < n_threads; t++) {
            threads[t].join();
            result += results[t * cache_line_size];
        }
    }
}

int main()
{
    std::vector<unsigned int> cache_line_sizes = {1, 2, 4, 8, 16, 32, 64, 128};
    unsigned int N = 10000;
    unsigned int T = 16;
    std::map<unsigned int, double> durations;
    for (auto const& cache_line_size : cache_line_sizes) {
        auto A = create_array(N);
        std::vector<float> result(T * cache_line_size, 0);
        auto start = high_resolution_clock::now();
        parallel_sum(A, T, cache_line_size);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        durations[cache_line_size] = duration.count();
    }
    // Print results to stdout with a CSV-style output.
    std::cout << "Offset,Duration," << std::endl;
    for (auto const& [cache_line_size, duration] : durations) {
        std::cout << cache_line_size << "," << duration << std::endl;
    }
}
