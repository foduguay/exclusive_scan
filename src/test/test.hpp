// DONT FORGET PRAGMA ONCE

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>
#include <set>
#include <vector>
#include "io.hpp"
#include "../exclusive_scan.hpp"

// Use (void) to silent unused warnings.
#define assertm(exp, msg) assert(((void)msg, exp))

namespace exclusive_scan_test {
    const std::string BASE_PATH = std::filesystem::current_path().string() + std::string("/data/test/");

    template<class TYPE>
    void _test_type(const std::string& test_name, bool success) {
        std::vector<TYPE> in;
        file_deserialize(test_name + ".in", in);
        std::vector<TYPE> expected;
        file_deserialize(test_name + ".exp", expected);
        {
            std::vector<TYPE> out(in.size());
            naive_exclusive_scan::exclusive_scan(in.data(), out.data(), in.size());
            assertm (success == (out == expected), "naive_exclusive_scan failed");
        }
        {
            std::vector<TYPE> out(in.size());
            avx_exclusive_scan::exclusive_scan(in.data(), out.data(), in.size());
            assertm (success == (out == expected), "avx_exclusive_scan failed");
        }
        {
            std::vector<TYPE> out(in.size());
            cuda_exclusive_scan::exclusive_scan(in.data(), out.data(), in.size());
            assertm (success == (out == expected), "cuda_exclusive_scan failed");
        }
        file_serialize(test_name + ".out", expected);
    }
    void _test_all_types(const std::string& test_name, bool success) {
        std::cout << "RUNNING " + test_name << std::endl;
        std::cout << "__u_char__" << std::endl;
        _test_type<u_char>(test_name, success);
        std::cout << "__u_int__" << std::endl;
        _test_type<u_int>(test_name, success);
        std::cout << "__char__" << std::endl;
        _test_type<char>(test_name, success);
        std::cout << "__int__" << std::endl;
        _test_type<int>(test_name, success);
        std::cout << "__int64_t__" << std::endl;
        _test_type<int64_t>(test_name, success);
        std::cout << "__double__" << std::endl;
        _test_type<double>(test_name, success);
        std::cout << "TEST " << test_name << " PASSED!" << std::endl << std::endl;
    }
    template<class TYPE>
    void _generate_test(const std::string& test_name, const std::vector<TYPE>& in, const std::vector<TYPE>& expected) {
        file_serialize<TYPE>(test_name + ".in", in);
        file_serialize<TYPE>(test_name + ".exp", expected);
    }

    template<class TYPE>
    void _generate_random_vector(int length, std::vector<TYPE>& out) {
        std::vector<TYPE> in(length);
        std::random_device rnd_device;
        std::mt19937 mersenne_engine {rnd_device()};
        std::uniform_int_distribution<int> dist {0, 1};
        auto gen = [&dist, &mersenne_engine]() {
            return dist(mersenne_engine);
        };
        std::generate(begin(out), end(out), gen);
    }

    template<class TYPE>
    void _conditionally_generate_successful_test(int length, int open_number) {
        std::set<int> done_tests;
        for (const auto & entry : std::filesystem::directory_iterator(BASE_PATH)) {
            auto str = entry.path().string();
            int last_dot = str.find_last_of(".");
            int last_underscore = str.find_last_of("_");
            bool expected_success = (std::string::npos != str.find("success"));
            if (expected_success) {
                std::string s = str.substr(last_underscore + 1, last_dot);
                done_tests.insert(std::stoi(s));
            }
        }
        for(auto s: done_tests) {
            std::cout << s << std::endl;
        }
        if(done_tests.cend() == done_tests.find(open_number)) {
        std::vector<TYPE> in(length);
            _generate_random_vector(length, in);
            std::vector<TYPE> expected(length);

            // Trust our naive method for generating longer tests
            naive_exclusive_scan::exclusive_scan(in.data(), expected.data(), in.size());
            std::string test_name = BASE_PATH + std::string("test_success_") + std::to_string(open_number);
            _generate_test<TYPE>(test_name, in, expected);
        }
    }

    void test_all() {
        _conditionally_generate_successful_test<int>(1024 * 1024, 2);
        for (const auto & entry : std::filesystem::directory_iterator(BASE_PATH)) {
            auto str = entry.path().string();
            int last_dot = str.find_last_of(".");
            if (str.substr(last_dot + 1, str.size()) == "in") {
                auto test_name = str.substr(0, last_dot);
                bool expected_success = (std::string::npos != test_name.find("success"));
                std::cout << test_name << std::endl;
                _test_all_types(test_name, expected_success);
            }
        }
    }

    template<class TYPE>
    void chronometer_type(int size) {
        std::vector<TYPE> in(size);
        _generate_random_vector(size, in);
        std::vector<TYPE> expected(size);

        auto t_start = std::chrono::high_resolution_clock::now();
        for(int i=0, n = 32 * 1024 * 1024 / size; i < n; ++i) {
            naive_exclusive_scan::exclusive_scan(in.data(), expected.data(), in.size());
        }
        auto t_end = std::chrono::high_resolution_clock::now();
        std::cout << "|" << std::setw(10) << std::fixed << std::setprecision(2) << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " |";

        t_start = std::chrono::high_resolution_clock::now();
        for(int i=0, n = 32 * 1024 * 1024 / size; i < n; ++i) {
            avx_exclusive_scan::exclusive_scan(in.data(), expected.data(), in.size());
        }
        t_end = std::chrono::high_resolution_clock::now();
        std::cout << std::setw(10) << std::fixed << std::setprecision(2) << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " |";

        t_start = std::chrono::high_resolution_clock::now();
        for(int i=0, n = 32 * 1024 * 1024 / size; i < n; ++i) {
            cuda_exclusive_scan::exclusive_scan(in.data(), expected.data(), in.size());
        }
        t_end = std::chrono::high_resolution_clock::now();
        std::cout << std::setw(10) << std::fixed << std::setprecision(2) << std::chrono::duration<double, std::milli>(t_end-t_start).count() << " |" << std::endl;
    }

    void chronometer() {
        std::cout << "-------------------------------------" << std::endl;
        std::cout << "|" << std::setw(10) << "naive" << " |" << std::setw(10) << "avx" << " |" << std::setw(10) << "cuda" << " |" << std::endl;
        std::cout << "-------------------------------------" << std::endl;
        for (int size = 32; size <= 32 * 1024 * 1024; size *= 32) {
            chronometer_type<int8_t>(size);
            chronometer_type<int16_t>(size);
            chronometer_type<int32_t>(size);
            chronometer_type<int64_t>(size);
            chronometer_type<float>(size);
            chronometer_type<double>(size);
        }
        std::cout << "-------------------------------------" << std::endl;
    }

}
