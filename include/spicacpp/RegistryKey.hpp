/*! \file    RegistryKey.hpp
 *  \brief   Interface to a simple registry key object.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * This class provides a simple interface to the registry. Construction opens the key specified
 * to the constructor while destruction closes the key. When a key is open, new values can be
 * added and existing values can be queried. This class currently only supports string and DWORD
 * values.
 *
 * The constructor takes two parameters. The first is intended to be one of the special symbols
 * HKEY_CURRENT_USER or HKEY_LOCAL_MACHINE. The second parameter is the path to the desired key
 * *without* a leading backslash. The constructor will create the key (and required parent keys)
 * if it does not exist.
 *
 * Error reporting is minimal (non-existant). This is an important area where this class could be
 * improved.
 */

#ifndef REGISTRYKEY_HPP
#define REGISTRYKEY_HPP

#include <spicacpp/environ.hpp>

#if !defined(eWINDOWS)
#error RegistryKey only supports the Windows operating system!
#endif

#include <string>
#include <windows.h>

namespace spica {
    namespace Windows {

        class RegistryKey {
          private:
            HKEY key_handle; // Holds the handle to the key.
            bool error;      // =true if we couldn't create (or open) the key.

          public:
            /*!
             * Open or create the specified registry key. This function tries to set as many
             * things as possible to default values in order to make life simple for the caller.
             *
             * \param top_level_key Either HKEY_CURRENT_USER or HKEY_LOCAL_MACHINE.
             * \param key_name The path, relative to the top level, to the key of interest.
             */
            RegistryKey(HKEY top_level_key, const char* key_name) noexcept;

            /*!
             * Close the key if it was opened successfully.
             */
            ~RegistryKey();

            /*!
             * Add the (name, value) pair to the key. This function only allows string values.
             *
             * \param name The name of the value.
             * \param value The value of the value.
             */
            void set_value(const char* name, const std::string& value);

            /*!
             * Retrieve the value associated with the given name. This function only retrieves
             * string values.
             *
             * \param name The name of the value.
             * \param value A reference to a std::string to receive the value of the value.
             * \return true if successful; false otherwise.
             */
            bool get_value(const char* name, std::string& value);

            /*!
             * Add the (name, value) pair to the key. This function only allows DWORD values.
             *
             * \param name The name of the value.
             * \param value The value of the value.
             */
            void set_value(const char* name, DWORD value);

            /*!
             * Retrieve the value associated with the given name. This function only retrieves
             * DWORD values.
             *
             * \param name The name of the value.
             * \param value A reference to a DWORD to receive the value of the value.
             * \return true if successful; false otherwise.
             */
            bool get_value(const char* name, DWORD& value);
        };
    } // namespace Windows
} // namespace spica

#endif
