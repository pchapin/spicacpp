/*! \file   base64.hpp
 *  \brief  Declarations of functions to do base64 encoding and decoding.
 *  \author Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef BASE64_HPP
#define BASE64_HPP

#include <cstdio>

#ifdef __cplusplus
extern "C" {
#endif

void base64_encode( std::FILE *infile, std::FILE *outfile );
void base64_decode( std::FILE *infile, std::FILE *outfile );

#ifdef __cplusplus
}
#endif

#endif
