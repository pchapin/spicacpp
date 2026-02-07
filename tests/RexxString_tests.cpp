/*! \file    RexxString_tests.cpp
 *  \brief   Exercise spica::RexxString.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * This program does not do exhaustive testing, but it does at least try a few basic things.
 * Each test should involve several test cases, for example defined by an array of structures.
 * Instead, each test exercises only a single "typical" case. This is not ideal, but it is
 * better than nothing.
 */

#include <cstring>
#include <sstream>

#include "../RexxString.hpp"
#include "../UnitTestManager.hpp"
#include "../u_tests.hpp"

using namespace std;
using namespace spica;

namespace {

    void constructor_test()
    {
        UnitTestManager::UnitTest test("constructor");

        RexxString object_1;
        RexxString object_2("Hello");
        RexxString object_3(object_2);

        UNIT_CHECK(object_1.length() == 0);
        UNIT_CHECK(object_2.length() == 5);
        UNIT_CHECK(strcmp(static_cast<const char*>(object_2), "Hello") == 0);
        UNIT_CHECK(object_3.length() == 5);
        UNIT_CHECK(strcmp(static_cast<const char*>(object_3), "Hello") == 0);
    }

    void IO_test()
    {
        UnitTestManager::UnitTest test("IO");

        RexxString object_1("Hello");

        ostringstream output;
        output << object_1;
        UNIT_CHECK(output.str() == "Hello");

        istringstream input("World");
        input >> object_1;
        UNIT_CHECK(strcmp(static_cast<const char*>(object_1), "World") == 0);
    }

    void append_test()
    {
        UnitTestManager::UnitTest test("append");

        RexxString junk("Junk");
        RexxString buffer;
        const int counter = 5;

        // Try appending one string onto another (several times).
        for (int i = 0; i < counter; i++) {
            buffer.append(junk);
        }
        UNIT_CHECK(buffer.length() == 20);
        UNIT_CHECK(strcmp(static_cast<const char*>(buffer), "JunkJunkJunkJunkJunk") == 0);

        buffer.erase();
        UNIT_CHECK(buffer.length() == 0);
        UNIT_CHECK(strcmp(static_cast<const char*>(buffer), "") == 0);

        // Now try appending an "old style" string.
        for (int i = 0; i < counter; i++) {
            buffer.append("Junk");
        }
        UNIT_CHECK(buffer.length() == 20);
        UNIT_CHECK(strcmp(static_cast<const char*>(buffer), "JunkJunkJunkJunkJunk") == 0);

        buffer.erase();

        // And finally append single characters.
        for (int i = 0; i < counter; i++) {
            buffer.append('J');
        }
        UNIT_CHECK(buffer.length() == 5);
        UNIT_CHECK(strcmp(static_cast<const char*>(buffer), "JJJJJ") == 0);
    }

    void assignment_test()
    {
        UnitTestManager::UnitTest test("assignment");

        RexxString one;
        RexxString two;
        const char* line = "Junk";

        one = line;
        UNIT_CHECK(one.length() == 4);
        UNIT_CHECK(strcmp(static_cast<const char*>(one), "Junk") == 0);

        two = one;
        UNIT_CHECK(two.length() == 4);
        UNIT_CHECK(strcmp(static_cast<const char*>(two), "Junk") == 0);
    }

    void left_right_test()
    {
        UnitTestManager::UnitTest test("left/right");

        RexxString object_1{"Junk"};
        RexxString object_2{object_1};
        const int count = 9;

        object_1 = object_1.left(count, '-');
        UNIT_CHECK(object_1.length() == 9);
        UNIT_CHECK(strcmp(static_cast<const char*>(object_1), "Junk-----") == 0);

        object_2 = object_2.right(count, '-');
        UNIT_CHECK(object_2.length() == 9);
        UNIT_CHECK(strcmp(static_cast<const char*>(object_2), "-----Junk") == 0);
    }

