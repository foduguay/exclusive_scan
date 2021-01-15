#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>

namespace exclusive_scan_test {

    template<class TYPE>
    bool file_serialize(const std::string& file_name, const std::vector<TYPE>& out) {
        // TODO check if path exists
        std::string full_path = file_name;
        std::cout << std::string("=>") << full_path;
        std::ofstream os(full_path);
        std::ostream_iterator<double> start(os, " ");
        for (TYPE t : out) {
            *start++ = t;
        }
        std::cout << ": wrote " << out.size() << " numbers" << std::endl;
        return true;
    };

    template<class TYPE>
    bool file_deserialize(const std::string& file_name, std::vector<TYPE>& in) {
        // TODO check if file exists
        std::string full_path = file_name;
        std::cout << std::string("<=") << full_path;
        std::ifstream is(full_path);
        std::istream_iterator<double> start(is), end;
        in = std::vector<TYPE>(start, end);
        std::cout << ": read " << in.size() << " numbers" << std::endl;
        return true;
    };

    template<class TYPE>
    void print(const std::vector<TYPE>& out) {
        std::cout << int(out[0]);
        for (int i = 1; i < out.size(); ++i) {
            std::cout << " " << double(out[i]);
        }
        std::cout << "\n";
    }
}
