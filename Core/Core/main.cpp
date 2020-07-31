#include <iostream>
#include "core/kawaii.hpp"
#include "util/macro.h"

USING_KAWAII

USING_STD

int main(int argc, const char* argv[]) {
    // insert code here...
    std::cout << "Hello, kawaii!\n";
    KawaiiDesune kawakawa;

    uint32_t a = 2;
    cout << bitsToFloat(a) << endl;

    /*for (int i = 0; i < 5; ++i) {
        cout << p.block(i) << "  b" << i << endl;
        cout << p.offset(i) << "  o" << i << endl;
    }
    p.getTotalOffset(6, 6);
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            cout << "getTotalOffset(" << i << "," << j << ") = " << p.getTotalOffset(i, j) << endl;
        }
    }*/

    return kawakawa.run(argc, argv);
}
