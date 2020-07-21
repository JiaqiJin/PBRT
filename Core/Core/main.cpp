#include <iostream>
#include "core/kawaii.hpp"
#include "util/macro.h"

USING_KAWAII

int main(int argc, const char* argv[]) {
    // insert code here...
    std::cout << "Hello, kawaii!\n";
    KawaiiDesune kawakawa;
    Point3f p; 
    return kawakawa.run(argc, argv);
}
