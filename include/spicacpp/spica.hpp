/*! \file    spica.hpp
 *  \brief   The master header file for the Spica C++ library.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * To simplify use of the Spica library this header is provided. It includes all other Spica
 * headers. Simply include spica.hpp in your project to access any spica related component.
 */

#ifndef SPICA_HPP
#define SPICA_HPP

/*! \namespace spica
 * \brief Name space enclosing the Spica library.
 *
 * This name space contains all components in the Spica library. No third party or external
 * library components are included in the spica name space.
 */

#include <spicacpp/BinomialHeap.hpp>
#include <spicacpp/BoundedList.hpp>
#include <spicacpp/Graph.hpp>
#include <spicacpp/sorters.hpp>
#include <spicacpp/VeryLong.hpp>

#endif

