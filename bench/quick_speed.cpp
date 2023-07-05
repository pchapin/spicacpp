/*! \file    quick_speed.cpp
 *  \brief   Measures the performance of quick sort.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * This file contains a program that measures the performance of bubble sort for various
 * sequence lengths. It is intended to demonstrate the O(n lg(n)) behavior of the algorithm.
 */

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include "sorters.hpp"
#include "Timer.hpp"

// A couple of useful constants.
const int K = 1024;
const int M = K*K;

//
// Main program just exercises each test.
//
int main( )
{
  int sequence_size;

  // We don't need to seed the random number generator randomly.
  std::srand( 0 );

  std::cout << std::setiosflags( std::ios::fixed );

  // Do the tests.
  for( sequence_size = 256; sequence_size <= 16*M; sequence_size *= 2 ) {
    spica::Timer stopwatch;

    // Allocate our test array and fill it with random data.
    int *p = new int[sequence_size];
    for( int i = 0; i < sequence_size; ++i ) p[i] = std::rand( );

    // Sort the sequence, timing it as we do so.
    stopwatch.start( );
    spica::quick_sort( p, p + sequence_size );
    stopwatch.stop( );

    // What is the result?
    std::cout <<   "Size = " << std::setw( 9 ) << sequence_size 
              << "; Time = " << std::setw( 9 ) << std::setprecision( 3 )
              << stopwatch.time()/1000.0 << "s" << std::endl;

    delete [] p;
  }

  return 0;
}

