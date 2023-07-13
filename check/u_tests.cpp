/*! \file    u_tests.cpp
 *  \brief   Main unit test driver program for the Spica C++ library.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstdlib>
#include <iostream>
#include <fstream>

#include "u_tests.hpp"
#include "UnitTestManager.hpp"

using namespace std;

int main( int argc, char **argv )
{
    ostream *output = &std::cout;
    ofstream output_file;

    if( argc == 2 ) {
        output_file.open( argv[1] );
        if( !output_file ) {
            cerr << "Unable to open " << argv[1] << " for output!\n";
            return EXIT_FAILURE;
        }
        output = &output_file;
    }

    UnitTestManager::register_suite( BinomialHeap_tests, "BinomialHeap Tests" );
    UnitTestManager::register_suite( BoundedList_tests, "BoundedList Tests" );
    UnitTestManager::register_suite( Graph_tests, "Graph Tests" );
    UnitTestManager::register_suite( sort_tests, "Sorting Algorithms" );
    UnitTestManager::register_suite( VeryLong_tests, "VeryLong Tests" );

    // TODO: The following tests are interactive, which is not ideal. They're better than nothing.
    UnitTestManager::register_suite( str_tests, "String Tests" );
    UnitTestManager::register_suite( Timer_tests, "Timer Tests" );

    UnitTestManager::execute_suites( *output, "Spica C++ Tests" );
    return UnitTestManager::test_status( );
}

