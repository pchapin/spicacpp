/*! \file    RexxString.cpp
 *  \brief   Implementation of a Rexx-like string class.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * This file implements a simple string class. It supports a set of operations that allow
 * clients to use string objects in a manner similar to the way strings in Rexx work.
 *
 * The 'rep' member should never be nullptr. This is because if an exception is throw during
 * construction (that's the only time rep might become nullptr), the object under construction
 * can never be accessed in a well defined manner.
 *
 * This version is well behaved in a multi-threaded environment provided the symbol
 * pMULTITHREADED is defined before compilation.
 *
 * TODO: Consider the following items...
 *
 * + The inserter and extractor operators should honor stream formatting state.
 *
 * + Change the thread handling to use C++ 2011 threads. The current implementation was written
 *   before C++ 2011 was available.
 */

#include <cstring>
#include <iostream>
#include <memory>

#include "RexxString.hpp"

#if defined(pMULTITHREADED)
#include "synchronize.hpp"
#endif

using namespace std;

/*! \class spica::RexxString
 *
 * Class RexxString has features that are similar to those offered by the strings built into the
 * Rexx language. However, not all of the features of Rexx strings are currently implemented.
 * Furthermore, the names of these methods and their semantics are not always exactly the same
 * as the corresponding Rexx operation. Nevertheless, the general behavior of these strings
 * should be very familiar to a Rexx programmer.
 *
 * These strings can *not* hold a null character as part of the string's data. However, these
 * strings are fully dynamic and can expand (and contract) as necessary. These strings are
 * exception safe in the sense that if an exception occurs during a string operation there is no
 * resource leakage and the string being operated on is left unchanged. The only exception these
 * methods might throw is `std::bad_alloc` due to an out of memory condition.
 *
 * Some of the operations in this class cause the string to be changed while many of the others
 * return a new string with the changed value. In the future I may provide both "mutating" and
 * "non-mutating" forms of most operations. For now, the decision to use one form or the other
 * was made based in part on what Rexx does, and in part on what seemed most reasonable. I
 * considered making these strings fully immutable but that does not seem correct for strings
 * such as these that have value semantics.
 *
 * Notice that the methods that use an offset into a string use offsets based on one, not zero.
 * This follows Rexx conventions. Furthermore the methods all try to do sensible things when
 * given strange argument values. These strings are quite forgiving in this respect. In
 * particular, negative counts are normally treated as if they were zero and zero counts do the
 * expected thing. Counts that go beyond the end of the string are generally taken to mean "all
 * the string that is available." Offsets that are before the start of the string (&lt; 1) or
 * off the end of the string generally cause "no operation" to occur.
 *
 * This string class currently uses reference counting to improve the speed of copying strings.
 * With this implementation, passing strings to functions by value, returning them by value, or
 * copying them are all low overhead, O(1) operations. A string's representation is only copied
 * when necessary (on demand). NOTE: With C++ 2011 move operations, this reference counting
 * trick is probably wholly unnecessary. I'll have to look into that.
 *
 * These strings are thread safe in the sense that multiple threads can manipulate the same
 * string without causing undefined behavior. If a thread reads a string's value while that
 * value is being updated by a mutating operation, it is unspecified if the first thread reads
 * the string's old value or new value. However, it will read one of those two values reliably.
 * It is important that no global strings are created in a multithreaded program. This is
 * because this class uses a global lock and C++ does not define the order in which global
 * static data is initialized across translation units. If you need a global string, create a
 * global pointer to a string and then give that pointer a value using operator new after `main`
 * has started.
 *
 * To compile in multithreaded support the symbol pMULTITHREADED must be defined when this file
 * (and the header file) are compiled. The module sem.cpp from the spica library must also be
 * provided. This class assumes that spica::mutex_sem supports recursive locking. If that is not
 * the case, the non-mutating operations that return a new string will deadlock.
 */

namespace spica {

    namespace {

        #if defined(pMULTITHREADED)
        // This is the "BRSL" (Big RexxString Lock).
        mutex_sem string_lock;
        #endif

        //-------------------------------------------------
        //           Internally Linked Functions
        //-------------------------------------------------

