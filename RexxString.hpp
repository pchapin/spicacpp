/*! \file    RexxString.hpp
 *  \brief   Interface to a Rexx-like string class.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * Everyone has their own string class. This is mine. These strings are intended to mimic the
 * features of strings that exist in the Rexx programming language. Of particular note is that
 * string positions start at one, not zero as is more typical for C/C++ strings.
 */

#ifndef SPICA_REXXSTRING_HPP
#define SPICA_REXXSTRING_HPP

#include "environ.hpp"

#include <iosfwd>
#include <limits>

// Unfortunately, the Windows version of <limits> appears to define min and max as macros that
// hide the previously declared min and max templates in the standard library.
#ifdef eWIN32
#undef min
#undef max
#endif

namespace spica {

    //! String class supporting Rexx-like operations.
    class RexxString {

        //! Insert a string into an output stream.
        friend std::ostream &operator<<( std::ostream &, const RexxString & );

        //! Extract a string from an input stream.
        friend std::istream &operator>>( std::istream &, RexxString & );

        //! Compare two strings for equality.
        friend bool operator==( const RexxString &, const RexxString & );

        //! Compare two strings.
        friend bool operator< ( const RexxString &, const RexxString & );

    private:

        // The text of a string is found through a string_node. There might be many RexxString
        // objects pointing to any particular string_node. Strings share their representations
        // when possible. Copying is done on demand.
        //
        struct string_node {
            int   count;
            char *workspace;

            string_node( ) : count( 1 ), workspace( nullptr ) { }
        };

        string_node *rep;

    public:

        //! Construct an empty string.
        RexxString( );

        //! Construct a RexxString that is a copy of the existing RexxString.
        RexxString( const RexxString &existing );

        //! Construct a RexxString that is a copy of the existing C-string.
        RexxString( const char *existing );

        //! Construct a RexxString from a single, existing character.
        explicit RexxString( char existing );

        //! Assign the other RexxString to this RexxString.
        RexxString &operator=( const RexxString &other );

        //! Assign the other C-string to this RexxString.
        RexxString &operator=( const char *other );

        // TODO: Add move constructor and move assignment operator.

        //! Destroy this RexxString.
        ~RexxString( );

        //! Convert this RexxString to a C-style string.
        /*!
         * This method returns a pointer to this string's internal representation. That pointer
         * will be invalidated by any mutating operation.
         */
        explicit operator const char *( ) const
        { return rep->workspace; }

        //! Return the length of this string.
        /*! \sa size */
        [[nodiscard]] std::size_t length( ) const;

        //! Return the length of this string.
        /*! \sa length */
        [[nodiscard]] std::size_t size( ) const
        { return length( ); }

        //! Append the given RexxString to the end of this RexxString.
        RexxString &append( const RexxString &other );

        //! Append the given C-string to the end of this RexxString.
        RexxString &append( const char *other );

        //! Append the given character to the end of this RexxString.
        RexxString &append( char other );

        //! Erase this RexxString, making it empty.
        void erase( );

        //! Return the rightmost characters of this RexxString.
        [[nodiscard]] RexxString right( std::size_t length, char pad = ' ' ) const;

        //! Return the leftmost characters of this RexxString.
        [[nodiscard]] RexxString left( std::size_t length, char pad = ' ' ) const;

        //! Return this RexxString centered between runs of pad characters.
        [[nodiscard]] RexxString center( std::size_t length, char pad = ' ' ) const;

        //! Copy this RexxString.
        [[nodiscard]] RexxString copy( std::size_t count ) const;

        //! Erase a substring of this RexxString.
        [[nodiscard]] RexxString erase(
            std::size_t starting_position,
            std::size_t count = std::numeric_limits<std::size_t>::max( ) ) const;

        //! Insert a RexxString into this RexxString.
        [[nodiscard]] RexxString insert(
            const RexxString& incoming,
            std::size_t starting_position = 1,
            std::size_t length = std::numeric_limits<std::size_t>::max( ) ) const;

        //! Search this RexxString forward for a character.
        [[nodiscard]] size_t pos( char needle, std::size_t starting_position = 1 ) const;

        //! Search this RexxString forward for a C-string.
        [[nodiscard]] size_t pos( const char *needle, std::size_t starting_position = 1 ) const;

        //! Search this RexxString backward for a character.
        [[nodiscard]] size_t last_pos(
            char needle,
            std::size_t starting_position = std::numeric_limits<std::size_t>::max( ) ) const;

        //! Strip leading or trailing instances of kill_char from this RexxString.
        [[nodiscard]] RexxString strip( char mode = 'B', char kill_char = ' ' ) const;

        //! Locate a substring of this RexxString.
        [[nodiscard]] RexxString substr(
            std::size_t starting_position,
            std::size_t count = std::numeric_limits<std::size_t>::max( ) ) const;

        //! Locate a substring of this RexxString consisting of the specified number of words.
        RexxString subword(
            std::size_t starting_position,
            std::size_t count = std::numeric_limits<std::size_t>::max( ),
            const char *white = nullptr ) const;

        //! Return a specific word from this RexxString.
        /*!
         * For this method indexes are word counts. The first word in the string is word number
         * 1. \sa subword.
         *
         * \param offset The index of the word of interest.
         * \param white Pointer to a string of word delimiter characters.
         */
        RexxString word( std::size_t starting_position, const char *white = nullptr ) const
            { return subword( starting_position, 1, white ); }

        //! Return the number of words in this RexxString.
        int words( const char *white = nullptr ) const;
    };

    // +++++
    // These relational operators are defined in terms of the two friends.
    // +++++

    //! Compare two RexxStrings for inequality.
    /*!
     * This function returns true if the RexxStrings are different. The comparison is done in a
     * case sensitive manner.
     */
    inline bool operator!=( const RexxString &left, const RexxString &right )
        { return !( left == right ); }

    //! Compare two RexxStrings.
    /*!
     * This function returns true if the first RexxString is the same as the second or if it
     * comes after the second. The comparison is done in a case sensitive manner.
     */
    inline bool operator>=( const RexxString &left, const RexxString &right )
        { return !( left < right ); }

    //! Compare two RexxStrings.
    /*!
     * This function returns true if the first RexxString comes after the second. The comparison
     * is done in a case sensitive manner.
     */
    inline bool operator>( const RexxString &left, const RexxString &right )
        { return right < left; }

    //! Compare two RexxStrings.
    /*!
     * This function returns true if the first RexxString is the same as the second or if it
     * comes before the second. The comparison is done in a case sensitive manner.
     */
    inline bool operator<=( const RexxString &left, const RexxString &right )
        { return right >= left; }

    // +++++
    // Infix binary concatenation is too useful to pass up.
    // +++++

    //! Concatenate two RexxStrings.
    RexxString operator+( const RexxString &left, const RexxString &right );

    //! Concatenate a RexxString and a C-string.
    RexxString operator+( const RexxString &left, const char *right );

    //! Concatenate a C-string and a RexxString.
    RexxString operator+( const char *left, const RexxString &right );

    //! Concatenate a RexxString and a character.
    RexxString operator+( const RexxString &left, char right );

    //! Concatenate a character and a RexxString.
    RexxString operator+( char left, const RexxString &right );

}

#endif
