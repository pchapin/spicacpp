/*! \file    str_tests.cpp
 *  \brief   Exercise spica::String.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * This program exercises the multithreaded support of spica::String. The testing done here is
 * certainly not exhaustive but it is a start. Hopefully it is better than nothing.
 */

#include "environ.hpp"

#include <iostream>

#include <process.h>
#include <windows.h>
#include "str.hpp"

spica::String *A;
spica::String *B;

unsigned __stdcall assigner( void * )
{
    while( true ) {
        *A = *B;
    }
    return 0;
}

unsigned __stdcall appender( void * )
{
    while( true ) {
        A->append("x");
    }
    return 0;
}

int str_testsMT( )
{
    unsigned ass_thread;
    unsigned app_thread;

    A = new spica::String;
    B = new spica::String;
    *B = "Hello!";

    uintptr_t ass_handle = _beginthreadex( NULL, 0, assigner, NULL, 0, &ass_thread );
    if( ass_handle == 0 ) {
        std::cout << "Unable to start the assignment thread.\n";
        return 1;
    }

    uintptr_t app_handle = _beginthreadex( NULL, 0, appender, NULL, 0, &app_thread );
    if( app_handle == 0 ) {
        std::cout << "Unable to start the append thread.\n";
        // Kill the other thread?
        return 1;
    }

    WaitForSingleObject( reinterpret_cast<HANDLE>(ass_handle), INFINITE );
    WaitForSingleObject( reinterpret_cast<HANDLE>(app_handle), INFINITE );

    delete A;
    delete B;
    return 0;
}