    void center_test()
    {
        UnitTestManager::UnitTest test("center");

        RexxString object_1{"Junk"};
        const int width = 11;

        object_1 = object_1.center(width, '-');
        UNIT_CHECK(object_1.length() == 11);
        UNIT_CHECK(strcmp(static_cast<const char*>(object_1), "---Junk----") == 0);
    }

    void copy_test()
    {
        UnitTestManager::UnitTest test("copy");

        RexxString object_1{"Junk"};
        const int count = 5;

        object_1 = object_1.copy(count);
        UNIT_CHECK(object_1.length() == 20);
        UNIT_CHECK(strcmp(static_cast<const char*>(object_1), "JunkJunkJunkJunkJunk") == 0);
    }

    void erase_test()
    {
        UnitTestManager::UnitTest test("erase");

        RexxString object_1{"Junk"};
        const int starting_position = 2;
        const int count = 2;

        object_1 = object_1.erase(starting_position, count);
        UNIT_CHECK(object_1.length() == 2);
        UNIT_CHECK(strcmp(static_cast<const char*>(object_1), "Jk") == 0);
    }

    void insert_test()
    {
        UnitTestManager::UnitTest test("insert");

        RexxString object_1{"Junk"};
        RexxString object_2{"xxxx"};
        const int starting_position = 2;
        const int count = 3;

        object_1 = object_1.insert(object_2, starting_position, count);
        UNIT_CHECK(object_1.length() == 7);
        UNIT_CHECK(strcmp(static_cast<const char*>(object_1), "Jxxxunk") == 0);
    }

    void pos_test()
    {
        UnitTestManager::UnitTest test("pos");

        RexxString object_1{"Junk"};
        const int starting_position = 2;
        const char needle_1 = 'n';
        RexxString needle_2{"nk"};

        size_t pos = object_1.pos(needle_1, starting_position);
        UNIT_CHECK(pos == 3);

        size_t last_pos = object_1.last_pos(needle_1);
        UNIT_CHECK(last_pos == 3);

        size_t string_pos = object_1.pos(static_cast<const char*>(needle_2), starting_position);
        UNIT_CHECK(string_pos == 3);
    }

    void strip_test()
    {
        UnitTestManager::UnitTest test("strip");

        RexxString object_1{"###Junk###"};
        const char kill_char = '#';

        object_1 = object_1.strip('B', kill_char);
        UNIT_CHECK(object_1.length() == 4);
        UNIT_CHECK(strcmp(static_cast<const char*>(object_1), "Junk") == 0);
    }

    void substr_test()
    {
        UnitTestManager::UnitTest test("substr");

        RexxString object_1{"Junk"};
        const int starting_position = 2;
        const int count = 2;

        object_1 = object_1.substr(starting_position, count);
        UNIT_CHECK(object_1.length() == 2);
        UNIT_CHECK(strcmp(static_cast<const char*>(object_1), "un") == 0);
    }

    void words_test()
    {
        UnitTestManager::UnitTest test("words");

        RexxString object_1{"I love my junk"};
        RexxString delimiters = " ";

        int count =
            object_1.words(delimiters.length() == 0 ? 0 : static_cast<const char*>(delimiters));
        UNIT_CHECK(count == 4);
    }

    void subword_test()
    {
        UnitTestManager::UnitTest test("subword");

        RexxString object_1{"I love my junk"};
        RexxString delimiters = " ";
        const int starting_position = 2;
        const int count = 2;

        object_1 =
            object_1.subword(starting_position, count,
                             delimiters.length() == 0 ? 0 : static_cast<const char*>(delimiters));
        UNIT_CHECK(object_1.length() == 7);
        UNIT_CHECK(strcmp(static_cast<const char*>(object_1), "love my") == 0);
    }

} // namespace

bool RexxString_tests()
{
    constructor_test();
    IO_test();
    append_test();
    assignment_test();
    left_right_test();
    center_test();
    copy_test();
    erase_test();
    insert_test();
    pos_test();
    strip_test();
    substr_test();
    words_test();
    subword_test();
    return true;
}
