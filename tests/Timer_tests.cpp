/*! \file    BoundedList_tests.cpp
 *  \brief   Exercise spica::Timer.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include "../environ.hpp"

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

#include "../Timer.hpp"
#include "../u_tests.hpp"
#include "../UnitTestManager.hpp"

namespace {

    const int delta = 50;  // Allowable error in milliseconds.

    //
    // static void do_sleep( int )
    //
    // This function encapsulates the system's sleep operation.
    //
    void do_sleep( int seconds )
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


    void test_constructor( )
    {
        UnitTestManager::UnitTest test( "constructor" );

        spica::Timer object_1;

        // Nothing should happen.
        do_sleep( 3 );
        UNIT_CHECK( object_1.time( ) == 0 );
    }


    void test_basic_usage( )
    {
        UnitTestManager::UnitTest test( "basic_usage" );

        spica::Timer object_2;

        object_2.start( );
        do_sleep( 3 );
        object_2.stop( );
        UNIT_CHECK( object_2.time( ) >= 3000 );
        UNIT_CHECK( object_2.time( ) <= 3000 + delta );

        // The timer should not accumulate time while stopped.
        do_sleep( 5 );
        UNIT_CHECK( object_2.time( ) >= 3000 );
        UNIT_CHECK( object_2.time( ) <= 3000 + delta );

        // Restart the timer and check again. It should accumulate additional time.
        object_2.start( );
        do_sleep( 3 );
        object_2.stop( );
        UNIT_CHECK( object_2.time( ) >= 6000 );
        UNIT_CHECK( object_2.time( ) <= 6000 + delta );
    }


    void test_miscellaneous( )
    {
        UnitTestManager::UnitTest test( "miscellaneous" );

        spica::Timer object_3;
    
        object_3.start( );
        do_sleep( 5 );

        // Checking the accumulated time while running should be correct.
        UNIT_CHECK( object_3.time( ) >= 5000 );
        UNIT_CHECK( object_3.time( ) <= 5000 + delta );

        // Resetting the timer should clear the accumulated time and stop the timer.
        object_3.reset( );
        UNIT_CHECK( object_3.time( ) == 0 );

        // Verify that the timer is not still running.
        do_sleep( 3 );
        UNIT_CHECK( object_3.time( ) == 0 );
    }
}

bool Timer_tests( )
{
    test_constructor( );
    test_basic_usage( );
    test_miscellaneous( );
    return true;
}
