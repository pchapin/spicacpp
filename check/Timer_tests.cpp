/*! \file    BoundedList_tests.cpp
 *  \brief   Exercise spica::Timer.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include "environ.hpp"

#include <iostream>

#if eOPSYS == eMSDOS
#include <dos.h>
#endif

#if eOPSYS == eWIN32
#include <windows.h>
#endif

#if eOPSYS == ePOSIX
#include <unistd.h>
#endif

#include "Timer.hpp"

//
// static void do_sleep( int )
//
// This function encapsulates the system's sleep operation.
//
static void do_sleep( int seconds )
{
    #if eOPSYS == eMSDOS
    sleep( seconds );
    #endif

    #if eOPSYS == eWIN32
    Sleep( seconds * 1000 );
    #endif

    #if eOPSYS == ePOSIX
    sleep( seconds );
    #endif
}


//
// void test_1( )
//
// This test insures that Timers are constructed properly.
//
static void test_1( )
{
    spica::Timer object_1;

    // Sleep for a while. If the timer is running (shouldn't be) let's give it a chance to
    // accumulate some time.
    //
    std::cout << "Test #1: Sleeping for 5 seconds..." << std::endl;
    do_sleep( 5 );

    // It should have just sat there.
    std::cout << "         object_1's time is: " << object_1.time( )
              << "ms (should be zero)" << std::endl;
}


//
// void test_2( )
//
// This test checks basic usage. Can we start and stop a Timer more than once and properly
// accumulate time?
//
static void test_2( )
{
    spica::Timer object_2;

    object_2.start( );
    std::cout << "Test #2: object_2 is running. I will sleep for 5 seconds... " << std::endl;
    do_sleep( 5 );
    object_2.stop( );

    std::cout << "         object_2's time is: " << object_2.time( )
              << "ms (should be approximately 5000)" << std::endl;
    std::cout << "         I have stopped object_2. I will sleep for 5 more seconds..." << std::endl;
    do_sleep( 5 );

    std::cout << "         object_2's time is: " << object_2.time( )
              << "ms (should be the same as above)" << std::endl;

    object_2.start( );
    std::coout << "         I have started object_2 again. I will sleep some more..." << std::endl;
    do_sleep( 5 );
    object_2.stop( );
    
    std::cout << "         object_2's time is: " << object_2.time( )
              << "ms (should be five more seconds)" << std::endl;
}


//
// void test_3( )
//
// This test exercises a few oddball things.
//
static void test_3( )
{
    spica::Timer object_3;
  
    object_3.start( );
    std::cout << "Test #3: Running object_3 for 10 seconds..." << std::endl;
    do_sleep( 10 );

    std::cout << "         object_3's time is: " << object_3.time( )
              << "ms. Now resetting while it's running..." << std::endl;
    object_3.reset( );
    std::cout << "         object_3's time is: " << object_3.time( )
              << "ms. Now sleeping for 5 seconds..." << std::endl;
    do_sleep( 5 );

    std::cout << "         object_3's time is: " << object_3.time( )
              << "ms. (should be the same as above)" << std::endl;
}


//
// The Main function just runs the tests above.
//
bool Timer_tests( )
{
    std::cout << "Class Timer test program (2006-02-10)\n" << std::endl;

    test_1( );
    test_2( );
    test_3( );
    return true;
}
