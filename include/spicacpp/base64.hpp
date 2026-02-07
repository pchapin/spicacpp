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

//! Base64 encode
/*!
 * This function does a base64 encoding of a previously opened input file and writes the result
 * to a previously opened output file. The files should be both opened in binary mode, however
 * the output file can be opened in append mode if desired. This function does not close either
 * file. There is no error indication.
 *
 * \param infile The previously opened input file to encode.
 * \param outfile The previously opened output file.
 */
void base64_encode(std::FILE* infile, std::FILE* outfile);

//! Base64 decode
/*!
 * This function does a base64 decoding of a previously opened input file and writes the result
 * to a previously opened output file. The files should be both opened in binary mode, however
 * the output file can be opened in append mode if desired. This function does not close either
 * file. There is no error indication.
 *
 * \param infile The previously opened input file to decode.
 * \param outfile The previously opened output file.
 */
void base64_decode(std::FILE* infile, std::FILE* outfile);

#ifdef __cplusplus
}
#endif

#endif
