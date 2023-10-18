
/*! \file    VeryLong_tests.cpp
 *  \brief   Code to test class VeryLong
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <string>
#include <sstream>

#include "../VeryLong.hpp"
#include "../u_tests.hpp"
#include "../UnitTestManager.hpp"

using namespace spica;

void check_constructor( )
{
    UnitTestManager::UnitTest test( "constructor" );

    VeryLong object;
    UNIT_CHECK( object.number_bits( ) == 0u );
    UNIT_CHECK( object == VeryLong::zero );
    UNIT_CHECK( object == 0 );
}


void check_io( )
{
    UnitTestManager::UnitTest test( "io" );

    std::string raw_digits( "123456789" );

    VeryLong object_1( "12345678900000987654321" );
    VeryLong object_2( "0" );
    VeryLong object_3( 123456L );
    VeryLong object_4( 0L );
    VeryLong object_5( -123456L );
    VeryLong object_6( object_1 );
    VeryLong object_7( object_2 );
    VeryLong object_8( object_5 );
    VeryLong object_9( "000000000000001234" );
    VeryLong object_A( "-1234" );
    VeryLong object_B( "-12345678900000987654321" );
    VeryLong object_C( "-0" );
    VeryLong object_D( raw_digits );

// The helper macro just makes the tests a bit less tedious to type.
#define CHECK_FORMAT(num, result)                  \
            std::ostringstream formatter_ ## num;  \
            formatter_ ## num << object_ ## num;   \
            UNIT_CHECK(formatter_ ## num.str() == result)

    CHECK_FORMAT(1, "12345678900000987654321");
    CHECK_FORMAT(2, "0");
    CHECK_FORMAT(3, "123456");
    CHECK_FORMAT(4, "0");
    CHECK_FORMAT(5, "-123456");
    CHECK_FORMAT(6, "12345678900000987654321");
    CHECK_FORMAT(7, "0");
    CHECK_FORMAT(8, "-123456");
    CHECK_FORMAT(9, "1234");
    CHECK_FORMAT(A, "-1234");
    CHECK_FORMAT(B, "-12345678900000987654321");
    CHECK_FORMAT(C, "0");
    CHECK_FORMAT(D, "123456789");

    // No input testing is done. That should be fixed eventually.
}


void check_relational( )
{
    UnitTestManager::UnitTest test( "relational" );

    VeryLong zero_1( 0L );
    VeryLong zero_2( 0L );
    VeryLong object_1(  "1234567890987654321" );
    VeryLong object_2(  "1234567890987654321" );
    VeryLong object_3(  "5678909876543211234" );
    VeryLong object_4( "-1234567890987654321" );
    VeryLong object_5( "-1234567890987654321" );
    VeryLong object_6( "-5678909876543211234" );

    UNIT_CHECK(   zero_1 == zero_2  );
    UNIT_CHECK( !(zero_1 == object_1) );
    UNIT_CHECK( !(zero_1   < zero_2) );
    UNIT_CHECK(   zero_1   < object_1 );
    UNIT_CHECK( !(object_1 < zero_1) );

    UNIT_CHECK(   object_1 == object_2  );
    UNIT_CHECK( !(object_1 == object_3) );
    UNIT_CHECK(   object_4 == object_5  );
    UNIT_CHECK( !(object_4 == object_1) );
    UNIT_CHECK( !(object_1  < object_2) );
    UNIT_CHECK(   object_1  < object_3  );
    UNIT_CHECK( !(object_3  < object_1) );
    UNIT_CHECK(   object_4  < object_1  );
    UNIT_CHECK( !(object_1  < object_4) );
    UNIT_CHECK(   object_6  < object_4  );
    UNIT_CHECK( !(object_4  < object_6) );
}


void check_assignment( )
{
    UnitTestManager::UnitTest test( "assignment" );

    VeryLong target;
    VeryLong source_1( 0L );
    VeryLong source_2(  "1234567890987654321" );
    VeryLong source_3( "-1234567890987654321" );

    target = source_1; UNIT_CHECK( target == source_1 );
    target = source_2; UNIT_CHECK( target == source_2 );
    target = source_3; UNIT_CHECK( target == source_3 );
}


void check_bit_manipulation( )
{
    UnitTestManager::UnitTest test( "bit_manipulation" );

    VeryLong object_1( 0L );
    VeryLong object_2( 5L );

    UNIT_CHECK( object_1.number_bits( ) == 0 );
    UNIT_CHECK( object_2.number_bits( ) == 3 );

    UNIT_CHECK( object_1.get_bit(  0 ) == 0 &&
                object_1.get_bit(  1 ) == 0 &&
                object_1.get_bit( 33 ) == 0 );
    UNIT_CHECK( object_2.get_bit(  0 ) == 1 &&
                object_2.get_bit(  1 ) == 0 &&
                object_2.get_bit(  2 ) == 1 &&
                object_2.get_bit(  3 ) == 0 &&
                object_2.get_bit( 33 ) == 0 );

    object_1.put_bit(  0, 1 );
    object_1.put_bit( 33, 1 );

    UNIT_CHECK( object_1.get_bit(  0 ) == 1 &&
                object_1.get_bit(  1 ) == 0 &&
                object_1.get_bit( 33 ) == 1 );

    object_2.put_bit( 0, 0 );
    object_2.put_bit( 2, 0 );
    UNIT_CHECK( object_2 == VeryLong::zero );
}


void check_increment_decrement( )
{
    UnitTestManager::UnitTest test( "increment/decrement" );

    VeryLong zero( 0L );
    VeryLong object_1( -1L );
    VeryLong object_2( +1L );
    VeryLong object_3( -1L );
    VeryLong object_4( +1L );

    UNIT_CHECK( ++object_1 == zero && object_1 == zero );
    UNIT_CHECK( --object_2 == zero && object_2 == zero );
    UNIT_CHECK( object_3++ ==   -1 && object_3 == zero );
    UNIT_CHECK( object_4-- ==    1 && object_4 == zero );
}


void check_unary_minus( )
{
    UnitTestManager::UnitTest test( "unary_minus" );

    VeryLong zero( 0L );
    VeryLong object_1(  "1234567890987654321" );
    VeryLong object_2( "-1234567890987654321" );

    UNIT_CHECK(     zero ==     -zero );
    UNIT_CHECK( object_2 == -object_1 );
    UNIT_CHECK( object_1 == -object_2 );
}


void check_plus( )
{
    UnitTestManager::UnitTest test( "plus" );

    VeryLong zero( 0L );
    VeryLong one( 1L );
    VeryLong minus_one( -1L );
    VeryLong target;
    VeryLong object_1( "1234567890987654321" );
    VeryLong object_2( "9999999999999999999" );
    VeryLong object_3( "-1234567890987654321" );
    VeryLong object_4( "65535" );
    VeryLong object_5( "4294967295" );
    VeryLong result_1( "2469135781975308642" );
    VeryLong result_2( "10000000000000000000" );
    VeryLong result_3( "1234567890987654320" );
    VeryLong result_4( "-1234567890987654322" );
    VeryLong result_5( "65536" );
    VeryLong result_6( "4294967296" );

    target = zero;      target += zero;      UNIT_CHECK( target ==     zero );
    target = zero;      target += object_1;  UNIT_CHECK( target == object_1 );
    target = object_1;  target += zero;      UNIT_CHECK( target == object_1 );
    target = object_1;  target += object_1;  UNIT_CHECK( target == result_1 );
    target = object_2;  target += one;       UNIT_CHECK( target == result_2 );
    target = object_1;  target += object_3;  UNIT_CHECK( target ==     zero );
    target = object_1;  target += minus_one; UNIT_CHECK( target == result_3 );
    target = minus_one; target += object_1;  UNIT_CHECK( target == result_3 );
    target = object_3;  target += minus_one; UNIT_CHECK( target == result_4 );
    target = object_4;  target += one;       UNIT_CHECK( target == result_5 );
    target = object_5;  target += one;       UNIT_CHECK( target == result_6 );
}


void check_minus( )
{
    UnitTestManager::UnitTest test( "minus" );

    VeryLong zero( 0L );
    VeryLong one( 1L );
    VeryLong minus_one( -1L );
    VeryLong target;
    VeryLong object_1( "1234567890987654321" );
    VeryLong object_2( "9999999999999999999" );
    VeryLong object_3( "-1234567890987654321" );
    VeryLong object_4( "4294967296" );
    VeryLong object_5( "-4294967296" );
    VeryLong result_1( "5678909876543211234" );
    VeryLong result_2( "10000000000000000000" );
    VeryLong result_3( "2469135781975308642" );
    VeryLong result_4( "1234567890987654322" );
    VeryLong result_5( "-1234567890987654322" );
    VeryLong result_6( "-1234567890987654320" );
    VeryLong result_7( "4294967295" );
    VeryLong result_8( "-4294967295" );

    target = zero;      target -= zero;      UNIT_CHECK( target == zero);
    target = object_1;  target -= zero;      UNIT_CHECK( target == object_1);
    target = object_1;  target -= object_1;  UNIT_CHECK( target == zero);
    target = result_1;  target -= object_1;
        UNIT_CHECK( target == VeryLong( "4444341985555556913" ) );
    target = result_2;  target -= one;       UNIT_CHECK( target == object_2 );
    target = object_1;  target -= object_3;  UNIT_CHECK( target == result_3 );
    target = object_1;  target -= minus_one; UNIT_CHECK( target == result_4 );
    target = minus_one; target -= object_1;  UNIT_CHECK( target == result_5 );
    target = object_3;  target -= minus_one; UNIT_CHECK( target == result_6 );
    target = object_4;  target -= one;       UNIT_CHECK( target == result_7 );
    target = object_5;  target += one;       UNIT_CHECK( target == result_8 );
}


void check_multiply( )
{
    UnitTestManager::UnitTest test( "multiply" );

    VeryLong zero( 0L );
    VeryLong one( 1 );
    VeryLong target;
    VeryLong object_1( "1234567890987654321" );
    VeryLong object_2( "5678909876543211234" );
    VeryLong result_1( "7010999789392912665121155378475842114" );

    target = zero;     target *= zero;     UNIT_CHECK( target == zero );
    target = object_1; target *= zero;     UNIT_CHECK( target == zero );
    target = zero;     target *= object_1; UNIT_CHECK( target == zero );
    target = object_1; target *= one;      UNIT_CHECK( target == object_1 );
    target = one;      target *= object_1; UNIT_CHECK( target == object_1 );
    target = object_1; target *= object_2; UNIT_CHECK( target == result_1 );
}


void check_divide( )
{
    UnitTestManager::UnitTest test( "divide" );

    VeryLong zero(0L);
    VeryLong one(1);
    VeryLong target;
    VeryLong object_1("1234567890987654321");
    VeryLong object_2("5678909876543211234");
    VeryLong object_3("998877665544332211998877665544332211");
    VeryLong object_4("1234567890987654321234567890987654321");
    VeryLong object_5("123456789");
    VeryLong result_1("4");
    VeryLong result_2("809090915806363692");
    VeryLong result_3("10000000008000000074800000597");

    target = zero;     target /= one;      UNIT_CHECK(target == zero);
    target = zero;     target /= object_1; UNIT_CHECK(target == zero);
    target = one;      target /= object_1; UNIT_CHECK(target == zero);
    target = object_1; target /= object_1; UNIT_CHECK(target == one);
    target = object_2; target /= object_1; UNIT_CHECK(target == result_1);
    target = object_3; target /= object_1; UNIT_CHECK(target == result_2);
    target = object_4; target /= object_5; UNIT_CHECK(target == result_3);
}


void check_modulus( )
{
    UnitTestManager::UnitTest test( "modulus" );

    VeryLong zero(0L);
    VeryLong one(1);
    VeryLong target;
    VeryLong object_1("1234567890987654321");
    VeryLong object_2("2469135781975308642");
    VeryLong object_3("5678909876543211234");
    VeryLong object_4("1234567890987654321234567890987654321");
    VeryLong object_5("123456789");
    VeryLong result_1("740638312592593950");
    VeryLong result_2("83951288");

    target = zero;     target %= one;      UNIT_CHECK(target == zero);
    target = one;      target %= one;      UNIT_CHECK(target == zero);
    target = one;      target %= object_1; UNIT_CHECK(target == one);
    target = object_2; target %= object_1; UNIT_CHECK(target == zero);
    target = object_3; target %= object_1; UNIT_CHECK(target == result_1);
    target = object_4; target %= object_5; UNIT_CHECK(target == result_2);
}


bool VeryLong_tests( )
{
    check_constructor( );
    check_relational( );
    check_assignment( );
    check_bit_manipulation( );
    check_increment_decrement( );
    check_unary_minus( );
    check_plus( );
    check_minus( );
    check_multiply( );
    check_divide( );
    check_modulus( );
    return true;
}
