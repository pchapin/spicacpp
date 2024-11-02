/*! \file    winexcept.hpp
 *  \brief   Helper classes for exception throwing Windows programs.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * This file contains a number of classes that are useful for preventing resource leaks in
 * Windows programs that throw exceptions.
 */

#ifndef WINEXCEPT_H
#define WINEXCEPT_H

#include <stdexcept>
#include <string>

#include <windows.h>

namespace spica {
    namespace Windows {
  
        //
        // class API_Error
        //
        // This class is a general Windows exception class. It is intended only for reporting
        // errors in Windows API functions. The constructor calls GetLastError() and so you
        // should only throw this exception after a function that sets GetLastError()'s return
        // in a meaningful way. This class is derived from the standard library's runtime_error
        // class to facilitate consistent error handling in applications.
        //
        class APIError : public std::runtime_error {
        public:
            // The message M is intended to make sense to the user.
            explicit APIError( const std::string &M ) noexcept : std::runtime_error( M )
                { raw_error = GetLastError( ); }

            // Returns the 16 bit error code associated with this error.
            DWORD error_code( ) const noexcept
                { return raw_error & 0x0000FFFF; }

            // Returns the 12 bit facility code associated with this error.
            DWORD facility_code( ) const noexcept
                { return ( raw_error & 0x0FFF0000 ) >> 16; }

            // Returns true if this is an application-defined error.
            bool application_defined( ) const noexcept
                { return ( raw_error & 0x20000000 ) ? true : false; }

            // Returns the severity of this error as reported by GetLastError().
            //   0 => Success, 1 => Informational, 2 => Warning, 3 => Error.
            int severity_level( ) const noexcept
                { return ( raw_error & 0xC0000000 ) >> 30; }

        private:
            DWORD  raw_error;
        };


        //
        // class Handle
        //
        // This class abstracts the concept of a handle and insures that the handle will get
        // properly closed even when an exception is thrown.
        //
        class Handle {
        public:
            Handle( ) :          the_handle( INVALID_HANDLE_VALUE ) { }
            Handle( HANDLE h ) : the_handle( h )                    { }
            operator HANDLE( ) const   { return the_handle; }
            void operator=( HANDLE h ) { the_handle = h;    }

            ~Handle( )
                { if( the_handle != INVALID_HANDLE_VALUE ) CloseHandle( the_handle ); }

            // Make copying illegal.
            Handle( const Handle & ) = delete;
            Handle &operator=( const Handle & ) = delete;

        protected:
            // Give access to derived classes so that specialized versions of Handle can easily
            // be created for various kernel object types.
            //
            HANDLE the_handle;
        };


        //
        // class CriticalGrabber
        //
        // This class simplifies the task of grabbing a CRITICAL_SECTION object. It insures that
        // the CRITICAL_SECTION will be properly released if an exception is thrown. This class
        // is obsolete. Applications should use MutexSem objects from my semaphore library
        // instead. Such objects offer a greater degree of cross platform support than this
        // class.
        //
        class CriticalGrabber {
        public:
            CriticalGrabber( CRITICAL_SECTION *p ) : cs( p ) { EnterCriticalSection( cs ); }
            ~CriticalGrabber( ) { LeaveCriticalSection( cs ); }

            // Make copying illegal.
            CriticalGrabber( const CriticalGrabber & ) = delete;
            CriticalGrabber &operator=( const CriticalGrabber & ) = delete;

        private:
            CRITICAL_SECTION *cs;
        };


        //
        // class PaintContext
        //
        // This class insures that device contexts obtained with BeginPaint() are properly
        // released with EndPaint() even when an exception is thrown.
        //
        class PaintContext {
        public:
            PaintContext( HWND Handle ) : window_handle( Handle )
                { context_handle = BeginPaint( window_handle, &paint_info ); }

            operator HDC( ) const
                { return context_handle; }

            ~PaintContext( )
                { EndPaint( window_handle, &paint_info ); }

            // Make copying illegal.
            PaintContext( const PaintContext & ) = delete;
            PaintContext &operator=( const PaintContext & ) = delete;

        private:
            PAINTSTRUCT paint_info;
            HWND        window_handle;
            HDC         context_handle;
        };


        //
        // class DeviceContext
        //
        // This class insures that device contexts obtained with GetDC() get properly released
        // with ReleaseDC() even when an exception is thrown.
        //
        class DeviceContext {
        public:
            DeviceContext( HWND Handle ) : window_handle( Handle )
                { context_handle = GetDC( window_handle ); }

            operator HDC( ) const
                { return context_handle; }

            ~DeviceContext( )
                { ReleaseDC( window_handle, context_handle ); }

            // Make copying illegal.
            DeviceContext( const DeviceContext & ) = delete;
            DeviceContext &operator=( const DeviceContext & ) = delete;

        private:
            HWND   window_handle;
            HDC    context_handle;
        };

    }
}

#endif
