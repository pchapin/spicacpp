/*! \file    FileVector.hpp
 *  \brief   Vector-like container backed by a file.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * A file vector looks and acts like a normal library vector except that it stores its data in a
 * regular, named file. File vectors provide random access iterators that act appropriately
 * allowing the library algorithms to be applied to files in a natural way.
 *
 * File vector is a template allowing different types of objects to be placed in the vector.
 * However, there are restrictions on what types of elements can be used. See the documentation
 * for more details.
 *
 * TODO:
 *
 * + This version only supports Windows. It could be generalized to any system that supports
 *   memory mapped files.
 *
 * + The current implementation limits the size of the file being memory mapped to 4 GB so that
 *   32-bit Windows is supported. However, that restriction is being applied (unnecessarily) to
 *   64-bit Windows as well. The code needs to be reviewed for proper operation with a 64-bit
 *   size_t (currently it relies on the "fact" that size_t wraps around at 4 GB!).
 *
 * + Reverse iterators are not implemented.
 *
 * + Under some circumstances there is a conflict between some of the methods. For example,
 *   `insert` that takes a size and a value will take two integers for the case of
 *   FileVector<int>. Yet such a call also looks like an instantiation of the template `insert`
 *   using "int" for "InputIterator". This general problem is well known and the solution, which
 *   is ugly, is described in "Generic Programming and the STL" by Matthew Austern.
 *
 * + The template `insert` method isn't currently implemented at all. The simple way of doing it
 *   will cause it to have O(n*m) performance---a violation of the standard for vectors. The
 *   correct solution is to detect the different iterator categories at compile time and use an
 *   appropriate version of the function depending on the category. This is also described in
 *   Austern's book.
 *
 * + This implementation never reduces the capacity of a FileVector except for when the
 *   FileVector is destroyed. It probably should reduce capacity under certain circumstances in
 *   order to reclaim disk space.
 *
 * + I really should be using placement `new` and an explicit destruction whenever I install a
 *   new object into the vector or deallocate objects that are already in the vector. That would
 *   allow me to accept objects that have constructors and destructors as long as they obeyed
 *   the other restrictions mentioned in the documentation. (If I do this, I should probably
 *   also use the assignment operator to move objects around in the vector instead of using
 *   `memmove` as is currently done).
 *
 * + Several of the standard vector methods are unimplemented. The relational operations `==`
 *   and `<` are declared, but not implemented.
 */

#ifndef FILEVECTOR_HPP
#define FILEVECTOR_HPP

#include "environ.hpp"

#if eOPSYS != eWINDOWS
#error FileVector only supports the Windows operating system.
#endif

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <type_traits>

#define NOMINMAX
#include <windows.h>

#include "winexcept.hpp"


namespace spica {

    // The POD concept is used to restrict the types that can be used in a FileVector. The
    // restrictions are necessary because the FileVector implementation uses `memmove` to move
    // objects around in the vector. This is only safe if the objects are trivially copyable and
    // have a standard layout.
    //
    template<typename T>
    concept POD = std::is_trivial_v<T> && std::is_standard_layout_v<T>;

    template<POD T>
    class FileVector {
    public:

        // The required typedef names for vectors.
        typedef              T  value_type;
        typedef              T *pointer;
        typedef        const T *const_pointer;
        typedef              T &reference;
        typedef        const T &const_reference;
        typedef    std::size_t  size_type;
        typedef std::ptrdiff_t  difference_type;
        typedef              T *iterator;
        typedef        const T *const_iterator;

        iterator begin( )
            { return raw; }

        const_iterator begin( ) const
            { return raw; }

        iterator end( )
            { return raw + item_count; }

        const_iterator end( ) const
            { return raw + item_count; }

        size_type size( ) const
            { return item_count; }

        size_type max_size( ) const;

        size_type capacity( ) const
            { return item_capacity; }

        bool empty( ) const
            { return ( item_count == 0 ) ? true : false; }

        reference operator[]( size_type offset )
            { return raw[offset]; }

        const_reference operator[]( size_type offset ) const
            { return raw[offset]; }

        reference front( )
            { return raw[0]; }

        const_reference front( ) const
            { return raw[0]; }

        reference back( )
            { return raw[item_count - 1]; }

        const_reference back( )  const
            { return raw[item_count - 1]; }

        void push_back( const T &new_item );

        void pop_back( )
            { if( item_count ) item_count--; }

        iterator insert( iterator Position, const T &new_item );

        void insert( iterator Position, size_type n, const T &fill );

        template<class InputIterator>
        void insert( iterator Position, InputIterator first, InputIterator last );

