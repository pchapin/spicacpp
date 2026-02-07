/*! \file    RegistryKey.cpp
 *  \brief   Implementation of a simple registry key object.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <string>
#include <windows.h>
#include <spicacpp/RegistryKey.hpp>

namespace spica {
    namespace Windows {

        RegistryKey::RegistryKey( HKEY top_level_key, const char *key_name ) noexcept
        {
            error = true;

            DWORD disposition;
            LONG  open_result =
                RegCreateKeyEx(
                    top_level_key,           // One of the special top level key names.
                    key_name,                // Name of subkey to open or create.
                    0,                       // Reserved. Must be zero.
                    nullptr,                 // Name of key class. Ignored if key exists.
                    REG_OPTION_NON_VOLATILE, // Save this key between reboots.
                    KEY_ALL_ACCESS,          // Want full access.
                    0,                       // Security attributes.
                   &key_handle,              // Resulting key handle.
                   &disposition              // Resulting flag (open or create).
                );

            // If an error occurs, the other calls will silently not work. This should be fixed
            // eventually. It would be better to have some sort of error reporting. Notice that
            // the registry functions don't seem to work with GetLastError( ).
            //
            if( open_result == ERROR_SUCCESS ) error = false;
        }


        RegistryKey::~RegistryKey( )
        {
            if( !error ) RegCloseKey( key_handle );
        }


        void RegistryKey::set_value( const char *name, const std::string &value )
        {
            if( !error ) {
                RegSetValueEx(
                    key_handle,               // Handle of the key.
                    name,                     // Name of the value.
                    0,                        // Reserved. Must be zero.
                    REG_SZ,                   // This value has type "string."
                    reinterpret_cast<CONST BYTE *>( value.c_str( ) ),
                    value.size()+1            // Number of bytes.
                ); 
            }
        }


        bool RegistryKey::get_value( const char *name, std::string &value )
        {
            // First let's be sure the key was opened successfully. If not, we can't get a value.
            if( error ) return false;

            char  buffer[128+1];
            DWORD type;
            DWORD buffer_size = 128+1;

            // Call the API function. Notice that we don't care about the type. We assume that
            // it is a string. This is probably a bad idea.
            //
            if( RegQueryValueEx(
                     key_handle,            // Handle of key to query.
                     name,                  // Name of the value.
                     0,                     // Reserved. Must be NULL.
                    &type,                  // The value's type.
                     reinterpret_cast<LPBYTE>( buffer ),
                    &buffer_size            // Size of buffer. Modified by call.
                ) == ERROR_SUCCESS ) {
                value = buffer;
                return true;
            }

            return false;
        }

        void RegistryKey::set_value( const char *name, DWORD value )
        {
            if( !error ) {
                RegSetValueEx(
                    key_handle,               // Handle of the key.
                    name,                     // Name of the value.
                    0,                        // Reserved. Must be zero.
                    REG_DWORD,                // This value has type "DWORD."
                    reinterpret_cast<CONST BYTE *>( &value ),
                    sizeof(DWORD)             // Number of bytes.
                );
            }
        }

        bool RegistryKey::get_value( const char *name, DWORD &value )
        {
            // First let's be sure the key was opened successfully. If not, we didn't get a value.
            if( error ) return false;

            DWORD buffer;
            DWORD type;
            DWORD buffer_size = sizeof(DWORD);

            // Call the API function. Notice that we don't care about the type. We assume that
            // it is a DWORD. This is probably a bad idea.
            //
            if( RegQueryValueEx(
                    key_handle,            // Handle of key to query.
                    name,                  // Name of value.
                    0,                     // Reserved. Must be NULL.
                   &type,                  // Value's type.
                    reinterpret_cast<LPBYTE>( &buffer ),
                   &buffer_size            // Size of buffer.
                ) == ERROR_SUCCESS ) {
                value = buffer;
                return true;
            }
            return false;
        }
    }
}
