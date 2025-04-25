// False sharing simple example.

#include <vector>
#include <random>
#include <iostream>
#include <chrono>
#include <utility>

using namespace std::chrono;

template <typename T>
inline std::vector<T> create_array(unsigned int N)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    using Distribution = typename std::conditional<
        std::is_floating_point<T>::value,
        std::uniform_real_distribution<T>,
        std::uniform_int_distribution<T>
    >::type;
    Distribution dist(-10, 10);
    std::vector<T> v(N);
    for (unsigned int i = 0; i < N; i++) {
        auto value = dist(gen);
        v[i] = value ? value : 1.0;
    }
    return v;
}

template <typename T>
inline T sum(const std::vector<T> &v)
{
    T sum = 0;
    for (unsigned int i = 0; i < v.size(); i++) {
        sum += v[i];
    }
    return sum;
}

template <typename T>
inline T multiply(const std::vector<T> &v)
{
    T sum = 1;
    for (unsigned int i = 0; i < v.size() - 1; i++) {
        sum *= v[i];
    }
    return sum;
}

template <typename T>
inline T divide(const std::vector<T> &v)
{
    T sum = 1;
    for (unsigned int i = 0; i < v.size() - 1; i++) {
        sum /= v[i];
    }
    return sum;
}

/// @brief Measure the time required to add up N numbers 1000 times. The `result` has to be printed
///        to stdout to ensure that the compiler does not skip the computations.
/// @tparam T Type to benchmark.
/// @param N Number of elements to add up.
/// @param name Name of the type.
template <typename T>
void benchmark_sum(unsigned int N, std::string name) {
    auto v = create_array<T>(N);
    T result {};
    auto start = high_resolution_clock::now();
    int REPEATS = 1000;
    for (int i = 0; i < REPEATS; i++) {
        result = sum(v);
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    std::cout << name << ",Add," << duration.count()  << "," << result << "," << std::endl;
}

/// @brief Measure the time required to perform N divisions.
/// @tparam T Type to benchmark.
/// @param N Number of elements to add up.
/// @param name Name of the type.
template <typename T>
void benchmark_multiply(unsigned int N, std::string name) {
    auto v = create_array<T>(N);
    auto start = high_resolution_clock::now();
    T result = multiply(v);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    std::cout << name << ",Multiply," << duration.count()  << "," << result << "," << std::endl;
}

/// @brief Measure the time required to perform N divisions.
/// @tparam T Type to benchmark.
/// @param N Number of elements to add up.
/// @param name Name of the type.
template <typename T>
void benchmark_divide(unsigned int N, std::string name) {
    auto v = create_array<T>(N);
    auto start = high_resolution_clock::now();
    T result = divide(v);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    std::cout << name << ",Divide," << duration.count()  << "," << result << "," << std::endl;
}

int main()
{
    unsigned int N = 10'000'000;
    std::cout << "Type,Operation,Duration,Result," << std::endl;
    benchmark_sum<float>(N, "float");
    benchmark_sum<double>(N, "double");
    benchmark_sum<long double>(N, "long double");
    benchmark_sum<int16_t>(N, "int16_t");
    benchmark_sum<int32_t>(N, "int32_t");
    benchmark_sum<int64_t>(N, "int64_t");

    benchmark_multiply<float>(N, "float");
    benchmark_multiply<double>(N, "double");
    benchmark_multiply<long double>(N, "long double");
    benchmark_multiply<int16_t>(N, "int16_t");
    benchmark_multiply<int32_t>(N, "int32_t");
    benchmark_multiply<int64_t>(N, "int64_t");

    benchmark_divide<float>(N, "float");
    benchmark_divide<double>(N, "double");
    benchmark_divide<long double>(N, "long double");
    benchmark_divide<int16_t>(N, "int16_t");
    benchmark_divide<int32_t>(N, "int32_t");
    benchmark_divide<int64_t>(N, "int64_t");
}
