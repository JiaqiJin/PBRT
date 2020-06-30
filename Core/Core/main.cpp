#include <iostream>
#include "core/kawaii.hpp"
#include "util/macro.hpp"

int main(int argc, const char* argv[]) {
    // insert code here...
    std::cout << "Hello, paladin!\n";
    Kawaii::KawaiiDesune kawakawa;
    return kawakawa.run(argc, argv);
}
