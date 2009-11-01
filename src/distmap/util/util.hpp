/*
 * util.hpp
 *
 *  Created on: Nov 1, 2009
 *      Author: mat
 */

#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <iosfwd>
#include <stdint.h>

void dumpBuffer( std::ostream& ss, const void* ptr, uint32_t size );

#endif /* UTIL_HPP_ */
