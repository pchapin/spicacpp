/*! \file    BoundedList_tests.cpp
 *  \brief   Exercise the string utility functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <iostream>
#include "string_utilities.hpp"

int string_utilities_tests( )
{
  std::string result;
  std::string search = "aabab";

  spica::create_long_string( result, search, 1024*1024, 2, true );
  spica::display_partial_matches( result, search );
  return 0;
}