        void erase( iterator Position );
        void erase( iterator first, iterator last );
        void clear( )
            { item_count = 0 };
        void resize( size_type n, const T &fill = T( ) );
        void assign( size_type n, const T &new_item );

        template<class InputIterator>
        void assign( InputIterator first, InputIterator last );

        /*!
         * Creates an empty vector if the file does not exist, otherwise creates a vector
         * containing the contents of the file.
         */
        explicit FileVector( const char *file_name );

        /*!
         * If the file exists it is overwritten. If it does not exist, it is created. The vector
         * contains n copies of initial.
         */
        FileVector( const char *file_name, size_type n, const T &initial = T( ) );

        // Copies existing.
        FileVector( const char *file_name, const FileVector &existing );

        /*!
         * If the file exists it is overwritten. If it does not exist, it is created. The vector
         * contains a copy of the range [first, last).
         */
        template<class InputIterator>
        FileVector( const char *file_name, InputIterator first, InputIterator last );

        // Cleans up.
        ~FileVector( );

        // Copies other.
        FileVector<T> &operator=( const FileVector<T> &other );

        // Swaps *this with other.
        void swap( FileVector<T> &other );

        // Increases the item_capacity without changing the item_count.
        void reserve( size_type new_capacity );
        
    private:

        // No copy constructor (for now).
        FileVector( const FileVector<T> & ) = delete;

        // The internal representation.
        HANDLE    file_handle;     // Handle from CreateFile( ).
        HANDLE    mapping_handle;  // The result of the file mapping operation.
        T        *raw;             // Points at the first element of the mapped file.

        size_type item_count;      // The number of elements in the vector.
        size_type item_capacity;   // Total number of reserved elements.

        /*!
         * This function can be used to expand the capacity as necessary. In this implementation
         * the capacity will never shrink. The parameter is how much more space than item_count
         * is needed. This function might not do anything if the existing capacity is
         * sufficient.
         */
        void reallocate( size_type more );
    };


    // These operator functions are not class members.
    template<typename T>
    bool operator==( const FileVector<T> &, const FileVector<T> & );

    template<typename T>
    bool operator<( const FileVector<T> &, const FileVector<T> & );

} // End of namespace scope.



//
// The implementation of FileVector's members.
//

namespace spica {

    //
    // FileVector<T>::reserve( size_type )
    //
    // Causes the vector's capacity to be at least as great as the argument.
    //
    template<POD T>
    void FileVector<T>::reserve( size_type new_capacity )
    {
        if( new_capacity < item_capacity ) return;

        size_type extra = new_capacity - item_capacity;
        reallocate( extra );
    }


    //
    // FileVector<T>::push_back( const T & )
    //
    // Adds a new item to the end of the vector.
    //
    template<POD T>
    void FileVector<T>::push_back( const T &new_item )
    {
        reallocate( 1 );

        // Install the item.
        raw[item_count] = new_item;
        item_count++;
    }


    //
    // FileVector<T>::insert( iterator position, const T &new_item )
    //
    // Inserts the new item before position.
    //
    template<POD T>
    typename FileVector<T>::iterator
        FileVector<T>::insert( iterator position, const T &new_item )
    {
        reallocate( 1 );

        std::memmove( position + 1, position, sizeof( T )*( ( raw + item_count ) - position ) );
        *position = new_item;
        item_count++;
        return position;
    }


    //
    // FileVector<T>::insert( iterator position, size_type n, const T &Fill )
    //
    // Inserts n copies of Fill before position.
    //
    template<POD T>
    void FileVector<T>::insert( iterator position, size_type n, const T &fill )
    {
        reallocate( n );

        std::memmove( position + n, position, sizeof( T )*( ( raw + item_count ) - position ) );
        for( size_type counter = 0; counter < n; ++counter ) {
            *position = fill;
            position++;
        }
    }


    //
    // template< typename InputIterator >
    // FileVector<T>::insert( iterator position, InputIterator first, InputIterator last )
    //
    // Inserts the given sequence before position. The annoying thing about this is that I can't
    // compute ahead of time the distance between `first` and `last`. Thus I pretty much have to
    // insert one item at a time.
    //
    // Note that this will cause the run-time performance of this function to be O(n*m) where n
    // is related to the item_count of the FileVector and m is the number of elements in the
    // input range. This is a violation of the standard when the iterator type parameter is
    // something more powerful than merely InputIterator. Eventually I should fix this by
    // detecting and handling the different iterator types.
    //
    // template<POD T>
    // template<typename InputIterator>
    // void FileVector<T>::insert( iterator position, InputIterator first, InputIterator last )
    // {
    // }


