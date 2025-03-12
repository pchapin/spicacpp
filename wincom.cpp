/*! \file    wincom.cpp
 *  \brief   Implementation of a serial port class for Windows.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <stdexcept>
#include <string>

#include <process.h>
#define NOMINMAX
#include <windows.h>

#include "wincom.hpp"
#include "windebug.hpp"
#include "winexcept.hpp"

namespace spica {
    namespace Windows {

        namespace {

            //-------------------------------------------------
            //           Internally Linked Functions
            //-------------------------------------------------

            // This function is for debugging purposes. It displays "interesting" fields of the
            // given DCB into the debugging window.
            //
            void display_DCB( const char *header, const DCB &the_DCB )
            {
                debugstream message;

                message << header << ": ";
                message
                    << "Baud="     << the_DCB.BaudRate                               << ' '
                    << "CTSFlow="  << ((the_DCB.fOutxCtsFlow == TRUE) ? "ON" : "OFF")<< ' '
                    << "RTSFlow="  <<
                    ((the_DCB.fRtsControl == RTS_CONTROL_HANDSHAKE) ? "ON" : "OFF")<< ' '
                    << "Parity="   << ((the_DCB.fParity == TRUE) ? "ON" : "OFF")     << ' '
                    << "DataBits=" << the_DCB.ByteSize                               << ' '
                    << "StopBits=";

                const char *stop_message = "?";
                switch( the_DCB.StopBits ) {
                case ONESTOPBIT  : stop_message = "1";   break;
                case ONE5STOPBITS: stop_message = "1.5"; break;
                case TWOSTOPBITS : stop_message = "2";   break;
                }
                message << stop_message;
                message.say( );
            }


            // This function is for debugging purposes. It displays "interesting" fields of the
            // given COMMTIMEOUTS into the debugging window.
            //
            void display_COMMTIMEOUTS( const char *header, const COMMTIMEOUTS &the_TIMEOUTS )
            {
                debugstream message;

                message << header << ":";
                message << " ReadIntervalTimeout="
                        << the_TIMEOUTS.ReadIntervalTimeout
                        << " WriteTotalTimeoutConstant="
                        << the_TIMEOUTS.WriteTotalTimeoutConstant
                        << " WriteTotalTimeoutMultiplier="
                        << the_TIMEOUTS.WriteTotalTimeoutMultiplier;
                message.say( );
            }
            
        }


        //-------------------------------------------------
        //           Externally Linked Functions
        //-------------------------------------------------

        /*!
         * This function is executed by the port's helper thread. It waits for an incoming
         * character and then calls the port's reader function for each character that arrives.
         * If there are many port objects in the program, then there will be many different
         * threads in this function (all with different port_object pointers). This function
         * must be externally linked because it is declared as a friend in the class definition.
         */
        unsigned int __stdcall comPort_reader( void *port_object )
        {
            const int BUFFER_SIZE = 256;

            DWORD event;          // The event that occured on the port.
            BOOL  wait_result;    // Did WaitCommEvent() work?
            BOOL  read_result;    // Did ReadFile() work?
            char  the_bytes[BUFFER_SIZE];  // Data from the port.
            DWORD byte_count;     // The number of bytes read from the port.

            // The parameter is really a pointer to a Com_Port object.
            ComPort *the_port = reinterpret_cast<ComPort *>(port_object);

            // Just loop forever.
            while( true ) {

                try {

                    if( the_port->testing_mode ) {
                        std::string simulated_input;
                        const char *scanner;

                        // Wait for some text.
                        the_port->input_buffer.pop( simulated_input );
                        scanner = simulated_input.c_str( );

                        // Send the text through the user's read processor.
                        while( *scanner ) {
                            the_port->read_processor( *scanner );
                            scanner++;
                        }

                        // After processing all characters from that batch, send a null
                        // character to inform the read processor that there are no more for
                        // now.
                        //
                        the_port->read_processor( '\0' );
                    }

                    else { // We are not debugging. Manipulate the real serial port.
                        OVERLAPPED overlapped;

                        // Set up the overlapped structure.
                        std::memset( &overlapped, 0, sizeof(overlapped) );
                        ResetEvent( the_port->read_event );
                        overlapped.hEvent = the_port->read_event;

                        // Wait on the event.
                        wait_result = WaitCommEvent( the_port->the_handle, &event, &overlapped );

                        // Is there a problem (other than having the I/O pending)?
                        if( wait_result == FALSE && GetLastError( ) != ERROR_IO_PENDING )
                            throw APIError( "Error waiting on the serial port" );

                        // If no problem, wait for the result.
                        else {
                            GetOverlappedResult( the_port->the_handle, &overlapped, &byte_count, TRUE );
                            if( event != EV_RXCHAR ) continue;
                        }

                        // Set up the overlapped structure (again!).
                        std::memset( &overlapped, 0, sizeof(overlapped) );
                        ResetEvent( the_port->read_event );
                        overlapped.hEvent = the_port->read_event;

                        read_result = ReadFile( the_port->the_handle,
                                                the_bytes, BUFFER_SIZE, &byte_count, &overlapped );

                        // Is there a problem (other than having the I/O pending)?
                        if( read_result == FALSE && GetLastError( ) != ERROR_IO_PENDING )
                            throw APIError( "Can't read data from serial port" );

                        // If no problem, wait for the result.
                        else {
                            GetOverlappedResult( the_port->the_handle, &overlapped, &byte_count, TRUE );
                        }

                        // Send the bytes through the read processor.
                        int Counter = 0;
                        while( byte_count > 0 ) {
                            the_port->read_processor( the_bytes[Counter++] );
                            byte_count--;
                        }

                        // After processing all characters from that batch, send a null
                        // character to inform the read processor that there are no more for
                        // now.
                        //
                        the_port->read_processor( '\0' );
                    }
                }
                catch( std::bad_alloc ) {
                    notifystream message;
                    message << "Out of memory while processing input from port "
                            << the_port->name;
                    message.say( );
                }
                catch( APIError We ) {
                    notifystream message;        
                    message << "Unexpected error while processing input from port "
                            << the_port->name << "\r";
                    message << We.what( );
                    message.say( );
                }
                catch( ... ) {
                    notifystream message;
                    message << "Unhandled exception while processing input from port "
                            << the_port->name;
                    message.say( );
                }
            }
            return 0;
        }


        //--------------------------------------
        //           Member Functions
        //--------------------------------------

        ComPort::ComPort( bool testing ) :
            name               ( 0 ),
            port_set           ( false ),
            outer_usage        ( false ),
            testing_mode       ( testing ),
            read_processor     ( 0 ),
            helperThread_handle( INVALID_HANDLE_VALUE ),
            input_buffer       ( 128 )
        {
            // Create event objects to be used during asynchronous I/O.
            read_event = CreateEvent( 0, TRUE, FALSE, 0 );
            write_event = CreateEvent( 0, TRUE, FALSE, 0 );
        }


        ComPort::~ComPort( )
        {
            outer_usage = true;

            // If the port has been configured, set it back to its original state.
            if( port_set ) {
                // Blow the helper thread away.
                stop_reading( );

                // Reset the serial port parameters.
                SetCommState( the_handle, &oldComm_state );

                // Reset the serial port timeouts.
                SetCommTimeouts( the_handle, &oldComm_timeouts );
            }

            // In any case, close the Event handles.
            CloseHandle( read_event );
            CloseHandle( write_event );
        }


        void ComPort::set( const char *given_name, int baud, void (*read)( char ) )
        {
            // For now, don't allow a CommPort object to be set more than once.
            if( port_set ) return;
            
            read_processor = read;

            // If I am in testing mode, just return at this point before playing with the port.
            // Note that I leave port_set as false.
            //
            if( testing_mode ) {
                debugstream message;
                message << "Set serial port " << given_name
                        << " in TESTING mode. All I/O is simulated";
                message.say( );
                return;
            }

            // Can I open the port?
            the_handle =
                CreateFile( given_name,                  // Name of port.
                            GENERIC_READ|GENERIC_WRITE,  // Read and write the port.
                            0,                           // Sharing mode.
                            0,                           // Security attributes.
                            OPEN_EXISTING,               // The port must exist.
                            FILE_FLAG_OVERLAPPED,        // Do asynchronous I/O.
                            0                            // No template file.
                            );
            if( the_handle == INVALID_HANDLE_VALUE )
                throw APIError( "Can't open serial port" );
            
            // We did. Now let's see what the parameters are.
            if( GetCommState( the_handle, &oldComm_state ) == FALSE )
                throw APIError( "Can't get serial port parameters" );

            if( GetCommTimeouts( the_handle, &oldComm_timeouts ) == FALSE )
                throw APIError( "Can't get serial port timeout settings" );

            // Display the old settings for debugging reference.
            display_DCB( "Initial serial port parameters", oldComm_state );
            display_COMMTIMEOUTS( "Initial serial port timeouts", oldComm_timeouts );

            // Set the parameters to what we want.
            newComm_state = oldComm_state;
            newComm_state.BaudRate = baud;
            newComm_state.ByteSize = 8;
            newComm_state.fParity  = FALSE;
            newComm_state.StopBits = ONESTOPBIT;
            newComm_state.fOutxCtsFlow = FALSE;
            // newComm_state.fOutxCtsFlow = TRUE;
            newComm_state.fRtsControl  = RTS_CONTROL_DISABLE;
            // newComm_state.fRtsControl  = RTS_CONTROL_HANDSHAKE;
            if( SetCommState(the_handle, &newComm_state) == FALSE )
                throw APIError( "Can't set serial port parameters" );
            
            // Let's see if it actually worked by checking the current state.
            DCB didState_work;
            if( GetCommState( the_handle, &didState_work ) == TRUE )
                display_DCB( "Set serial port parameters to", didState_work );

            // Set the timeouts to what we want.
            newComm_timeouts = oldComm_timeouts;
            newComm_timeouts.ReadIntervalTimeout         = 50;
            newComm_timeouts.WriteTotalTimeoutConstant   = 1000;
            newComm_timeouts.WriteTotalTimeoutMultiplier = 5;
            if( SetCommTimeouts( the_handle, &newComm_timeouts ) == FALSE )
                throw APIError( "Can't set serial port timeout settings" );

            // Let's see if it actually worked by checking the current timeouts.
            COMMTIMEOUTS didTimeouts_work;
            if( GetCommTimeouts( the_handle, &didTimeouts_work ) == TRUE )
                display_COMMTIMEOUTS( "Set serial port timeouts to", didTimeouts_work );
            
            // Set up the event mask for this port.
            if( SetCommMask( the_handle, EV_RXCHAR ) == FALSE )
                throw APIError( "Can't set the event mask for the serial port" );

            // Let's see if it actually worked by checking the current mask.
            DWORD didMask_work;
            if( GetCommMask( the_handle, &didMask_work ) == TRUE ) {
                debugstream message;
                message << "Set serial port event mask to " << didMask_work;
                message.say( );
            }
      
            name     = given_name;
            port_set = true;
        }


        void ComPort::start_reading( )
        {
            unsigned int thread_ID;
    
            if( ( port_set || testing_mode ) && helperThread_handle == INVALID_HANDLE_VALUE ) {
                helperThread_handle = reinterpret_cast<HANDLE>(
                    _beginthreadex( 0, 0, comPort_reader, this, 0, &thread_ID ) );
        
                debugstream message;
                message << "Serial port helper thread started";
                message.say( );
            }
        }


       void ComPort::write( const char *outgoing )
        {
            // Writes to the port are just ignored. (Port? What port?)
            if( testing_mode ) return;
            
            DWORD       bytes_written;
            BOOL        write_result;
            DWORD       string_length;
            OVERLAPPED  overlapped;

            // Check length of incoming string.
            const size_t incoming_length = strlen( outgoing );
            if( incoming_length > MAXDWORD )
                throw std::length_error( "Outgoing string too long to write to serial port" );
            string_length = static_cast<DWORD>( incoming_length );

            // Set up the overlapped structure.
            std::memset( &overlapped, 0, sizeof(overlapped) );
            ResetEvent( write_event );
            overlapped.hEvent = write_event;

            write_result = WriteFile( the_handle,
                                      outgoing, string_length, &bytes_written, &overlapped );

            // Is there a problem (other than having the I/O pending)?
            if( write_result == FALSE && GetLastError( ) != ERROR_IO_PENDING )
                throw APIError( "Can't write data to serial port" );

            // If the write was synchronous, did it transfer the desired material?
            else if( write_result == TRUE && bytes_written != string_length )
                throw APIError( "Can't write data to serial port" );

            // I am waiting for the asynchronous operation to complete.
            else {
                GetOverlappedResult( the_handle, &overlapped, &bytes_written, TRUE );
                if( bytes_written != string_length )
                    throw APIError( "Can't write data to serial port" );
            }
        }


        void ComPort::stop_reading( ) noexcept
        {
            if( helperThread_handle != INVALID_HANDLE_VALUE ) {
                TerminateThread( helperThread_handle, 0 );
                helperThread_handle = INVALID_HANDLE_VALUE;
                if( !outer_usage ) {
                    debugstream message;
                    message << "Serial port helper thread stopped";
                    message.say( );
                }
            }
        }


        void ComPort::unset( )
        {
            if( port_set ) {
                // Blow the helper thread away.
                stop_reading( );

                // Reset the serial port parameters.
                SetCommState( the_handle, &oldComm_state );

                // Reset the serial port timeouts.
                SetCommTimeouts( the_handle, &oldComm_timeouts );
            }
            
            // Close the underlying handle.
            if( the_handle != INVALID_HANDLE_VALUE ) CloseHandle( the_handle );

            // Set things back to their initial state.
            the_handle     = INVALID_HANDLE_VALUE;
            read_processor = 0;
            port_set       = false;
            
            if( name != 0 ) {
                debugstream message;
                message << "Serial port " << name << " closed";
                message.say( );
            }
            name = 0;
        }

    }
}
