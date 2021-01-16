#include <iostream>
#include <iterator>
#include <fstream>
#include <array>

namespace exclusive_scan_test {

    template<class TYPE, int MAX_SIZE>
    int file_serialize(const std::string& file_name, const std::array<TYPE, MAX_SIZE>& out) {
        std::string full_path = file_name;
        std::cout << std::string("=>") << full_path;
        std::ofstream os(full_path);
        std::ostream_iterator<double> start(os, " ");
        for (TYPE t : out) {
            *start++ = t;
        }
        std::cout << ": wrote " << out.size() << " numbers" << std::endl;
        return out.size();
    };

    template<class TYPE, int MAX_SIZE>
    int file_deserialize(const std::string& file_name, std::array<TYPE, MAX_SIZE>& in) {
        std::string full_path = file_name;
        std::cout << std::string("<=") << full_path;
        std::ifstream is(full_path);
        std::istream_iterator<double> start(is), end;
        in = std::array<TYPE, MAX_SIZE>();
        for (auto it = in.begin(); start != end; ++start, ++it) {
            *it = *start;
        }
        std::cout << ": read " << in.size() << " numbers" << std::endl;
        return in.size();
    };

    template<class TYPE, int MAX_SIZE>
    void print(const std::array<TYPE, MAX_SIZE>& out) {
        std::cout << int(out[0]);
        for (int i = 1; i < out.size(); ++i) {
            std::cout << " " << double(out[i]);
        }
        std::cout << "\n";
    }
}