    //
    // FileVector<T>::erase( iterator )
    //
    // Erases one item from the vector. No checks are done on the iterator's validity.
    //
    template<POD T>
    void FileVector<T>::erase( iterator position )
    {
        std::memmove(
            position, position + 1, sizeof( T )*( ( raw + item_count ) - ( position + 1 ) ) );
        item_count--;
    }


    //
    // FileVector<T>::erase( iterator first, iterator last )
    //
    // Erases a range from the vector. No checks are done on the range's validity.
    //
    template<POD T>
    void FileVector<T>::erase( iterator first, iterator last )
    {
        std::memmove( first, last, sizeof( T )*( ( raw + item_count ) - last ) );
        item_count -= ( last - first );
    }


    //
    // FileVector<T>::FileVector( const char *file_name );
    //
    // The constructor opens and maps the file.
    //
    template<POD T>
    FileVector<T>::FileVector( const char *file_name )
    {
        // Open the file.
        file_handle = CreateFile(
            file_name,                   // The name (of course).
            GENERIC_READ|GENERIC_WRITE,  // I want to read and write the file.
            0,                           // Share mode =>  exclusive access.
            0,                           // Default security attributes.
            OPEN_ALWAYS,                 // If it does not exist, create it.
            FILE_FLAG_RANDOM_ACCESS,     // Let Windows optimize access.
            0                            // No template file.
        );
        if( file_handle == INVALID_HANDLE_VALUE )
            throw Windows::APIError( "Can't open the backing file for a FileVector" );

        // Learn the file's size. I check to make sure the file is not too large so that I can
        // represent its byte count in size_type. This is mostly only relevant for 32-bit
        // Windows where size_type is limited to 4 GB and yet files might be larger than that.
        // The current implementation works by limiting the file size to 4 GB, which is
        // unnecessarily restrictive on 64-bit Windows.
        //
        // In addition, I will start off with the vector's item_capacity equal to its item_count
        // (unless its item_count is zero -- it's important that item_capacity never be zero).
        //
        DWORD high_word;
        DWORD low_word;
        low_word = GetFileSize( file_handle, &high_word );
        if( high_word != 0 ) {
            CloseHandle( file_handle );
            throw std::bad_alloc( );   // Is this the best choice to throw here?
        }
        item_count     = low_word;
        item_count    /= sizeof( T );
        item_capacity = item_count;
        if( item_capacity == 0 ) item_capacity = 1;
        
        // Map it.
        mapping_handle = CreateFileMapping(
            file_handle,          // The file we are trying to map.
            0,                    // Default security attributes.
            PAGE_READWRITE,       // I want to read and write this file.
            0,                    // Map a size equal to the current capacity.
            sizeof( T ) * item_capacity, //  ...
            0                     // I am not interested in using a name.
        );
        if( mapping_handle == 0 ) {
            CloseHandle( file_handle );
            throw Windows::APIError( "Can't map the backing file for an FileVector" );
        }

        // Create a view into the mapped file.
        raw = static_cast<T *>( MapViewOfFile(
            mapping_handle,       // The mapped file from which we create the view.
            FILE_MAP_ALL_ACCESS,  // Read/Write through this view.
            0,                    // Offset into file where view starts.
            0,                    //   ...
            0                     // Map entire file.
        ) );
        if( raw == 0 ) {
            CloseHandle( mapping_handle );
            CloseHandle( file_handle );
            throw Windows::APIError(
                "Can't create a file view of the backing file for an FileVector" );
        }
    }


    //
    // FileVector<T>::FileVector( const char *file_name, size_type n, const T &initial );
    //
    // The constructor opens and maps the file.
    //
    template<POD T>
    FileVector< T >::FileVector( const char *file_name, size_type n, const T &initial )
    {
        // Open the file.
        file_handle = CreateFile(
            file_name,                   // The name (of course).
            GENERIC_READ|GENERIC_WRITE,  // I want to read and write the file.
            0,                           // Share mode =>  exclusive access.
            0,                           // Default security attributes.
            CREATE_ALWAYS,               // If it exists, overwrite it.
            FILE_FLAG_RANDOM_ACCESS,     // Let Windows optimize access.
            0                            // No template file.
        );
        if( file_handle == INVALID_HANDLE_VALUE )
            throw Windows::APIError( "Can't open the backing file for a FileVector" );

        // The item_count is a parameter.
        item_count    = n;
        item_capacity = item_count;
        if( item_capacity == 0 ) item_capacity = 1;

        // Map it.
        mapping_handle = CreateFileMapping(
            file_handle,          // The file we are trying to map.
            0,                    // Default security attributes.
            PAGE_READWRITE,       // I want to read and write this file.
            0,                    // Map a size equal to the current capacity.
            sizeof(T) * item_capacity, //  ...
            0                     // I am not interested in using a name.
        );
        if( mapping_handle == 0 ) {
            CloseHandle( file_handle );
            throw Windows::APIError( "Can't map the backing file for an FileVector" );
        }

        // Create a view into the mapped file.
        raw = static_cast<T *>( MapViewOfFile(
            mapping_handle,       // The mapped file from which we create the view.
            FILE_MAP_ALL_ACCESS,  // Read/Write through this view.
            0,                    // Offset into file where view starts.
            0,                    //   ...
            0                     // Map entire file.
        ) );
        if( raw == 0 ) {
            CloseHandle( mapping_handle );
            CloseHandle( file_handle );
            throw Windows::APIError(
                "Can't create a file view of the backing file for an FileVector" );
        }

        // Initialize the vector with initial.
        for( size_type i = 0; i < item_count; ++i ) {
            raw[i] = initial;
        }
    }


