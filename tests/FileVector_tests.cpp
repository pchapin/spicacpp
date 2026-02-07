/*! \file    FileVector_tests.cpp
 *  \brief   Exercise spica::FileVector.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include "environ.hpp"

#include "FileVector.hpp"
#include <algorithm>
#include <iostream>

//
// Function to test operator[]( )
//
static void access_test()
{
    int i;

    std::cout << "Testing operator[]( )...";
    spica::FileVector<int> my_file("test.dat", 1024);
    for (i = 0; i < 1024; ++i) {
        my_file[i] = i;
    }

    for (i = 0; i < 1024; ++i) {
        if (my_file[i] != i) {
            std::cout << "FAILED!\n";
            break;
        }
    }
    if (i == 1024)
        std::cout << "passed\n";
}

//
// Function to test push_back( )
//
static void push_back_test()
{
    int i;

    std::cout << "Testing push_back( )...";
    spica::FileVector<int> my_file("test.dat");
    if (my_file.size() != 1024) {
        std::cout << "FAILED! size( ) not as expected!\n";
        return;
    }

    for (i = 0; i < 1024; ++i) {
        my_file.push_back(2 * i);
    }

    for (i = 1024; i < 2048; ++i) {
        if (my_file[i] != 2 * (i - 1024)) {
            std::cout << "FAILED!\n";
            break;
        }
    }
    if (i == 2048)
        std::cout << "passed\n";
}

int main(int, char**)
{
    try {
        access_test();
        push_back_test();
    }
    catch (std::bad_alloc) {
        std::cout << "Out of memory!" << "\n";
    }
    return 0;
}
