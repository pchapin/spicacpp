/*! \file    RexxString.hpp
 *  \brief   Interface to a Rexx-like string class.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * Everyone has their own string class. This is mine. These strings mimic the features of
 * strings that exist in the Rexx language.
 */

#ifndef STR_HPP
#define STR_HPP

#include "environ.hpp"
#include <iosfwd>
#include <limits>

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

            string_node( ) : count( 1 ), workspace( 0 ) { }
        };

        string_node *rep;

    public:

        //! Construct an empty string.
        RexxString( );

        //! Construct a string that is a copy of the existing string.
        RexxString( const RexxString &existing );

        //! Construct a string that is a copy of the existing C-string.
        RexxString( const char *existing );

        //! Construct a string from a single, existing character.
        RexxString( char existing );

        //! Assign the other string to this string.
        RexxString &operator=( const RexxString &other );

        //! Assign the other C-string to this string.
        RexxString &operator=( const char *other );

        //! Destroy this string.
        ~RexxString( );

        //! Convert this string to a C-style string.
        /*!
         * This method returns a pointer to this string's internal representation. That pointer
         * will be invalidated by any mutating operation.
         */
        operator const char *( ) const { return rep->workspace; }

        //! Return the length of this string.
        /*! \sa size */
        int length( ) const;

        //! Return the length of this string.
        /*! \sa length */
        int size( ) const { return length( ); }

        //! Append the given string to the end of this string.
        RexxString &append( const RexxString &other );

        //! Append the given string to the end of this string.
        RexxString &append( const char *other );

        //! Append the given character to the end of this string.
        RexxString &append( char other );

        //! Erase this string, making it empty.
        void erase( );

        //! Return the rightmost characters of this string.
        RexxString right( size_t length, char pad = ' ' ) const;

        //! Return the leftmost characters of this string.
        RexxString left( size_t length, char pad = ' ' ) const;

        //! Return this string centered between runs of pad characters.
        RexxString center( size_t length, char pad = ' ' ) const;

        //! Copy this string.
        RexxString copy( size_t count ) const;

        //! Erase a substring of this string.
        RexxString erase(
            size_t starting_position,
            size_t count = std::numeric_limits<std::size_t>::max( ) ) const;

        //! Insert a string into this string.
        RexxString insert(
            const RexxString &incoming,
            size_t starting_position = 1,
            size_t length = std::numeric_limits<std::size_t>::max( ) ) const;

        //! Search this string forward for a character.
        size_t pos( char needle, size_t starting_position = 1 ) const;

        //! Search this string forward for a string.
        size_t pos( const char *needle, size_t starting_position = 1 ) const;

        //! Search this string backward for a character.
        size_t last_pos(
            char needle,
            size_t starting_position = std::numeric_limits<std::size_t>::max( ) ) const;

        //! Strip leading or trailing instances of kill_char from this string.
        RexxString strip( char mode = 'B', char kill_char = ' ' ) const;

        //! Locate a substring of this string.
        RexxString substr(
            size_t starting_position,
            size_t count = std::numeric_limits<std::size_t>::max( ) ) const;

        //! Locate a substring of this string consisting of the specified number of words.
        RexxString subword(
            size_t starting_position,
            size_t count = std::numeric_limits<std::size_t>::max( ),
            const char *white = 0 ) const;

        //! Return a specific word from this string.
        /*!
         * For this method indicies are word counts. The first word in the string is word number
         * 1. \sa subword.
         *
         * \param offset The index of the word of interest.
         * \param white Pointer to a string of word delimiter characters.
         */
        RexxString word( size_t starting_position, const char *white = 0 ) const
            { return subword( starting_position, 1, white ); }

        //! Return the number of words in this string.
        int words( const char *white = 0 ) const;
    };

    // +++++
    // These relational operators are defined in terms of the two friends.
    // +++++

    //! Compare two strings for inequality.
    /*!
     * This function returns true if the strings are different. The comparison is done in a case
     * sensitive manner.
     */
    inline bool operator!=( const RexxString &left, const RexxString &right )
        { return !( left == right ); }

    //! Compare two strings.
    /*!
     * This function returns true if the first string is the same as the second or if it comes
     * after the second. The comparison is done in a case sensitive manner.
     */
    inline bool operator>=( const RexxString &left, const RexxString &right )
        { return !( left < right ); }

    //! Compare two strings.
    /*!
     * This function returns true if the first strings comes after the second.
     */
    inline bool operator>( const RexxString &left, const RexxString &right )
        { return right < left; }

    //! Compare two strings.
    /*!
     * This function returns true if the first string is the same as the second or if it comes
     * before the second. The comparison is done in a case sensitive manner.
     */
    inline bool operator<=( const RexxString &left, const RexxString &right )
        { return right >= left; }

    // +++++
    // Infix binary concatenation is too useful to pass up.
    // +++++

    //! Concatenate two strings.
    RexxString operator+( const RexxString &left, const RexxString &right );

    //! Concatenate a strings and a C-string.
    RexxString operator+( const RexxString &left, const char *right );

    //! Concatenate a C-string and a string.
    RexxString operator+( const char *left, const RexxString &right );

    //! Concatenate a string and a character.
    RexxString operator+( const RexxString &left, char right );

    //! Concatenate a character and a string.
    RexxString operator+( char left, const RexxString &right );

}

#endif