    //
    // FileVector<T>::~FileVector( )
    //
    // The destructor closes things down in an orderly manner.
    //
    template<POD T>
    FileVector<T>::~FileVector( )
    {
        UnmapViewOfFile( raw );
        CloseHandle( mapping_handle );

        // Now truncate the file if necessary.
        if( item_count < item_capacity ) {
            SetFilePointer( file_handle, sizeof( T )*item_count, 0, FILE_BEGIN );
            SetEndOfFile( file_handle );
        }

        // Close the file.
        CloseHandle( file_handle );
    }


    //
    // FileVector<T>::swap( FileVector<T> & )
    //
    // Swaps two FileVector objects in constant time.
    //
    template<POD T>
    void FileVector<T>::swap( FileVector<T> &other )
    {
        using std::swap;

        swap( file_handle,    other.file_handle    );
        swap( mapping_handle, other.mapping_handle );
        swap( raw,            other.raw            );
        swap( item_count,     other.item_count     );
        swap( item_capacity,  other.item_capacity  );
    }


    //
    // FileVector<T>::reallocate( size_type more )
    //
    // This function arranges so that there is at least "more" reserved space than the current
    // item_count. If the item_capacity is already sufficient, nothing is done. Otherwise the
    // file is remapped as necessary.
    //
    template<POD T>
    void FileVector<T>::reallocate( size_type more )
    {
        // Check for overflow in the index variables. This code takes advantage of the fact that
        // size_type is unsigned and thus modular.
        //
        if( item_count + more < item_count ||
            sizeof( T )*item_count + sizeof( T )*more < sizeof( T )*item_count ) {
            throw std::bad_alloc( );
        }

        // If there is already enough space, we are fine.
        if( item_count + more <= item_capacity ) return;

        // If we need more space, allocate more. Double item_capacity as often as necessary to
        // get the necessary space. If item_capacity overflows size_type, give up.
        //
        size_type new_capacity = item_capacity;
        while( item_count + more > new_capacity ) {
            size_type old_value = new_capacity;
            new_capacity *= 2;
            if( new_capacity < old_value || sizeof( T )*new_capacity < sizeof( T )*old_value)  {
                throw std::bad_alloc();
            }
        }

        // Remap the file with the new item_capacity. If an exception occurs during the
        // remapping operation, the FileVector will be unusuable.
        //
        UnmapViewOfFile( raw );
        CloseHandle( mapping_handle );

        // Map it.
        mapping_handle = CreateFileMapping(
            file_handle,          // The file we are trying to map.
            0,                    // Default security attributes.
            PAGE_READWRITE,       // I want to read and write this file.
            0,                    // Map a size equal to the current capacity.
            sizeof( T ) * new_capacity, //  ...
            0                     // I am not interested in using a name.
        );
        if( mapping_handle == 0 ) {
            CloseHandle( file_handle );
            throw Windows::APIError( "Can't remap the backing file for an FileVector" );
        }

        // Create a view into the mapped file.
        raw = static_cast<T *>( MapViewOfFile(
            mapping_handle,       // The mapped file from which we create the view.
            FILE_MAP_ALL_ACCESS,  // Read/Write through this view.
            0,                    // Offset into file where view starts.
            0,                    //   ...
            0                     // Map entire file.
        ) );
        if( raw == 0 ) {
            CloseHandle( mapping_handle );
            CloseHandle( file_handle );
            throw Windows::APIError(
                "Can't recreate a file view of the backing file for an FileVector" );
        }
        item_capacity = new_capacity;
    }

}

#endif
