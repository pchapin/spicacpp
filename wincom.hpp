/*! \file    wincom.hpp
 *  \brief   Interface to a serial port class for Windows.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef WINCOM_HPP
#define WINCOM_HPP

#include "environ.hpp"

#if !defined(eWINDOWS)
#error WinCom only supports the Windows operating system!
#endif

#if !defined(eMULTITHREADED)
#error WinCom requires multiple threads and multithread support not active!
#endif

#include <string>
#include "winexcept.hpp"
#include "WorkQueue.hpp"

namespace spica {
    namespace Windows {

        /*!
         * A wrapper around a serial communications port on a Windows machine.
         */
        class ComPort : public Handle {

            friend unsigned int __stdcall comPort_reader( void *port_object );

        private:
            const char   *name;             // The "file" name for this port.
            DCB           oldComm_state;    // Old settings of the port.
            DCB           newComm_state;    // Our settings.
            COMMTIMEOUTS  oldComm_timeouts; // Old timeouts on the port.
            COMMTIMEOUTS  newComm_timeouts; // Our timeouts.
            bool          port_set;         // =true if the port initialized.
            bool          outer_usage;      // =true if constructing or destroying.
            bool          testing_mode;     // =true when we simulate port input.
            void        (*read_processor)(char); // Function that handles input.
            HANDLE        helperThread_handle;   // Handle of reading thread.
            HANDLE        read_event;       // Event object used during reads.
            HANDLE        write_event;      // Event object used during writes.
            WorkQueue<std::string> input_buffer; // Used to hold simulated input.

        public:
            /*!
             * Open the COM port in an uninitialized state. The constructor initializes the
             * object's many members into something sensible. Note that the constructor does not
             * configure the port. That is done in the `set` function. This way port objects can
             * be constructed before the desired port parameters are known.
             *
             * \param testing If `true`, the port is put in "testing" mode. In this mode, all
             * input is simulated and no output is sent to the actual hardware port.
             */
            ComPort( bool testing = false );

            /*!
             * Close the COM port. The destructor stops the helper thread (if it's running) and
             * then resets the port parameters to the way they were before.
             *
             * Should the destructor also cancel any asynchronous I/O that might be pending (for
             * example before closing the event handles that I/O is trying to use)? Probably.
             */
           ~ComPort( );

            /*!
             * Set the parameters of the port and specify the function that will process each
             * character as it arrives. The processing of arriving characters is done by a
             * helper thread. This class configures the port to use hardware (RTS/CTS) flow
             * control. It also configures appropriate timeout intervals for both reading and
             * writing. These parameters are hard coded for now, but future versions of this
             * class might allow the user to specify more of these things.
             *
             * \param given_name The name of the serial port as understood by Windows.
             * \param baud The desired baud rate.
             * \param read A pointer to a function that processes incoming characters.
             */
            void set( const char *given_name, int baud, void (*read)( char ) );

            /*!
             * Define a line of simulated input. This is only useful if the port was constructed
             * in testing mode. In that case, any attempt to read the port will return the
             * simulated input.
             *
             * \param input The simulated input that will be returned via reading.
             */
            void set_input( const std::string &input )
                { input_buffer.push( input ); }

            /*!
             * Start the thread that reads the port. It is not an error to call this function
             * more than once. If the helper thread was already running, additional calls to
             * this function are ignored.
             */
            void start_reading( );

            /*!
             * Write data to this port. Data arriving at the port is handled by the read
             * processor function. This function will throw an exception if it can't write the
             * data as expected.
             *
             * \param outgoing The null-terminated string to be written to the port. The null
             * characater is not written.
             */
            void write( const char *outgoing );

            /*!
             * Kill the read processing thread if it was running. It is not an error to call
             * this function if the helper thread isn't running. In that case, the call is
             * ignored.
             *
             * This function should probably not resort to TerminateThread( ) as that is a
             * pretty gross way to kill a thread. (What if the helper thread is in the middle of
             * processing some input and holds some resources?). For now it will be
             * acceptable.
             */
            void stop_reading( ) noexcept;

            /*!
             * Return the port to the same settings it had before construction. This function
             * undoes `set`. It leaves the port in a state where `set` can be called again. It's
             * useful for changing port parameters on the fly.
             */
            void unset( );
        };

    }
}

#endif

