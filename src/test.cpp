#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>
#include <execution>
#include <thread>
#include "join_thread.h"
#include "parallen_for_each.h"
#include "parallen_find.h"
#include "parallen_sum.h"

// 测量函数执行时间
template<typename Func>
auto measure_time(Func func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

// 串行 for_each
template<typename Iterator, typename Func>
void serial_for_each(Iterator first, Iterator last, Func f) {
    std::for_each(first, last, f);
}

// 串行 find
template<typename Iterator, typename MatchType>
Iterator serial_find(Iterator first, Iterator last, MatchType match) {
    return std::find(first, last, match);
}

// 串行 partial_sum
template<typename Iterator>
void serial_partial_sum(Iterator first, Iterator last) {
    std::partial_sum(first, last, first);
}

// 返回执行时间
long long TestSerialForEach(size_t n) {
    std::vector<int> nvec(n);
    std::iota(nvec.begin(), nvec.end(), 0);
    return measure_time([&]() {
        serial_for_each(nvec.begin(), nvec.end(), [](int& i) { i *= i; });
    });
}

long long TestParallelForEach(size_t n) {
    std::vector<int> nvec(n);
    std::iota(nvec.begin(), nvec.end(), 0);
    return measure_time([&]() {
        parallel_for_each(nvec.begin(), nvec.end(), [](int& i) { i *= i; });
    });
}

long long TestCxx17ForEach(size_t n) {
    std::vector<int> nvec(n);
    std::iota(nvec.begin(), nvec.end(), 0);
    return measure_time([&]() {
        std::for_each(std::execution::par, nvec.begin(), nvec.end(), [](int& i) { i *= i; });
    });
}

long long TestSerialFind(size_t n) {
    std::vector<int> nvec(n);
    for (size_t i = 0; i < n; i++) nvec[i] = i * i;
    return measure_time([&]() {
        auto iter = serial_find(nvec.begin(), nvec.end(), (n - 1) * (n - 1));
    });
}

long long TestParallelFind(size_t n) {
    std::vector<int> nvec(n);
    for (size_t i = 0; i < n; i++) nvec[i] = i * i;
    return measure_time([&]() {
        auto iter = parallel_find(nvec.begin(), nvec.end(), (n - 1) * (n - 1));
    });
}

long long TestCxx17Find(size_t n) {
    std::vector<int> nvec(n);
    for (size_t i = 0; i < n; i++) nvec[i] = i * i;
    return measure_time([&]() {
        auto iter = std::find(std::execution::par, nvec.begin(), nvec.end(), (n - 1) * (n - 1));
    });
}

long long TestSerialPartialSum(size_t n) {
    std::vector<int> nvec(n);
    std::iota(nvec.begin(), nvec.end(), 1);
    return measure_time([&]() {
        serial_partial_sum(nvec.begin(), nvec.end());
    });
}

long long TestParallelPartialSum(size_t n) {
    std::vector<int> nvec(n);
    std::iota(nvec.begin(), nvec.end(), 1);
    return measure_time([&]() {
        parallel_partial_sum(nvec.begin(), nvec.end());
    });
}

long long TestCxx17PartialSum(size_t) {
    // 不支持直接返回-1
    return -1;
}

int main()
{
    std::vector<size_t> test_sizes = {26, 10000, 100000, 1000000, 10000000};

    for (auto n : test_sizes) {
        std::cout << "\n===== Data size: " << n << " =====" << std::endl;

        // ForEach
        auto t_serial = TestSerialForEach(n);
        auto t_parallel = TestParallelForEach(n);
        auto t_cxx17 = TestCxx17ForEach(n);

        double speedup_parallel = t_serial / double(t_parallel);
        double speedup_cxx17 = t_serial / double(t_cxx17);

        std::cout << "ForEach Tests:\n";
        std::cout << "  Serial: " << t_serial << " us\n";
        std::cout << "  Parallel: " << t_parallel << " us, Speedup: " << speedup_parallel << std::endl;
        std::cout << "  C++17 Parallel: " << t_cxx17 << " us, Speedup: " << speedup_cxx17 << std::endl;

        // Find
        t_serial = TestSerialFind(n);
        t_parallel = TestParallelFind(n);
        t_cxx17 = TestCxx17Find(n);

        speedup_parallel = t_serial / double(t_parallel);
        speedup_cxx17 = t_serial / double(t_cxx17);

        std::cout << "\nFind Tests:\n";
        std::cout << "  Serial: " << t_serial << " us\n";
        std::cout << "  Parallel: " << t_parallel << " us, Speedup: " << speedup_parallel << std::endl;
        std::cout << "  C++17 Parallel: " << t_cxx17 << " us, Speedup: " << speedup_cxx17 << std::endl;

        // PartialSum
        t_serial = TestSerialPartialSum(n);
        t_parallel = TestParallelPartialSum(n);
        t_cxx17 = TestCxx17PartialSum(n);

        speedup_parallel = t_serial / double(t_parallel);
        if (t_cxx17 != -1) {
            double speedup_cxx17 = t_serial / double(t_cxx17);
            std::cout << "  C++17 Parallel: " << t_cxx17 << " us, Speedup: " << speedup_cxx17 << std::endl;
        } else {
            std::cout << "  C++17 parallel partial_sum is not supported in GCC/libstdc++.\n";
        }
    }

    return 0;
}