        bool is_white( int ch, const char *white )
        {
            // If the user is trying to use a special kind of whitespace...
            if( white != nullptr ) {
                if( strchr( white, ch ) != nullptr ) return true;
                return false;
            }

            // Otherwise use the default.
            if( ch == ' '  || ch == '\t' || ch == '\v' ||
                ch == '\r' || ch == '\n' || ch == '\f' )
                return true;

            return false;
        }

    }


    //--------------------------------------
    //           Friend Functions
    //--------------------------------------

    /*!
     * This function returns true if the strings have the same contents. The comparison is done
     * in a case sensitive manner.
     */
    bool operator==( const RexxString &left, const RexxString &right )
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        // Is this comparison worthwhile?
        if( left.rep == right.rep ) return true;
        return ( strcmp( left.rep->workspace, right.rep->workspace ) == 0 );
    }


    /*!
     * This function returns true if the first string comes before the second. [Elaborate on
     * what 'comes before' means for strings]
     */
    bool operator<( const RexxString &left, const RexxString &right )
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        // Is this comparison worthwhile?
        if( left.rep == right.rep ) return false;
        return ( strcmp( left.rep->workspace, right.rep->workspace ) < 0 );
    }


    /*!
     * This function writes the characters of the given string into the given output stream. A
     * newline character is *not* added to the output automatically.
     */
    std::ostream &operator<<( std::ostream &os, const RexxString &right )
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        os << right.rep->workspace;
        return os;
    }


    /*!
     * This function reads an entire line of characters from the given input stream into the
     * given string. Not that this behavior is different than the stream extractor for
     * `std::string` which only reads a single word. Here characters are read until a newline or
     * EOF is reached. The string is expanded as necessary. This function does not add the
     * newline to the string although it does remove the newline from the input.
     */
    std::istream &operator>>( std::istream &is, RexxString &right )
    {
        char   ch;
        RexxString temp;

        // This is sort of inefficient. Do I care? Not right now.
        while( is.get( ch ) ) {
            if( ch == '\n' ) break;
            temp.append( ch );
        }

        right = temp;
        return is;
    }

    //----------------------------
    //           Methods
    //----------------------------

    RexxString::RexxString( )
    {
        std::unique_ptr<string_node> new_node( new string_node );
        new_node->workspace = new char[1];
        *new_node->workspace = '\0';
        rep = new_node.release( );
    }


    RexxString::RexxString( const RexxString &existing )
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock(string_lock);
        #endif

        rep = existing.rep;
        rep->count++;
    }


    RexxString::RexxString( const char *existing )
    {
        unique_ptr<string_node> new_node( new string_node );
        new_node->workspace = new char[strlen( existing ) + 1];
        strcpy( new_node->workspace, existing );
        rep = new_node.release( );
    }


    RexxString::RexxString( char existing )
    {
        unique_ptr<string_node> new_node( new string_node );
        new_node->workspace = new char[2];
        new_node->workspace[0] = existing;
        new_node->workspace[1] = '\0';
        rep = new_node.release( );
    }


    /*!
     * This method releases the memory owned by the string provided that this string's
     * representation is not being shared.
     */
    RexxString::~RexxString( )
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        if( rep->count > 1 ) rep->count--;
        else {
            delete [] rep->workspace;
            delete    rep;
        }
    }


    RexxString &RexxString::operator=( const RexxString &other )
    {
        // Check for assignment to self.
        if( &other == this ) return *this;

        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        if( rep->count > 1 ) rep->count--;
        else {
            delete [] rep->workspace;
            delete    rep;
        }

        rep = other.rep;
        rep->count++;
        return *this;
    }


    RexxString &RexxString::operator=( const char *other )
    {
        if( other == nullptr ) return *this;

        unique_ptr<string_node> new_node( new string_node );
        new_node->workspace = new char[strlen( other ) + 1];
        strcpy( new_node->workspace, other );

        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        if( rep->count > 1 ) rep->count--;
        else {
            delete [] rep->workspace;
            delete    rep;
        }
        rep = new_node.release( );
        return *this;
    }


    /*!
     * The length does not include the terminating null character. Note that currently this is
     * an O(n) operation.
     */
    size_t RexxString::length( ) const
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        return strlen( rep->workspace );
    }


    RexxString &RexxString::append( const RexxString &other )
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        size_t count = strlen( rep->workspace ) + strlen( other.rep->workspace );
        unique_ptr<string_node> new_node( new string_node );
        new_node->workspace = new char[count + 1];

        strcpy( new_node->workspace, rep->workspace );
        strcat( new_node->workspace, other.rep->workspace );

        if( rep->count > 1 ) rep->count--;
        else {
            delete [] rep->workspace;
            delete    rep;
        }
        rep = new_node.release( );
        return *this;
    }


    RexxString &RexxString::append( const char *other )
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        size_t count = strlen( rep->workspace ) + strlen( other );
        unique_ptr<string_node> new_node( new string_node );
        new_node->workspace = new char[count + 1];

        strcpy( new_node->workspace, rep->workspace );
        strcat( new_node->workspace, other );

        if( rep->count > 1 ) rep->count--;
        else {
            delete [] rep->workspace;
            delete    rep;
        }
        rep = new_node.release( );
        return *this;
    }


    RexxString &RexxString::append( char other )
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        size_t count = strlen( rep->workspace ) + 1;
        unique_ptr<string_node> new_node( new string_node );
        new_node->workspace = new char[count + 1];

        strcpy( new_node->workspace, rep->workspace );
        new_node->workspace[count - 1] = other;
        new_node->workspace[count    ] = '\0';

        if( rep->count > 1 ) rep->count--;
        else {
            delete [] rep->workspace;
            delete    rep;
        }
        rep = new_node.release( );
        return *this;
    }


    /*!
     * After this method returns, this string is empty. This is a mutating operation. In that
     * respect it differs from erase(int, int)
     */
    void RexxString::erase( )
    {
        unique_ptr<string_node> new_node( new string_node );
        new_node->workspace = new char[1];
        *new_node->workspace = '\0';

        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        if( rep->count > 1 ) rep->count--;
        else {
            delete [] rep->workspace;
            delete rep;
        }
        rep = new_node.release( );
    }


    /*!
     * If the requested number of characters is larger than the length of the string this method
     * pads the result (on the left) with multiple copies of the pad character. Consequently
     * this method always returns a string with the requested length.
     *
     * \param length The number of characters to return.
     * \param pad The pad character to use if length is too large.
     * \return A new string containing the result. The original string is unchanged.
     */
    RexxString RexxString::right( size_t length, char pad ) const
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        // A place to put the answer.
        RexxString result;

        size_t current_length = strlen( rep->workspace );

        // If we need to make the string shorter...
        if( length < current_length ) {
            char *temp = new char[length + 1];
            strcpy( temp, &rep->workspace[current_length - length] );
            delete [] result.rep->workspace;
            result.rep->workspace = temp;
        }

        // otherwise we need to make the string longer or the same size...
        else {
            char *temp = new char[length + 1];
            memset( temp, pad, length - current_length );
            strcpy( &temp[length - current_length], rep->workspace );
            delete [] result.rep->workspace;
            result.rep->workspace = temp;
        }

        return result;
    }


    /*!
     * If the requested number of characters is larger than the length of this string this
     * method pads the result (on the right) with multiple copies of the pad character.
     * Consequently this method always returns a string with the requested length.
     *
     * \param length The number of characters to return.
     * \param pad The pad character to use if length is too large.
     * \return A new string containing the result. The original string is unchanged.
     */
    RexxString RexxString::left( size_t length, char pad ) const
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        // A place to put the answer.
        RexxString result;

        size_t current_length = strlen(rep->workspace);

        // If we need to make the string shorter...
        if( length < current_length ) {
            char *temp = new char[length + 1];
            strncpy( temp, rep->workspace, length );
            temp[length] = '\0';
            delete [] result.rep->workspace;
            result.rep->workspace = temp;
        }

        // otherwise we need to make the string longer...
        else {
            char *temp = new char[length + 1];
            strcpy( temp, rep->workspace );
            memset( &temp[current_length], pad, length - current_length );
            temp[length] = '\0';
            delete [] result.rep->workspace;
            result.rep->workspace = temp;
        }

        return result;
    }


    /*!
     * If the requested length is larger than the length of this string (and this would be the
     * usual case), this string is centered between a suitable number of pad characters. If the
     * requested length is less than the length of this string, this string is truncated so that
     * the returned string contains only the first length characters of this string.
     *
     * \param length The length of the returned string.
     * \param pad The pad character to use on either side of the centered string.
     * \return A new string containing the result. The original string is not changed.
     */
    RexxString RexxString::center( size_t length, char pad ) const
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        // A place to put the answer.
        RexxString result;

        size_t current_length = strlen( rep->workspace );

        // If the current string is too large or the same size, it's just a `left` operation.
        if( length <= current_length ) {
            return left( length, pad );
        }

        // Otherwise we have to do real work.
        else {
            size_t left_side  = ( length - current_length ) / 2;
            size_t right_side = length - current_length - left_side;

            char *temp = new char[length + 1];
            memset( temp, pad, left_side );
            strcpy( &temp[left_side], rep->workspace );
            memset( &temp[left_side + current_length], pad, right_side );
            temp[length] = '\0';
            delete [] result.rep->workspace;
            result.rep->workspace = temp;
        }
        return result;
    }


    /*!
     * For example if this string was 'xyz' and count was 3, this method would return
     * 'xyzxyzxyz'.
     *
     * \param count The number of times this string should be copied.
     *
     * \return A new string containing count copies of this string concatenated onto one
     * another. The original string is unchanged.
     */
    RexxString RexxString::copy( size_t count ) const
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        // A place to put the answer.
        RexxString result;

        char *temp = new char[count * strlen( rep->workspace ) + 1];

        temp[0] = '\0';
        for( size_t i = 0; i < count; i++ ) strcat( temp, rep->workspace );
        delete [] result.rep->workspace;
        result.rep->workspace = temp;

        return result;
    }


    /*!
     * \param offset The index to the starting character of the substring to be erased.
     * \param count The number of characters to be erased.
     * \return A new string containing the result after erasure. The original string is
     * unchanged.
     */
    RexxString RexxString::erase( size_t starting_position, size_t count ) const
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        // A place to put the answer.
        RexxString result;

        // The client uses one-based positions. We'll used zero-based offsets.
        size_t offset = starting_position - 1;

        size_t current_length = strlen( rep->workspace );

        // Verify that there is actual work to do.
        if( offset >= current_length || count == 0 )
            { result = *this; return result; }

        // Trim the count so that it fits into the bounds on the string. This has to be done
        // carefully considering that count might be maximum in many cases.
        size_t max_count = current_length - offset;
        if( count > max_count ) count = max_count;

        // Now do the work.
        char *temp = new char[current_length - count + 1];
        memcpy( temp, rep->workspace, offset );
        strcpy( &temp[offset], &rep->workspace[offset + count] );
        delete [] result.rep->workspace;
        result.rep->workspace = temp;

        return result;
    }


    /*!
     * If the index is just past the end of this string, the inserted material is appended to
     * this string. If the index is farther past the end than that, the result string is the
     * same as this string (the insertion request is ignored).
     *
     * \param incoming The string to insert.
     *
     * \param offset The index into this string before which the new material will be inserted.
     * Thus an index of one implies that the new material is prepended to the string.
     *
     * \param count The number of characters to insert. If this parameter is greater than the
     * length of the incoming string, all the incoming characters are used.
     *
     * \return A new string containing the result after insertion. The original string is
     * unchanged.
     */
    RexxString RexxString::insert( const RexxString &incoming, size_t starting_position, size_t count ) const
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        // A place to put the answer.
        RexxString result;

        size_t offset = starting_position - 1;
        size_t current_length = strlen( rep->workspace );

        // Verify that there is actual work to do.
        if( offset > current_length || count == 0 )
            { result = *this; return result; }

        // Trim the count.
        size_t incoming_length = strlen( incoming.rep->workspace );
        if( count > incoming_length ) count = incoming_length;

        // Now do the work.
        char *temp = new char [current_length + count + 1];
        memcpy( temp, rep->workspace, offset );
        memcpy( &temp[offset], incoming.rep->workspace, count );
        strcpy( &temp[offset + count], &rep->workspace[offset] );
        delete [] result.rep->workspace;
        result.rep->workspace = temp;

        return result;
    }


    /*!
     * You can *not* locate the null character at the end of the string using this method.
     *
     * \param needle The character to find.
     * \param offset The starting index for the search.
     * \return The index of the first occurrence of the needle or 0 if it is not found.
     */
    size_t RexxString::pos( char needle, size_t starting_position ) const
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        size_t offset = starting_position - 1;

        // If we are starting off the end of the string, then obviously we didn't find anything.
        // Note that this function *does* allow the caller to locate the null character at the
        // end of the string.
        if( offset > strlen( rep->workspace ) )
            return 0;

        // Locate the character.
        const char *p = rep->workspace + offset;
        p = strchr( p, needle );

        // If we didn't find it, return error.
        if( p == nullptr ) return 0;

        // Otherwise return the offset to the character.
        return ( p - rep->workspace ) + 1;
    }


    /*!
     * \param needle Pointer to the string to find.
     * \param offset The starting index for the search.
     * \return The index to the beginning of the needle string's first occurrence or 0 if the
     * needle string is not found.
     */
    size_t RexxString::pos( const char *needle, size_t starting_position ) const
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        size_t offset = starting_position - 1;

        // If we are starting off the end of the string, then obviously we didn't find anything.
        if( offset > strlen( rep->workspace ) )
            return 0;

        // Locate the substring.
        const char *p = rep->workspace + offset;
        p = strstr( p, needle );

        // If we didn't find it, return error.
        if( p == nullptr ) return 0;

        // Otherwise return the offset to the first character in the substring.
        return ( p - rep->workspace ) + 1;
    }


    /*!
     * \param needle The character to find.
     *
     * \param offset The starting index for the search. Any index that is off the end of the
     * string implies that the search starts at the string's end.
     *
     * \return The index of the last occurrence of the needle character (last relative to the
     * starting index) or 0 if the character was not found.
     */
    size_t RexxString::last_pos( char needle, size_t starting_position ) const
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        size_t offset = starting_position - 1;

        size_t current_length = strlen( rep->workspace );

        // Handle the case of offset being off the end of the string.
        if( offset > current_length ) offset = current_length;

        const char *p = rep->workspace + offset;

        // Now back up. If we find the character, return the offset to it.
        while( p >= rep->workspace ) {
            if( *p == needle ) return ( p - rep->workspace ) + 1;
            p--;
            // Is it technically ok to step a pointer one off the beginning of an array? (NO!)
        }

        // If we got here, then we didn't find the character.
        return 0;
    }


    /*!
     * \param mode Use 'L' to strip leading characters, 'T' to strip trailing characters, or 'B'
     * to strip both leading and trailing characters.
     *
     * \param kill_char The character to strip. All leading (or trailing or both) copies of such
     * character are removed.
     *
     * \return A new string containing the result. The original string is unchanged.
     */
    RexxString RexxString::strip( char mode, char kill_char ) const
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        // A place to put the answer.
        RexxString result;

        const char *start = rep->workspace;
        const char *end   = strchr( rep->workspace, '\0' );

        // Handle the empty string as a special case.
        if( start == end ) return result;

        // Back up end so that it points just before the terminating null character.
        end--;

        // Move start to the desired spot.
        if( mode == 'L' || mode == 'B' ) {
            while( *start ) {
                if( *start != kill_char ) break;
                start++;
            }
        }

        // Move end to the desired spot. Note that there is a portability problem here. If the
        // string is entirely kill_char and just 'T' mode is requested, end will be backed up
        // all the way before rep->workspace. This means that end will point off the *front* of
        // an array and that is a bad thing. This should be fixed someday.
        //
        if( mode == 'T' || mode == 'B' ) {
            while( end >= rep->workspace ) {
                if( *end != kill_char ) break;
                end--;
            }
        }

        // There is nothing to do if the two pointers passed by each other.
        if( start > end ) {
            return result;
        }

        // Otherwise there is something to do.
        else {
            ptrdiff_t length = ( end - start ) + 1;
            char *temp = new char[length + 1];
            memcpy( temp, start, length );
            temp[length] = '\0';
            delete [] result.rep->workspace;
            result.rep->workspace = temp;
        }
        return result;
    }


    /*!
     * \param offset The starting index for the substring.
     * \param count The length of the substring
     * \return The specified substring.
     */
    RexxString RexxString::substr( size_t starting_position, size_t count ) const
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        // A place to put the answer.
        RexxString result;

        size_t offset = starting_position - 1;

        size_t current_length = strlen( rep->workspace );

        // If the offset is off the end of the string, then return an empty string.
        //
        if( offset >= current_length ) {
            return result;
        }

        // Adjust the count if necessary.
        if( count > current_length - offset ) count = current_length - offset;

        // Create the new string.
        char *temp = new char[count + 1];
        memcpy( temp, &rep->workspace[offset], count );
        temp[count] = '\0';

        delete [] result.rep->workspace;
        result.rep->workspace = temp;
        return result;
    }


    /*!
     * Embedded delimiter characters are retained exactly as they exist in the string, but
     * leading and trailing delimiter characters are removed. By default delimiters are the
     * white space characters: space, tab, vertical tab, carriage return, newline, and form
     * feed. However, if white is non-null it is taken to point at a string that defines the
     * delimiter characters. For example, if white points at "xyz" then the characters 'x', 'y',
     * and 'z' would be the delimiters. In that case, the second word of the string
     * "HixThereyYouz" would be "There".
     *
     * \param offset The starting index in this string. For this method, indicies are word
     * counts. The first word is at index 1.
     * \param count The number of words in the desired substring.
     * \param white Pointer to a string containing word delimiter characters.
     * \return The specified substring.
     */
    RexxString RexxString::subword( size_t starting_position, size_t count, const char *white ) const
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        // A place to put the answer.
        RexxString result;

        size_t offset = starting_position - 1;

        size_t current_length = words( white );

        // If the offset is off the end of the string, then return an empty string.
        if( offset >= current_length ) {
            return result;
        }

        // Adjust the count if necessary.
        if( count > current_length - offset ) count = current_length - offset;

        // Handle the count of zero as a special case.
        if( count == 0 ) return result;

        // Find the beginning of the the offset-th word.
        const char *start = rep->workspace;
        while( true ) {

            // Skip leading whitespace.
            while( is_white( *start, white ) ) start++;

            if( offset == 0 ) break;

            // Find the end of this word.
            while( !is_white( *start, white ) ) start++;
            offset--;
        }

        // Now find the end of the count-th word from start.
        const char *end = start;
        while( true ) {

            // Find the end of this word.
            while( !( is_white( *end, white ) || *end == '\0' ) ) end++;
            count--;

            if( count == 0 ) break;

            // Find the beginning of the next word.
            while( is_white( *end, white ) ) end++;
        }

        // Now create the new character string.
        ptrdiff_t length = end - start;
        char *temp = new char[length + 1];
        memcpy( temp, start, length );
        temp[length] = '\0';

        delete [] result.rep->workspace;
        result.rep->workspace = temp;
        return result;
    }


    /*!
     * \param white Points at a string of word delimiter characters.
     * \return The number of words in this string.
     * \sa subword
     */
    int RexxString::words( const char *white ) const
    {
        #if defined(pMULTITHREADED)
        mutex_sem::grabber lock( string_lock );
        #endif

        int  word_count = 0;   // The number of words found.
        int  in_word    = 0;   // =1 When we are scanning a word.

        // Scan down the string...
        for( const char *p = rep->workspace; *p; p++ ) {

            // If this is the start of a word...
            if( !is_white( *p, white ) && !in_word ) {
                word_count++;
                in_word = 1;
            }

            // If we just finished scanning a word...
            if( is_white( *p, white ) && in_word ) {
                in_word = 0;
            }
        }
        return word_count;
    }


    /*!
     * This function concatenates right onto the end of left and returns the result. Neither
     * right nor left are modified.
     */
    RexxString operator+( const RexxString &left, const RexxString &right )
        { RexxString temp( left ); temp.append( right ); return temp; }

    /*!
     * This function concatenates right onto the end of left and returns the result. Neither
     * right nor left are modified.
     */
    RexxString operator+( const RexxString &left, const char *right )
        { RexxString temp( left ); temp.append( right ); return temp; }

    /*!
     * This function concatenates right onto the end of left and returns the result. Neither
     * right nor left are modified.
     */
    RexxString operator+( const char *left, const RexxString &right )
        { RexxString temp( left ); temp.append( right ); return temp; }

    /*!
     * This function concatenates right onto the end of left and returns the result. Neither
     * right nor left are modified.
     */
    RexxString operator+( const RexxString &left, char right )
        { RexxString temp( left ); temp.append( right ); return temp; }

    /*!
     * This function concatenates right onto the end of left and returns the result. Neither
     * right nor left are modified.
     */
    RexxString operator+( char left, const RexxString &right )
        { RexxString temp( left ); temp.append( right ); return temp; }

}
