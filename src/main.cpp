#include <thread>
#include "test/test.hpp"

int main(int argc, char** argv) {
    exclusive_scan_test::test_all();
    exclusive_scan_test::chronometer();
}
