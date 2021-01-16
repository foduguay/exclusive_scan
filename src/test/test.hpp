#ifndef TEST_H
#define TEST_H

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>
#include <set>
#include "io.hpp"
#include "../exclusive_scan.hpp"

// Use (void) to silent unused warnings.
#define assertm(exp, msg) assert(((void)msg, exp))

namespace exclusive_scan_test {
    const std::string BASE_PATH = std::filesystem::current_path().string() + std::string("/data/test/");

    template<class TYPE, int MAX_SIZE>
    void _test_type(const std::string& test_name, bool success) {
        std::array<TYPE, MAX_SIZE> in;
        file_deserialize<TYPE, MAX_SIZE>(test_name + ".in", in);
        std::array<TYPE, MAX_SIZE> expected;
        file_deserialize<TYPE, MAX_SIZE>(test_name + ".exp", expected);
        {
            std::array<TYPE, MAX_SIZE> out;
            opencl_exclusive_scan::exclusive_scan(in.data(), out.data(), in.size());
            assertm (success == (out == expected), "opencl_exclusive_scan failed");
        }
        {
            std::array<TYPE, MAX_SIZE> out;
            threaded_exclusive_scan::exclusive_scan(in.data(), out.data(), in.size());
            assertm (success == (out == expected), "threaded_exclusive_scan failed");
        }
        {
            std::array<TYPE, MAX_SIZE> out;
            naive_exclusive_scan::exclusive_scan(in.data(), out.data(), in.size());
            assertm (success == (out == expected), "naive_exclusive_scan failed");
        }
        file_serialize<TYPE, MAX_SIZE>(test_name + ".out", expected);
    }
    template<int MAX_SIZE>
    void _test_all_types(const std::string& test_name, bool success) {
        std::cout << "RUNNING " + test_name << std::endl;
        std::cout << "__char__" << std::endl;
        _test_type<char, MAX_SIZE>(test_name, success);
        std::cout << "TEST " << test_name << " PASSED!" << std::endl << std::endl;
    }
    template<class TYPE, int MAX_SIZE>
    void _generate_test(const std::string& test_name, const std::array<TYPE, MAX_SIZE>& in, const std::array<TYPE, MAX_SIZE>& expected) {
        file_serialize<TYPE, MAX_SIZE>(test_name + ".in", in);
        file_serialize<TYPE, MAX_SIZE>(test_name + ".exp", expected);
    }

    template<class TYPE, int MAX_SIZE>
    void _generate_random_array(std::array<TYPE, MAX_SIZE>& out) {
        std::random_device rnd_device;
        std::mt19937 mersenne_engine {rnd_device()};
        std::uniform_int_distribution<char> dist {0, 1};
        auto gen = [&dist, &mersenne_engine]() {
            return dist(mersenne_engine);
        };
        std::generate(begin(out), end(out), gen);
    }

    template<class TYPE, int MAX_SIZE>
    void _conditionally_generate_successful_test(int open_number) {
        std::array<TYPE, MAX_SIZE> in;
        _generate_random_array<TYPE, MAX_SIZE>(in);
        std::array<TYPE, MAX_SIZE> expected;

        // Trust our naive method for generating longer tests
        naive_exclusive_scan::exclusive_scan(in.data(), expected.data(), in.size());
        std::string test_name = BASE_PATH + std::string("test_success_") + std::to_string(open_number);
        _generate_test<TYPE, MAX_SIZE>(test_name, in, expected);
    }

    void test_all() {
        //_conditionally_generate_successful_test<char, 1024>(1);
        //_conditionally_generate_successful_test<char, 1024>(2);
        for (const auto & entry : std::filesystem::directory_iterator(BASE_PATH)) {
            auto str = entry.path().string();
            int last_dot = str.find_last_of(".");
            if (str.substr(last_dot + 1, str.size()) == "in") {
                auto test_name = str.substr(0, last_dot);
                bool expected_success = (std::string::npos != test_name.find("success"));
                std::cout << test_name << std::endl;
                _test_all_types<1024>(test_name, expected_success);
            }
        }
    }

    template<class TYPE, int MAX_SIZE>
    void chronometer_type(int size) {
        std::array<TYPE, MAX_SIZE> in;
        _generate_random_array<TYPE, MAX_SIZE>(in);
        std::array<TYPE, MAX_SIZE> expected;

        auto t_start = std::chrono::high_resolution_clock::now();
        for(int i=0, n = 1024 * 1024 / size; i < n; ++i) {
            naive_exclusive_scan::exclusive_scan(in.data(), expected.data(), MAX_SIZE);
        }
        auto t_end = std::chrono::high_resolution_clock::now();
        std::cout  << std::setw(9) << size << " |" << std::setw(10) << std::fixed << std::setprecision(2) << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " |";

        t_start = std::chrono::high_resolution_clock::now();
        for(int i=0, n = 1024 * 1024 / size; i < n; ++i) {
            opencl_exclusive_scan::exclusive_scan(in.data(), expected.data(), MAX_SIZE);
        }
        t_end = std::chrono::high_resolution_clock::now();
        std::cout << std::setw(10) << std::fixed << std::setprecision(2) << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " |";

        t_start = std::chrono::high_resolution_clock::now();
        for(int i=0, n = 1024 * 1024 / size; i < n; ++i) {
            threaded_exclusive_scan::exclusive_scan(in.data(), expected.data(), MAX_SIZE);
        }
        t_end = std::chrono::high_resolution_clock::now();
        std::cout << std::setw(10) << std::fixed << std::setprecision(2) << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " |" << std::endl;
    }

    void chronometer() {
        std::cout << "          time in milliseconds for computations" << std::endl;
        std::cout << "          -------------------------------------" << std::endl;
        std::cout << "block size|" << std::setw(10) << "naive" << " |" << std::setw(10) << "opencl" << " |" << std::setw(10) << "threads" << " |" << std::endl;
        std::cout << "          -------------------------------------" << std::endl;
        chronometer_type<char, 1024 * 1024>(1 * 1024);
        chronometer_type<char, 1024 * 1024>(4 * 1024);
        chronometer_type<char, 1024 * 1024>(8 * 1024);
        chronometer_type<char, 1024 * 1024>(16 * 1024);
        chronometer_type<char, 1024 * 1024>(32 * 1024);
        chronometer_type<char, 1024 * 1024>(64 * 1024);
        chronometer_type<char, 1024 * 1024>(128 * 1024);
        chronometer_type<char, 1024 * 1024>(256 * 1024);
        chronometer_type<char, 1024 * 1024>(512 * 1024);
        chronometer_type<char, 1024 * 1024>(1024 * 1024);
        std::cout << "          -------------------------------------" << std::endl;
    }

}

#endif // TEST_H
